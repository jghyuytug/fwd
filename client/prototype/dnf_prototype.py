#!/usr/bin/env python3
"""
DNF Client Prototype
====================
最小可行原型 - 验证核心功能

功能:
1. SDL2窗口 (通过pygame)
2. 加载NPK/IMG精灵
3. 播放动画
4. 键盘控制移动

运行:
    pip install pygame
    python dnf_prototype.py
"""

import os
import sys
import struct
import zlib
import time

# 尝试导入pygame
try:
    import pygame
    from pygame.locals import *
except ImportError:
    print("请安装pygame: pip install pygame")
    sys.exit(1)

# ============================================================================
# NPK解析 (复用已验证的逻辑)
# ============================================================================

# NPK解密密钥
NPK_KEY = bytearray(256)
_base = "puchikon@neople dungeon and fighter DNF"
for i, c in enumerate(_base):
    NPK_KEY[i] = ord(c)
for i in range(len(_base), 255):
    NPK_KEY[i] = ord("DNF"[(i - len(_base)) % 3])
NPK_KEY[255] = 0


def decrypt_npk_name(encrypted: bytes) -> str:
    """XOR解密NPK文件名"""
    decrypted = bytearray(256)
    for i in range(256):
        decrypted[i] = encrypted[i] ^ NPK_KEY[i]
    try:
        end = decrypted.index(0)
    except ValueError:
        end = 256
    return decrypted[:end].decode('utf-8', errors='replace').rstrip('/')


def parse_npk(npk_path: str) -> dict:
    """解析NPK档案"""
    with open(npk_path, 'rb') as f:
        magic = f.read(16)
        if not magic.startswith(b'NeoplePack'):
            raise ValueError(f"Invalid NPK: {magic[:16]}")

        file_count = struct.unpack('<I', f.read(4))[0]

        files = []
        for i in range(file_count):
            offset = struct.unpack('<I', f.read(4))[0]
            size = struct.unpack('<I', f.read(4))[0]
            encrypted_name = f.read(256)
            name = decrypt_npk_name(encrypted_name)
            files.append({'offset': offset, 'size': size, 'name': name})

        return {'files': files, 'path': npk_path}


def read_npk_file(npk: dict, index: int) -> bytes:
    """读取NPK中的文件"""
    entry = npk['files'][index]
    with open(npk['path'], 'rb') as f:
        f.seek(entry['offset'])
        return f.read(entry['size'])


# ============================================================================
# IMG解析 (交错格式)
# ============================================================================

def parse_img(data: bytes) -> dict:
    """解析IMG (交错格式)"""
    if len(data) < 32 or not data[:6].startswith(b'Neople'):
        return None

    image_count = struct.unpack('<I', data[28:32])[0]
    if image_count > 4096:
        return None

    frames = []
    offset = 32  # Header后

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
            'format': fmt,
            'compress': compress,
            'width': width,
            'height': height,
            'size': size,
            'key_x': key_x,
            'key_y': key_y,
            'data_offset': data_offset,
            'is_link': is_link,
            'link_target': size if is_link else -1
        })

        if is_link:
            offset += 36
        else:
            offset += 36 + size

    return {'frames': frames, 'raw': data}


def decode_frame(img: dict, frame_idx: int) -> pygame.Surface:
    """解码IMG帧为pygame Surface"""
    if frame_idx >= len(img['frames']):
        return None

    frame = img['frames'][frame_idx]

    # 处理链接帧
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

    # 解压
    if frame['compress'] == 6:  # zlib
        try:
            pixels = zlib.decompress(compressed)
        except:
            return None
    elif frame['compress'] == 5:  # raw
        pixels = compressed
    else:
        return None

    width, height = frame['width'], frame['height']

    # 创建Surface
    surface = pygame.Surface((width, height), pygame.SRCALPHA)

    # 转换像素格式
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
                a = pixels[i*4]
                r = pixels[i*4+1]
                g = pixels[i*4+2]
                b = pixels[i*4+3]
                surface.set_at((x, y), (r, g, b, a))
    else:
        return None

    return surface


# ============================================================================
# 游戏原型
# ============================================================================

class Sprite:
    """精灵类"""
    def __init__(self, frames: list, offsets: list):
        self.frames = frames  # pygame.Surface列表
        self.offsets = offsets  # (key_x, key_y)列表
        self.current_frame = 0
        self.frame_time = 0.1  # 每帧100ms
        self.elapsed = 0

    def update(self, dt: float):
        """更新动画"""
        self.elapsed += dt
        if self.elapsed >= self.frame_time:
            self.elapsed = 0
            self.current_frame = (self.current_frame + 1) % len(self.frames)

    def draw(self, screen: pygame.Surface, x: int, y: int, flip: bool = False):
        """绘制精灵"""
        frame = self.frames[self.current_frame]
        if frame is None:
            return

        ox, oy = self.offsets[self.current_frame]

        if flip:
            frame = pygame.transform.flip(frame, True, False)
            ox = frame.get_width() - ox

        screen.blit(frame, (x - ox, y - oy))


class Player:
    """玩家类"""
    def __init__(self, sprite: Sprite):
        self.sprite = sprite
        self.x = 400.0
        self.y = 400.0
        self.speed = 200.0  # 像素/秒
        self.facing_left = False

    def update(self, dt: float, keys):
        """更新玩家"""
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

        # 归一化对角移动
        if dx != 0 and dy != 0:
            dx *= 0.707
            dy *= 0.707

        self.x += dx * self.speed * dt
        self.y += dy * self.speed * dt

        # 边界检查
        self.x = max(50, min(750, self.x))
        self.y = max(200, min(550, self.y))

        self.sprite.update(dt)

    def draw(self, screen: pygame.Surface):
        """绘制玩家"""
        self.sprite.draw(screen, int(self.x), int(self.y), self.facing_left)


def load_character_sprite(npk_path: str, img_name: str) -> Sprite:
    """从NPK加载角色精灵"""
    print(f"Loading NPK: {npk_path}")
    npk = parse_npk(npk_path)

    # 查找IMG
    img_data = None
    for i, f in enumerate(npk['files']):
        if img_name in f['name']:
            print(f"  Found: {f['name']}")
            img_data = read_npk_file(npk, i)
            break

    if img_data is None:
        print(f"  IMG not found: {img_name}")
        return None

    # 解析IMG
    img = parse_img(img_data)
    if img is None:
        print("  Failed to parse IMG")
        return None

    print(f"  Frames: {len(img['frames'])}")

    # 解码所有帧
    frames = []
    offsets = []

    for i, f in enumerate(img['frames']):
        if f['is_link']:
            # 链接帧 - 使用目标帧
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

        if i < 5:
            status = "OK" if frames[-1] else "FAIL"
            print(f"    Frame {i}: {f['width']}x{f['height']} [{status}]")

    # 过滤有效帧
    valid_frames = [(f, o) for f, o in zip(frames, offsets) if f is not None]
    if not valid_frames:
        print("  No valid frames!")
        return None

    frames, offsets = zip(*valid_frames)
    print(f"  Valid frames: {len(frames)}")

    return Sprite(list(frames), list(offsets))


def main():
    """主函数"""
    # 初始化pygame
    pygame.init()
    screen = pygame.display.set_mode((800, 600))
    pygame.display.set_caption("DNF Prototype - Arrow keys to move, ESC to quit")
    clock = pygame.time.Clock()

    # 加载字体
    font = pygame.font.Font(None, 24)

    # 资源路径
    npk_base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    npk_file = os.path.join(npk_base, "sprite_character.NPK")

    # 加载角色精灵
    print("\n" + "="*60)
    print("DNF Client Prototype")
    print("="*60 + "\n")

    player_sprite = load_character_sprite(npk_file, "fighter.img")

    if player_sprite is None:
        print("\nFailed to load sprite, using placeholder")
        # 创建占位精灵
        placeholder = pygame.Surface((50, 100), pygame.SRCALPHA)
        pygame.draw.rect(placeholder, (255, 0, 255), (0, 0, 50, 100))
        pygame.draw.rect(placeholder, (0, 0, 0), (0, 0, 50, 100), 2)
        player_sprite = Sprite([placeholder], [(25, 100)])

    player = Player(player_sprite)

    # 背景色
    bg_color = (40, 44, 52)
    ground_color = (60, 65, 75)

    print("\n" + "="*60)
    print("Controls:")
    print("  Arrow Keys - Move")
    print("  ESC - Quit")
    print("="*60 + "\n")

    # 主循环
    running = True
    while running:
        dt = clock.tick(60) / 1000.0  # 60 FPS

        # 事件处理
        for event in pygame.event.get():
            if event.type == QUIT:
                running = False
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    running = False

        # 更新
        keys = pygame.key.get_pressed()
        player.update(dt, keys)

        # 渲染
        screen.fill(bg_color)

        # 地面
        pygame.draw.rect(screen, ground_color, (0, 450, 800, 150))
        pygame.draw.line(screen, (80, 85, 95), (0, 450), (800, 450), 2)

        # 玩家
        player.draw(screen)

        # HUD
        fps = clock.get_fps()
        fps_text = font.render(f"FPS: {fps:.1f}", True, (200, 200, 200))
        screen.blit(fps_text, (10, 10))

        pos_text = font.render(f"Pos: ({player.x:.0f}, {player.y:.0f})", True, (200, 200, 200))
        screen.blit(pos_text, (10, 30))

        frame_text = font.render(f"Frame: {player.sprite.current_frame}/{len(player.sprite.frames)}", True, (200, 200, 200))
        screen.blit(frame_text, (10, 50))

        info_text = font.render("Arrow Keys: Move | ESC: Quit", True, (150, 150, 150))
        screen.blit(info_text, (10, 570))

        pygame.display.flip()

    pygame.quit()
    print("Goodbye!")


if __name__ == "__main__":
    main()
