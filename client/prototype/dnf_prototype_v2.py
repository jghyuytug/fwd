#!/usr/bin/env python3
"""
DNF Client Prototype V2
=======================
扩展原型 - 多角色、地图、怪物、攻击

功能:
1. 多角色选择 (1-7键)
2. 地图背景
3. 怪物精灵
4. 攻击动作 (Z键)
5. 碰撞检测

运行:
    python dnf_prototype_v2.py
"""

import os
import sys
import struct
import zlib
import time
import random
import math

try:
    import pygame
    from pygame.locals import *
except ImportError:
    print("请安装pygame: pip install pygame")
    sys.exit(1)

# ============================================================================
# NPK/IMG 解析 (复用)
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
        for i in range(file_count):
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
            'data_offset': data_offset, 'is_link': is_link,
            'link_target': size if is_link else -1
        })

        offset = offset + 36 if is_link else offset + 36 + size

    return {'frames': frames, 'raw': data}


def decode_frame_fast(img: dict, frame_idx: int) -> pygame.Surface:
    """快速解码 - 使用pygame的fromstring"""
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

    # 转换为RGBA
    if frame['format'] == 14:  # ARGB1555
        if len(pixels) < width * height * 2:
            return None
        rgba = bytearray(width * height * 4)
        for i in range(width * height):
            p = struct.unpack('<H', pixels[i*2:i*2+2])[0]
            a = 255 if (p & 0x8000) else 0
            r = ((p >> 10) & 0x1F) * 255 // 31
            g = ((p >> 5) & 0x1F) * 255 // 31
            b = (p & 0x1F) * 255 // 31
            rgba[i*4:i*4+4] = bytes([r, g, b, a])
        surface = pygame.image.fromstring(bytes(rgba), (width, height), 'RGBA')
        return surface

    elif frame['format'] == 16:  # ARGB8888
        if len(pixels) < width * height * 4:
            return None
        rgba = bytearray(width * height * 4)
        for i in range(width * height):
            a, r, g, b = pixels[i*4], pixels[i*4+1], pixels[i*4+2], pixels[i*4+3]
            rgba[i*4:i*4+4] = bytes([r, g, b, a])
        surface = pygame.image.fromstring(bytes(rgba), (width, height), 'RGBA')
        return surface

    return None


# ============================================================================
# 游戏类
# ============================================================================

class SpriteSheet:
    """精灵表 - 缓存所有帧"""
    def __init__(self, frames: list, offsets: list):
        self.frames = frames
        self.offsets = offsets

    @staticmethod
    def load_from_npk(npk_path: str, img_name: str):
        try:
            npk = parse_npk(npk_path)
        except:
            return None

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
                surface = decode_frame_fast(img, i)
                frames.append(surface)
                offsets.append((f['key_x'], f['key_y']))

        valid = [(f, o) for f, o in zip(frames, offsets) if f is not None]
        if not valid:
            return None

        frames, offsets = zip(*valid)
        return SpriteSheet(list(frames), list(offsets))


class Animation:
    """动画状态"""
    def __init__(self, sheet: SpriteSheet, start: int, end: int, fps: float = 10, loop: bool = True):
        self.sheet = sheet
        self.start = start
        self.end = min(end, len(sheet.frames) - 1)
        self.fps = fps
        self.loop = loop
        self.current = start
        self.elapsed = 0
        self.finished = False

    def reset(self):
        self.current = self.start
        self.elapsed = 0
        self.finished = False

    def update(self, dt: float):
        if self.finished:
            return

        self.elapsed += dt
        frame_time = 1.0 / self.fps

        while self.elapsed >= frame_time:
            self.elapsed -= frame_time
            self.current += 1

            if self.current > self.end:
                if self.loop:
                    self.current = self.start
                else:
                    self.current = self.end
                    self.finished = True

    def draw(self, screen: pygame.Surface, x: int, y: int, flip: bool = False):
        if self.current >= len(self.sheet.frames):
            return

        frame = self.sheet.frames[self.current]
        if frame is None:
            return

        ox, oy = self.sheet.offsets[self.current]

        if flip:
            frame = pygame.transform.flip(frame, True, False)
            ox = frame.get_width() - ox

        screen.blit(frame, (x - ox, y - oy))


class Entity:
    """游戏实体基类"""
    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y
        self.vx = 0
        self.vy = 0
        self.facing_left = False
        self.animations = {}
        self.current_anim = None
        self.hitbox = pygame.Rect(0, 0, 40, 80)

    def set_animation(self, name: str):
        if name in self.animations and self.current_anim != name:
            self.current_anim = name
            self.animations[name].reset()

    def update(self, dt: float):
        self.x += self.vx * dt
        self.y += self.vy * dt

        if self.current_anim and self.current_anim in self.animations:
            self.animations[self.current_anim].update(dt)

        # 更新hitbox位置
        self.hitbox.center = (int(self.x), int(self.y - 40))

    def draw(self, screen: pygame.Surface):
        if self.current_anim and self.current_anim in self.animations:
            self.animations[self.current_anim].draw(
                screen, int(self.x), int(self.y), self.facing_left
            )


class Player(Entity):
    """玩家"""
    def __init__(self, x: float, y: float, sheet: SpriteSheet):
        super().__init__(x, y)
        self.speed = 250.0
        self.attacking = False
        self.attack_cooldown = 0

        # 动画设置 (根据精灵帧数分配)
        total_frames = len(sheet.frames)
        idle_end = min(3, total_frames - 1)
        walk_start = idle_end + 1
        walk_end = min(walk_start + 5, total_frames - 1)

        self.animations['idle'] = Animation(sheet, 0, idle_end, fps=8)
        self.animations['walk'] = Animation(sheet, walk_start, walk_end, fps=12)

        self.current_anim = 'idle'

    def handle_input(self, keys, dt: float):
        dx, dy = 0, 0

        if not self.attacking:
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

        # 归一化
        if dx != 0 and dy != 0:
            dx *= 0.707
            dy *= 0.707

        self.vx = dx * self.speed
        self.vy = dy * self.speed

        # 攻击
        if keys[K_z] and self.attack_cooldown <= 0:
            self.attacking = True
            self.attack_cooldown = 0.5

        # 更新冷却
        if self.attack_cooldown > 0:
            self.attack_cooldown -= dt

        # 选择动画
        if self.attacking:
            if self.attack_cooldown <= 0.3:
                self.attacking = False
        elif dx != 0 or dy != 0:
            self.set_animation('walk')
        else:
            self.set_animation('idle')

    def update(self, dt: float):
        super().update(dt)

        # 边界
        self.x = max(50, min(750, self.x))
        self.y = max(350, min(550, self.y))


class Monster(Entity):
    """怪物"""
    def __init__(self, x: float, y: float, sheet: SpriteSheet):
        super().__init__(x, y)
        self.speed = 80.0
        self.hp = 100
        self.max_hp = 100
        self.state = 'idle'
        self.state_timer = 0
        self.target = None
        self.hit_flash = 0

        total = len(sheet.frames)
        self.animations['idle'] = Animation(sheet, 0, min(2, total-1), fps=6)
        self.current_anim = 'idle'

    def take_damage(self, damage: int):
        self.hp -= damage
        self.hit_flash = 0.1
        if self.hp <= 0:
            self.hp = 0
            self.state = 'dead'

    def update(self, dt: float):
        # AI逻辑
        if self.state == 'idle':
            self.state_timer -= dt
            if self.state_timer <= 0:
                self.state = 'patrol'
                self.state_timer = random.uniform(1, 3)
                angle = random.uniform(0, 2 * 3.14159)
                self.vx = math.cos(angle) * self.speed
                self.vy = math.sin(angle) * self.speed * 0.3
                self.facing_left = self.vx < 0

        elif self.state == 'patrol':
            self.state_timer -= dt
            if self.state_timer <= 0:
                self.state = 'idle'
                self.state_timer = random.uniform(0.5, 2)
                self.vx = 0
                self.vy = 0

        elif self.state == 'dead':
            self.vx = 0
            self.vy = 0

        super().update(dt)

        # 边界
        self.x = max(100, min(700, self.x))
        self.y = max(380, min(520, self.y))

        # 闪烁
        if self.hit_flash > 0:
            self.hit_flash -= dt

    def draw(self, screen: pygame.Surface):
        if self.state == 'dead':
            return

        # 闪烁效果
        if self.hit_flash > 0:
            # 简单的红色叠加
            pass

        super().draw(screen)

        # 血条
        bar_width = 40
        bar_height = 4
        hp_ratio = self.hp / self.max_hp
        bar_x = int(self.x - bar_width // 2)
        bar_y = int(self.y - 90)

        pygame.draw.rect(screen, (60, 60, 60), (bar_x, bar_y, bar_width, bar_height))
        pygame.draw.rect(screen, (220, 50, 50), (bar_x, bar_y, int(bar_width * hp_ratio), bar_height))


class Game:
    """游戏主类"""
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((800, 600))
        pygame.display.set_caption("DNF Prototype V2 - 1-7:角色 Z:攻击 方向键:移动")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 24)
        self.running = True

        # 资源路径
        self.npk_base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"

        # 角色列表
        self.character_names = [
            ('fighter', 'Fighter'),
            ('swordman', 'Swordman'),
            ('gunner', 'Gunner'),
            ('mage', 'Mage'),
            ('priest', 'Priest'),
            ('atgunner', 'Gunner F'),
        ]

        # 加载精灵
        print("\nLoading sprites...")
        self.character_sheets = {}
        for img_name, display_name in self.character_names:
            sheet = SpriteSheet.load_from_npk(
                os.path.join(self.npk_base, "sprite_character.NPK"),
                f"{img_name}.img"
            )
            if sheet:
                self.character_sheets[img_name] = sheet
                print(f"  Loaded {display_name}: {len(sheet.frames)} frames")

        # 怪物精灵 (使用monsterface)
        self.monster_sheet = SpriteSheet.load_from_npk(
            os.path.join(self.npk_base, "sprite_monster.NPK"),
            "monsterface.img"
        )
        if self.monster_sheet:
            print(f"  Loaded Monster: {len(self.monster_sheet.frames)} frames")

        # 创建玩家
        self.current_char = 'fighter'
        self.player = self.create_player()

        # 创建怪物
        self.monsters = []
        self.spawn_monsters(5)

        # 游戏状态
        self.score = 0
        self.combo = 0
        self.combo_timer = 0

        print("\nGame started!")
        print("Controls: 1-6 change character, Z attack, Arrow keys move")

    def create_player(self) -> Player:
        sheet = self.character_sheets.get(self.current_char)
        if sheet is None:
            # 占位
            placeholder = pygame.Surface((50, 100), pygame.SRCALPHA)
            pygame.draw.rect(placeholder, (100, 150, 255), (0, 0, 50, 100))
            sheet = SpriteSheet([placeholder], [(25, 100)])
        return Player(400, 450, sheet)

    def spawn_monsters(self, count: int):
        self.monsters.clear()
        if self.monster_sheet is None:
            return

        for i in range(count):
            x = random.randint(150, 650)
            y = random.randint(400, 500)
            monster = Monster(x, y, self.monster_sheet)
            self.monsters.append(monster)

    def check_attack(self):
        """检查攻击碰撞"""
        if not self.player.attacking:
            return

        # 攻击范围
        attack_range = 80
        attack_rect = pygame.Rect(0, 0, attack_range, 60)

        if self.player.facing_left:
            attack_rect.right = self.player.x
        else:
            attack_rect.left = self.player.x

        attack_rect.centery = self.player.y - 40

        for monster in self.monsters:
            if monster.state == 'dead':
                continue

            if attack_rect.colliderect(monster.hitbox):
                damage = random.randint(15, 25)
                monster.take_damage(damage)
                self.combo += 1
                self.combo_timer = 2.0
                self.score += damage * self.combo

                if monster.hp <= 0:
                    self.score += 100

    def update(self, dt: float):
        keys = pygame.key.get_pressed()

        # 角色切换
        char_keys = [K_1, K_2, K_3, K_4, K_5, K_6]
        for i, key in enumerate(char_keys):
            if keys[key] and i < len(self.character_names):
                new_char = self.character_names[i][0]
                if new_char != self.current_char and new_char in self.character_sheets:
                    self.current_char = new_char
                    old_pos = (self.player.x, self.player.y)
                    self.player = self.create_player()
                    self.player.x, self.player.y = old_pos

        # 更新玩家
        self.player.handle_input(keys, dt)
        self.player.update(dt)

        # 检查攻击
        self.check_attack()

        # 更新怪物
        alive_count = 0
        for monster in self.monsters:
            monster.update(dt)
            if monster.state != 'dead':
                alive_count += 1

        # 重生怪物
        if alive_count == 0:
            self.spawn_monsters(5)

        # 连击计时
        if self.combo_timer > 0:
            self.combo_timer -= dt
            if self.combo_timer <= 0:
                self.combo = 0

    def draw(self):
        # 背景
        self.screen.fill((35, 40, 50))

        # 地面渐变
        for y in range(400, 600):
            ratio = (y - 400) / 200
            color = (
                int(45 + ratio * 20),
                int(50 + ratio * 15),
                int(60 + ratio * 10)
            )
            pygame.draw.line(self.screen, color, (0, y), (800, y))

        # 地平线
        pygame.draw.line(self.screen, (80, 90, 100), (0, 400), (800, 400), 2)

        # 排序绘制 (按Y坐标)
        entities = [(self.player.y, 'player', self.player)]
        for m in self.monsters:
            if m.state != 'dead':
                entities.append((m.y, 'monster', m))

        entities.sort(key=lambda e: e[0])

        for _, etype, entity in entities:
            entity.draw(self.screen)

        # 攻击效果
        if self.player.attacking and self.player.attack_cooldown > 0.3:
            effect_x = self.player.x + (-50 if self.player.facing_left else 50)
            effect_y = self.player.y - 40
            pygame.draw.circle(self.screen, (255, 200, 100), (int(effect_x), int(effect_y)), 20, 3)

        # HUD
        fps = self.clock.get_fps()
        self.draw_text(f"FPS: {fps:.0f}", 10, 10)
        self.draw_text(f"Score: {self.score}", 10, 30)

        if self.combo > 1:
            combo_text = f"{self.combo} COMBO!"
            text_surf = self.font.render(combo_text, True, (255, 200, 50))
            self.screen.blit(text_surf, (400 - text_surf.get_width()//2, 100))

        # 角色名
        for i, (name, display) in enumerate(self.character_names):
            color = (255, 255, 100) if name == self.current_char else (150, 150, 150)
            self.draw_text(f"{i+1}:{display}", 650, 10 + i * 20, color)

        # 操作提示
        self.draw_text("Arrow:Move Z:Attack 1-6:Character", 10, 575, (120, 120, 120))

        pygame.display.flip()

    def draw_text(self, text: str, x: int, y: int, color=(200, 200, 200)):
        surf = self.font.render(text, True, color)
        self.screen.blit(surf, (x, y))

    def run(self):
        while self.running:
            dt = self.clock.tick(60) / 1000.0

            for event in pygame.event.get():
                if event.type == QUIT:
                    self.running = False
                elif event.type == KEYDOWN:
                    if event.key == K_ESCAPE:
                        self.running = False
                    elif event.key == K_r:
                        self.spawn_monsters(5)
                        self.score = 0

            self.update(dt)
            self.draw()

        pygame.quit()


if __name__ == "__main__":
    game = Game()
    game.run()
