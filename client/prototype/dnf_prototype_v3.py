#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DNF Client Prototype V3
=======================
集成PVF数据读取

功能:
1. PVF文件解析和提取
2. ANI动画定义读取
3. NPK/IMG精灵加载
4. 显示PVF中的数据

运行:
    pip install pygame
    python dnf_prototype_v3.py
"""

import os
import sys
import struct
import zlib
import time
from pathlib import Path
from dataclasses import dataclass
from typing import List, Optional, Dict, Tuple

# pygame
try:
    import pygame
    from pygame.locals import *
except ImportError:
    print("Please install pygame: pip install pygame")
    sys.exit(1)


# ============================================================================
# PVF Reader (simplified from pvf_reader.py)
# ============================================================================

@dataclass
class PVFHeader:
    tag_length: int
    tag_string: str
    revision: int
    index_header_size: int
    index_crc32: int
    index_count: int


@dataclass
class PVFEntry:
    name: str
    offset: int
    size: int
    decrypt_key: int
    hash_value: int


class PVFReader:
    """PVF Reader with auto key detection"""

    KNOWN_KEYS = [0x81A79011, 0x00000000, 0xCE24D9AF]

    def __init__(self, filepath: str):
        self.filepath = Path(filepath)
        self.header: Optional[PVFHeader] = None
        self.header_size: int = 0
        self.indexes: List[PVFEntry] = []
        self.fp = None
        self.additional_key = 0x81A79011
        self._index_by_name: Dict[str, PVFEntry] = {}

    def open(self) -> bool:
        try:
            self.fp = open(self.filepath, 'rb')
            tag_length = struct.unpack('<I', self.fp.read(4))[0]
            tag_string = self.fp.read(tag_length).decode('utf-8', errors='ignore')
            revision = struct.unpack('<I', self.fp.read(4))[0]
            index_header_size = struct.unpack('<I', self.fp.read(4))[0]
            index_crc32 = struct.unpack('<I', self.fp.read(4))[0]
            index_count = struct.unpack('<I', self.fp.read(4))[0]

            self.header = PVFHeader(tag_length, tag_string, revision,
                                   index_header_size, index_crc32, index_count)
            self.header_size = self.fp.tell()
            return True
        except Exception as e:
            print(f"[PVF] Open failed: {e}")
            return False

    def _ror32(self, value: int, shift: int) -> int:
        return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF

    def _make_crc_table(self):
        table = []
        for i in range(256):
            c = i
            for _ in range(8):
                c = 0xEDB88320 ^ (c >> 1) if c & 1 else c >> 1
            table.append(c)
        return table

    def _try_decrypt(self, data: bytes, key: int) -> Optional[bytes]:
        size = len(data)
        if size & 3:
            return None

        crc_table = self._make_crc_table()
        crc = (~self.header.index_count) & 0xFFFFFFFF
        decrypt_key = self.header.index_crc32

        decrypted = bytearray(data)
        for i in range(0, size, 4):
            dword = struct.unpack('<I', decrypted[i:i+4])[0]
            if key:
                dword ^= key
            dword = self._ror32(decrypt_key ^ dword, 6)
            struct.pack_into('<I', decrypted, i, dword)
            for j in range(4):
                crc = (crc >> 8) ^ crc_table[(crc ^ decrypted[i+j]) & 0xFF]

        if (~crc & 0xFFFFFFFF) == self.header.index_crc32:
            return bytes(decrypted)
        return None

    def parse_index(self, max_entries: int = 0) -> bool:
        if not self.header:
            return False

        encrypted = self.fp.read(self.header.index_header_size)
        if len(encrypted) != self.header.index_header_size:
            return False

        decrypted = None
        for key in self.KNOWN_KEYS:
            result = self._try_decrypt(encrypted, key)
            if result:
                decrypted = result
                self.additional_key = key
                break

        if not decrypted:
            return False

        offset = 0
        count = max_entries if max_entries > 0 else self.header.index_count
        count = min(count, self.header.index_count)

        for _ in range(count):
            try:
                if offset + 4 > len(decrypted):
                    break
                hash_val = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4
                name_len = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4
                name = decrypted[offset:offset+name_len].decode('utf-8', errors='ignore')
                offset += name_len
                size = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4
                dec_key = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4
                file_off = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4

                entry = PVFEntry(name, file_off, size, dec_key, hash_val)
                self.indexes.append(entry)
                self._index_by_name[name.lower().replace('\\', '/')] = entry
            except:
                break

        return len(self.indexes) > 0

    def get_file(self, path: str) -> Optional[PVFEntry]:
        normalized = path.lower().replace('\\', '/')
        return self._index_by_name.get(normalized)

    def extract(self, entry: PVFEntry) -> Optional[bytes]:
        try:
            data_start = self.header_size + self.header.index_header_size
            self.fp.seek(data_start + entry.offset)
            aligned = ((entry.size + 3) // 4) * 4
            encrypted = self.fp.read(aligned)

            decrypted = bytearray(encrypted)
            key = entry.decrypt_key
            for i in range(0, len(decrypted) - (len(decrypted) % 4), 4):
                dword = struct.unpack('<I', decrypted[i:i+4])[0]
                if self.additional_key:
                    dword ^= self.additional_key
                dword = self._ror32(key ^ dword, 6)
                struct.pack_into('<I', decrypted, i, dword)

            return bytes(decrypted[:entry.size])
        except:
            return None

    def close(self):
        if self.fp:
            self.fp.close()


# ============================================================================
# ANI Parser
# ============================================================================

@dataclass
class ANIFrame:
    frame_index: int
    offset_x: int
    offset_y: int
    delay: int


@dataclass
class ANIData:
    img_path: str
    frames: List[ANIFrame]


def parse_ani(data: bytes) -> Optional[ANIData]:
    """Parse ANI animation definition file"""
    try:
        if len(data) < 10:
            return None

        # ANI format: frame_count(2) + unk(2) + path_len(4) + path + frame_data
        offset = 0
        frame_count = struct.unpack('<H', data[offset:offset+2])[0]
        offset += 2

        # Skip unknown
        offset += 2

        # Path length
        path_len = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # IMG path
        img_path = data[offset:offset+path_len].decode('utf-8', errors='ignore')
        offset += path_len

        frames = []
        for i in range(frame_count):
            if offset + 20 > len(data):
                break

            # Frame data structure varies, simplified parsing
            fidx = struct.unpack('<H', data[offset:offset+2])[0]
            offset += 2

            ox = struct.unpack('<h', data[offset:offset+2])[0]
            offset += 2
            oy = struct.unpack('<h', data[offset:offset+2])[0]
            offset += 2

            # Skip rest of frame data
            offset += 14

            frames.append(ANIFrame(fidx, ox, oy, 100))

        return ANIData(img_path, frames)
    except Exception as e:
        print(f"[ANI] Parse error: {e}")
        return None


# ============================================================================
# NPK/IMG Parser (from previous prototype)
# ============================================================================

NPK_KEY = bytearray(256)
_base = "puchikon@neople dungeon and fighter DNF"
for i, c in enumerate(_base):
    NPK_KEY[i] = ord(c)
for i in range(len(_base), 255):
    NPK_KEY[i] = ord("DNF"[(i - len(_base)) % 3])
NPK_KEY[255] = 0


def decrypt_npk_name(encrypted: bytes) -> str:
    decrypted = bytearray(256)
    for i in range(256):
        decrypted[i] = encrypted[i] ^ NPK_KEY[i]
    try:
        end = decrypted.index(0)
    except ValueError:
        end = 256
    return decrypted[:end].decode('utf-8', errors='replace').rstrip('/')


def parse_npk(npk_path: str) -> dict:
    with open(npk_path, 'rb') as f:
        magic = f.read(16)
        if not magic.startswith(b'NeoplePack'):
            raise ValueError(f"Invalid NPK")
        file_count = struct.unpack('<I', f.read(4))[0]
        files = []
        for _ in range(file_count):
            offset = struct.unpack('<I', f.read(4))[0]
            size = struct.unpack('<I', f.read(4))[0]
            encrypted_name = f.read(256)
            name = decrypt_npk_name(encrypted_name)
            files.append({'offset': offset, 'size': size, 'name': name})
        return {'files': files, 'path': npk_path}


def read_npk_file(npk: dict, index: int) -> bytes:
    entry = npk['files'][index]
    with open(npk['path'], 'rb') as f:
        f.seek(entry['offset'])
        return f.read(entry['size'])


def parse_img(data: bytes) -> dict:
    if len(data) < 32 or not data[:6].startswith(b'Neople'):
        return None
    image_count = struct.unpack('<I', data[28:32])[0]
    if image_count > 4096:
        return None

    frames = []
    offset = 32

    for i in range(image_count):
        if offset + 36 > len(data):
            break
        fmt = struct.unpack('<I', data[offset:offset+4])[0]
        compress = struct.unpack('<I', data[offset+4:offset+8])[0]
        width = struct.unpack('<i', data[offset+8:offset+12])[0]
        height = struct.unpack('<i', data[offset+12:offset+16])[0]
        size = struct.unpack('<I', data[offset+16:offset+20])[0]
        key_x = struct.unpack('<i', data[offset+20:offset+24])[0]
        key_y = struct.unpack('<i', data[offset+24:offset+28])[0]
        data_offset = offset + 36
        is_link = (fmt == 0 and compress == 0 and width == 0 and height == 0)
        frames.append({
            'format': fmt, 'compress': compress,
            'width': width, 'height': height, 'size': size,
            'key_x': key_x, 'key_y': key_y,
            'data_offset': data_offset,
            'is_link': is_link, 'link_target': size if is_link else -1
        })
        offset += 36 if is_link else 36 + size

    return {'frames': frames, 'raw': data}


def decode_frame(img: dict, frame_idx: int) -> pygame.Surface:
    if frame_idx >= len(img['frames']):
        return None
    frame = img['frames'][frame_idx]

    max_links = 10
    while frame['is_link'] and max_links > 0:
        target = frame['link_target']
        if target >= len(img['frames']):
            return None
        frame = img['frames'][target]
        max_links -= 1

    if frame['width'] <= 0 or frame['height'] <= 0:
        return None

    data = img['raw']
    compressed = data[frame['data_offset']:frame['data_offset'] + frame['size']]

    if frame['compress'] == 6:
        try:
            pixels = zlib.decompress(compressed)
        except:
            return None
    elif frame['compress'] == 5:
        pixels = compressed
    else:
        return None

    width, height = frame['width'], frame['height']
    surface = pygame.Surface((width, height), pygame.SRCALPHA)

    if frame['format'] == 14:  # ARGB1555
        if len(pixels) < width * height * 2:
            return None
        for y in range(height):
            for x in range(width):
                i = y * width + x
                p = struct.unpack('<H', pixels[i*2:i*2+2])[0]
                a = 255 if (p & 0x8000) else 0
                r = ((p >> 10) & 0x1F) * 255 // 31
                g = ((p >> 5) & 0x1F) * 255 // 31
                b = (p & 0x1F) * 255 // 31
                surface.set_at((x, y), (r, g, b, a))
    elif frame['format'] == 15:  # ARGB4444
        if len(pixels) < width * height * 2:
            return None
        for y in range(height):
            for x in range(width):
                i = y * width + x
                p = struct.unpack('<H', pixels[i*2:i*2+2])[0]
                a = ((p >> 12) & 0xF) * 255 // 15
                r = ((p >> 8) & 0xF) * 255 // 15
                g = ((p >> 4) & 0xF) * 255 // 15
                b = (p & 0xF) * 255 // 15
                surface.set_at((x, y), (r, g, b, a))
    elif frame['format'] == 16:  # ARGB8888
        if len(pixels) < width * height * 4:
            return None
        for y in range(height):
            for x in range(width):
                i = y * width + x
                a, r, g, b = pixels[i*4], pixels[i*4+1], pixels[i*4+2], pixels[i*4+3]
                surface.set_at((x, y), (r, g, b, a))
    else:
        return None

    return surface


# ============================================================================
# Game Classes
# ============================================================================

class Sprite:
    def __init__(self, frames: list, offsets: list):
        self.frames = frames
        self.offsets = offsets
        self.current_frame = 0
        self.frame_time = 0.1
        self.elapsed = 0

    def update(self, dt: float):
        self.elapsed += dt
        if self.elapsed >= self.frame_time:
            self.elapsed = 0
            self.current_frame = (self.current_frame + 1) % len(self.frames)

    def draw(self, screen: pygame.Surface, x: int, y: int, flip: bool = False):
        if not self.frames:
            return
        frame = self.frames[self.current_frame]
        if frame is None:
            return
        ox, oy = self.offsets[self.current_frame]
        if flip:
            frame = pygame.transform.flip(frame, True, False)
            ox = frame.get_width() - ox
        screen.blit(frame, (x - ox, y - oy))


class Player:
    def __init__(self, sprite: Sprite):
        self.sprite = sprite
        self.x = 400.0
        self.y = 400.0
        self.speed = 200.0
        self.facing_left = False

    def update(self, dt: float, keys):
        dx, dy = 0, 0
        if keys[K_LEFT]:
            dx = -1
            self.facing_left = True
        if keys[K_RIGHT]:
            dx = 1
            self.facing_left = False
        if keys[K_UP]:
            dy = -1
        if keys[K_DOWN]:
            dy = 1

        if dx != 0 and dy != 0:
            dx *= 0.707
            dy *= 0.707

        self.x += dx * self.speed * dt
        self.y += dy * self.speed * dt
        self.x = max(50, min(750, self.x))
        self.y = max(200, min(550, self.y))
        self.sprite.update(dt)

    def draw(self, screen: pygame.Surface):
        self.sprite.draw(screen, int(self.x), int(self.y), self.facing_left)


# ============================================================================
# Main Game
# ============================================================================

def load_sprite_from_npk(npk_path: str, img_name: str) -> Optional[Sprite]:
    """Load sprite from NPK file"""
    try:
        npk = parse_npk(npk_path)
        img_data = None
        for i, f in enumerate(npk['files']):
            if img_name in f['name']:
                img_data = read_npk_file(npk, i)
                break
        if img_data is None:
            return None

        img = parse_img(img_data)
        if img is None:
            return None

        frames = []
        offsets = []
        for i, f in enumerate(img['frames']):
            if f['is_link']:
                target = f['link_target']
                if target < len(frames):
                    frames.append(frames[target])
                    offsets.append(offsets[target])
                else:
                    frames.append(None)
                    offsets.append((0, 0))
            else:
                surface = decode_frame(img, i)
                frames.append(surface)
                offsets.append((f['key_x'], f['key_y']))

        valid = [(f, o) for f, o in zip(frames, offsets) if f is not None]
        if not valid:
            return None
        frames, offsets = zip(*valid)
        return Sprite(list(frames), list(offsets))
    except Exception as e:
        print(f"[NPK] Load error: {e}")
        return None


def main():
    pygame.init()
    screen = pygame.display.set_mode((900, 650))
    pygame.display.set_caption("DNF Prototype V3 - PVF Integration")
    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 20)

    # Paths
    npk_base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    pvf_path = "C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf"

    print("=" * 60)
    print("DNF Client Prototype V3 - PVF Integration")
    print("=" * 60)

    # Load PVF
    pvf_info = []
    pvf = PVFReader(pvf_path)
    if pvf.open():
        print(f"[PVF] Opened: {pvf.header.index_count} files")
        pvf_info.append(f"PVF: {pvf.header.index_count} files")
        pvf_info.append(f"GUID: {pvf.header.tag_string[:20]}...")
        pvf_info.append(f"Rev: {pvf.header.revision}")

        if pvf.parse_index(max_entries=50000):
            print(f"[PVF] Parsed {len(pvf.indexes)} entries")
            pvf_info.append(f"Parsed: {len(pvf.indexes)} entries")

            # Count file types
            types: Dict[str, int] = {}
            for entry in pvf.indexes:
                ext = entry.name.split('.')[-1].lower() if '.' in entry.name else 'no_ext'
                types[ext] = types.get(ext, 0) + 1

            # Top 5 types
            top_types = sorted(types.items(), key=lambda x: -x[1])[:5]
            pvf_info.append("Top file types:")
            for ext, count in top_types:
                pvf_info.append(f"  .{ext}: {count}")

            # Try to extract an ANI file
            ani_entry = pvf.get_file("creature/common/animation/levelup_dodgeunder.ani")
            if ani_entry:
                ani_data = pvf.extract(ani_entry)
                if ani_data:
                    ani = parse_ani(ani_data)
                    if ani:
                        pvf_info.append(f"ANI: {ani.img_path}")
                        pvf_info.append(f"  Frames: {len(ani.frames)}")
    else:
        pvf_info.append("PVF: Failed to open")

    # Load character sprite
    npk_file = os.path.join(npk_base, "sprite_character.NPK")
    player_sprite = load_sprite_from_npk(npk_file, "fighter.img")

    if player_sprite is None:
        print("[NPK] Using placeholder sprite")
        placeholder = pygame.Surface((50, 100), pygame.SRCALPHA)
        pygame.draw.rect(placeholder, (255, 0, 255), (0, 0, 50, 100))
        player_sprite = Sprite([placeholder], [(25, 100)])
    else:
        print(f"[NPK] Loaded fighter: {len(player_sprite.frames)} frames")

    player = Player(player_sprite)

    # UI colors
    bg_color = (30, 32, 40)
    panel_color = (45, 48, 58)
    text_color = (200, 200, 200)
    highlight_color = (100, 180, 255)

    print("\nControls:")
    print("  Arrow Keys - Move")
    print("  ESC - Quit")
    print("=" * 60)

    running = True
    while running:
        dt = clock.tick(60) / 1000.0

        for event in pygame.event.get():
            if event.type == QUIT:
                running = False
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    running = False

        keys = pygame.key.get_pressed()
        player.update(dt, keys)

        # Render
        screen.fill(bg_color)

        # Game area
        pygame.draw.rect(screen, (50, 54, 65), (0, 0, 600, 650))

        # Ground
        pygame.draw.rect(screen, (60, 65, 75), (0, 450, 600, 200))
        pygame.draw.line(screen, (80, 85, 95), (0, 450), (600, 450), 2)

        # Player
        player.draw(screen)

        # Info panel
        pygame.draw.rect(screen, panel_color, (610, 10, 280, 630))
        pygame.draw.rect(screen, highlight_color, (610, 10, 280, 630), 1)

        # PVF Info
        y = 20
        title = font.render("PVF Data Info", True, highlight_color)
        screen.blit(title, (620, y))
        y += 25

        for line in pvf_info:
            text = font.render(line, True, text_color)
            screen.blit(text, (620, y))
            y += 18

        # Divider
        y += 10
        pygame.draw.line(screen, highlight_color, (620, y), (880, y), 1)
        y += 15

        # Game Info
        title = font.render("Game State", True, highlight_color)
        screen.blit(title, (620, y))
        y += 25

        info_lines = [
            f"FPS: {clock.get_fps():.1f}",
            f"Player: ({player.x:.0f}, {player.y:.0f})",
            f"Frame: {player.sprite.current_frame}/{len(player.sprite.frames)}",
            f"Facing: {'Left' if player.facing_left else 'Right'}",
        ]
        for line in info_lines:
            text = font.render(line, True, text_color)
            screen.blit(text, (620, y))
            y += 18

        # Divider
        y += 10
        pygame.draw.line(screen, highlight_color, (620, y), (880, y), 1)
        y += 15

        # Controls
        title = font.render("Controls", True, highlight_color)
        screen.blit(title, (620, y))
        y += 25

        controls = [
            "Arrow Keys: Move",
            "ESC: Quit",
        ]
        for line in controls:
            text = font.render(line, True, (150, 150, 150))
            screen.blit(text, (620, y))
            y += 18

        pygame.display.flip()

    pvf.close()
    pygame.quit()
    print("Goodbye!")


if __name__ == "__main__":
    main()
