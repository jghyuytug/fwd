#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DNF Client Prototype V4 - Data Driven
======================================
使用PVF数据驱动游戏逻辑

功能:
1. 从PVF加载真实怪物/技能/装备数据
2. 数据驱动的战斗系统
3. 实体管理和碰撞检测
4. 简单的战斗演示

运行:
    pip install pygame
    python dnf_prototype_v4.py
"""

import os
import sys
import random
import time
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any

# pygame
try:
    import pygame
    from pygame.locals import *
except ImportError:
    print("Please install pygame: pip install pygame")
    sys.exit(1)

# 本地模块
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_stringtable import PVFStringTable
from pvf_formats import (
    SkillParser, MonsterParser, EquipmentParser,
    SkillData, MonsterData, EquipmentData
)


# ============================================================================
# 配置
# ============================================================================

CONFIG = {
    'pvf_path': 'C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf',
    'stringtable_path': 'C:/Users/waw/Desktop/game/stringtable.bin',
    'screen_width': 800,
    'screen_height': 600,
    'fps': 60,
}


# ============================================================================
# 游戏数据管理器
# ============================================================================

class GameDataManager:
    """游戏数据管理器 - 从PVF加载所有数据"""

    def __init__(self):
        self.pvf: Optional[PVFReader] = None
        self.stringtable: Optional[PVFStringTable] = None
        self.skills: Dict[str, SkillData] = {}
        self.monsters: Dict[str, MonsterData] = {}
        self.equipment: Dict[str, EquipmentData] = {}
        self.loaded = False

    def load(self, pvf_path: str, st_path: str, max_items: int = 100) -> bool:
        """加载所有游戏数据"""
        print("[DataManager] Loading game data...")

        # 加载 StringTable
        self.stringtable = PVFStringTable()
        if not self.stringtable.load_from_file(st_path):
            print("[DataManager] Failed to load StringTable")
            return False

        # 加载 PVF
        self.pvf = PVFReader(pvf_path)
        if not self.pvf.open():
            print("[DataManager] Failed to open PVF")
            return False

        if not self.pvf.parse_index(max_entries=50000):
            print("[DataManager] Failed to parse PVF index")
            return False

        # 加载数据
        self._load_monsters(max_items)
        self._load_skills(max_items)
        self._load_equipment(max_items)

        self.loaded = True
        print(f"[DataManager] Loaded: {len(self.monsters)} monsters, "
              f"{len(self.skills)} skills, {len(self.equipment)} equipment")

        return True

    def _load_monsters(self, max_items: int):
        """加载怪物数据"""
        count = 0
        for entry in self.pvf.indexes:
            if entry.name.endswith('.mob'):
                data = self.pvf.extract_file(entry)
                if data:
                    try:
                        monster = MonsterParser.parse(data, entry.name)
                        if monster.hp > 0:  # 只保存有效怪物
                            self.monsters[entry.name] = monster
                            count += 1
                            if count >= max_items:
                                break
                    except:
                        pass

    def _load_skills(self, max_items: int):
        """加载技能数据"""
        count = 0
        for entry in self.pvf.indexes:
            if entry.name.endswith('.skl') and 'fighter' in entry.name:
                data = self.pvf.extract_file(entry)
                if data:
                    try:
                        skill = SkillParser.parse(data, entry.name)
                        self.skills[entry.name] = skill
                        count += 1
                        if count >= max_items:
                            break
                    except:
                        pass

    def _load_equipment(self, max_items: int):
        """加载装备数据"""
        count = 0
        for entry in self.pvf.indexes:
            if entry.name.endswith('.equ') and 'weapon' in entry.name:
                data = self.pvf.extract_file(entry)
                if data:
                    try:
                        equip = EquipmentParser.parse(data, entry.name)
                        if equip.phys_attack > 0:
                            self.equipment[entry.name] = equip
                            count += 1
                            if count >= max_items:
                                break
                    except:
                        pass

    def get_random_monster(self) -> Optional[MonsterData]:
        """获取随机怪物"""
        if self.monsters:
            key = random.choice(list(self.monsters.keys()))
            return self.monsters[key]
        return None

    def get_random_skill(self) -> Optional[SkillData]:
        """获取随机技能"""
        if self.skills:
            key = random.choice(list(self.skills.keys()))
            return self.skills[key]
        return None

    def close(self):
        if self.pvf:
            self.pvf.close()


# ============================================================================
# 游戏实体
# ============================================================================

@dataclass
class Entity:
    """游戏实体基类"""
    x: float = 0
    y: float = 0
    hp: int = 100
    max_hp: int = 100
    attack: int = 10
    defense: int = 0
    speed: float = 100.0
    alive: bool = True
    width: int = 40
    height: int = 60
    color: tuple = (255, 255, 255)
    name: str = "Entity"

    def update(self, dt: float):
        pass

    def draw(self, screen: pygame.Surface):
        if not self.alive:
            return
        # 绘制实体矩形
        rect = pygame.Rect(int(self.x - self.width/2),
                          int(self.y - self.height),
                          self.width, self.height)
        pygame.draw.rect(screen, self.color, rect)
        pygame.draw.rect(screen, (0, 0, 0), rect, 2)

        # 绘制血条
        bar_width = 50
        bar_height = 6
        bar_x = int(self.x - bar_width/2)
        bar_y = int(self.y - self.height - 15)
        pygame.draw.rect(screen, (100, 100, 100),
                        (bar_x, bar_y, bar_width, bar_height))
        hp_ratio = max(0, self.hp / self.max_hp)
        pygame.draw.rect(screen, (0, 255, 0) if hp_ratio > 0.3 else (255, 0, 0),
                        (bar_x, bar_y, int(bar_width * hp_ratio), bar_height))

    def take_damage(self, damage: int) -> int:
        """受到伤害"""
        actual = max(1, damage - self.defense)
        self.hp -= actual
        if self.hp <= 0:
            self.hp = 0
            self.alive = False
        return actual

    def get_rect(self) -> pygame.Rect:
        return pygame.Rect(int(self.x - self.width/2),
                          int(self.y - self.height),
                          self.width, self.height)


class Player(Entity):
    """玩家角色"""

    def __init__(self):
        super().__init__()
        self.x = 400
        self.y = 400
        self.hp = 1000
        self.max_hp = 1000
        self.attack = 100
        self.defense = 20
        self.speed = 200
        self.color = (100, 150, 255)
        self.name = "Player"
        self.facing_left = False
        self.attack_cooldown = 0
        self.weapon: Optional[EquipmentData] = None

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

        if self.attack_cooldown > 0:
            self.attack_cooldown -= dt

    def get_attack_power(self) -> int:
        """获取攻击力（包含武器加成）"""
        base = self.attack
        if self.weapon:
            base += self.weapon.phys_attack
        return base

    def draw(self, screen: pygame.Surface):
        super().draw(screen)
        # 绘制名称
        font = pygame.font.SysFont(None, 20)
        name_surf = font.render(self.name, True, (255, 255, 255))
        screen.blit(name_surf, (int(self.x - name_surf.get_width()/2),
                                int(self.y - self.height - 30)))


class Monster(Entity):
    """怪物实体 - 数据驱动"""

    def __init__(self, data: MonsterData, x: float, y: float):
        super().__init__()
        self.data = data
        self.x = x
        self.y = y

        # 从PVF数据设置属性
        self.hp = data.hp if data.hp > 0 else 100
        self.max_hp = self.hp
        self.attack = data.attack if data.attack > 0 else 10
        self.defense = data.defense
        self.speed = data.move_speed * 50 if data.move_speed > 0 else 80
        self.name = f"Monster_{data.monster_id}"
        self.color = (255, 100, 100)

        # AI状态
        self.target: Optional[Entity] = None
        self.ai_timer = 0
        self.ai_state = 'idle'  # idle, chase, attack

    def update(self, dt: float, player: Player):
        if not self.alive:
            return

        self.target = player
        self.ai_timer += dt

        # 简单AI：追踪玩家
        if self.target and self.target.alive:
            dist = ((self.x - self.target.x)**2 + (self.y - self.target.y)**2)**0.5

            if dist < 50:
                self.ai_state = 'attack'
            elif dist < 300:
                self.ai_state = 'chase'
            else:
                self.ai_state = 'idle'

            if self.ai_state == 'chase':
                dx = self.target.x - self.x
                dy = self.target.y - self.y
                length = max(1, (dx*dx + dy*dy)**0.5)
                self.x += (dx / length) * self.speed * dt
                self.y += (dy / length) * self.speed * dt

    def draw(self, screen: pygame.Surface):
        super().draw(screen)
        # 绘制AI状态
        font = pygame.font.SysFont(None, 16)
        state_surf = font.render(self.ai_state, True, (255, 255, 0))
        screen.blit(state_surf, (int(self.x - state_surf.get_width()/2),
                                 int(self.y + 5)))


# ============================================================================
# 战斗系统
# ============================================================================

class CombatSystem:
    """战斗系统"""

    def __init__(self, data_manager: GameDataManager):
        self.data = data_manager
        self.damage_texts: List[Dict] = []

    def player_attack(self, player: Player, monsters: List[Monster]) -> int:
        """玩家攻击"""
        if player.attack_cooldown > 0:
            return 0

        attack_power = player.get_attack_power()
        attack_range = 80
        total_damage = 0

        for monster in monsters:
            if not monster.alive:
                continue
            dist = ((player.x - monster.x)**2 + (player.y - monster.y)**2)**0.5
            if dist <= attack_range:
                # 计算伤害
                damage = self._calculate_damage(attack_power, monster.defense)
                actual = monster.take_damage(damage)
                total_damage += actual
                self._add_damage_text(monster.x, monster.y - monster.height,
                                     actual)

        player.attack_cooldown = 0.3  # 攻击间隔
        return total_damage

    def monster_attack(self, monster: Monster, player: Player) -> int:
        """怪物攻击"""
        if not monster.alive or not player.alive:
            return 0

        dist = ((monster.x - player.x)**2 + (monster.y - player.y)**2)**0.5
        if dist > 60:
            return 0

        damage = self._calculate_damage(monster.attack, player.defense)
        actual = player.take_damage(damage)
        self._add_damage_text(player.x, player.y - player.height, actual,
                             color=(255, 0, 0))
        return actual

    def _calculate_damage(self, attack: int, defense: int) -> int:
        """计算伤害"""
        base = max(1, attack - defense * 0.5)
        variance = random.uniform(0.9, 1.1)
        crit = 1.5 if random.random() < 0.1 else 1.0
        return int(base * variance * crit)

    def _add_damage_text(self, x: float, y: float, damage: int,
                        color: tuple = (255, 255, 0)):
        """添加伤害数字"""
        self.damage_texts.append({
            'x': x, 'y': y, 'damage': damage,
            'color': color, 'timer': 1.0, 'vy': -50
        })

    def update(self, dt: float):
        """更新伤害文字"""
        for text in self.damage_texts[:]:
            text['timer'] -= dt
            text['y'] += text['vy'] * dt
            if text['timer'] <= 0:
                self.damage_texts.remove(text)

    def draw(self, screen: pygame.Surface):
        """绘制伤害数字"""
        font = pygame.font.SysFont(None, 24)
        for text in self.damage_texts:
            alpha = int(255 * text['timer'])
            color = tuple(min(255, c + (255 - c) * (1 - text['timer']))
                         for c in text['color'])
            surf = font.render(str(text['damage']), True, color)
            screen.blit(surf, (int(text['x'] - surf.get_width()/2),
                              int(text['y'])))


# ============================================================================
# 游戏主类
# ============================================================================

class Game:
    """游戏主类"""

    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode(
            (CONFIG['screen_width'], CONFIG['screen_height']))
        pygame.display.set_caption("DNF Prototype V4 - Data Driven")
        self.clock = pygame.time.Clock()
        self.running = True
        self.font = pygame.font.SysFont(None, 24)

        # 游戏数据
        self.data_manager = GameDataManager()
        self.player: Optional[Player] = None
        self.monsters: List[Monster] = []
        self.combat: Optional[CombatSystem] = None

        # 统计
        self.kills = 0
        self.spawn_timer = 0

    def load(self) -> bool:
        """加载游戏资源"""
        if not self.data_manager.load(CONFIG['pvf_path'],
                                       CONFIG['stringtable_path']):
            return False

        # 创建玩家
        self.player = Player()

        # 装备随机武器
        if self.data_manager.equipment:
            weapon_key = list(self.data_manager.equipment.keys())[0]
            self.player.weapon = self.data_manager.equipment[weapon_key]
            print(f"[Game] Equipped: {self.player.weapon}")

        # 创建战斗系统
        self.combat = CombatSystem(self.data_manager)

        # 生成初始怪物
        self._spawn_monsters(3)

        return True

    def _spawn_monsters(self, count: int):
        """生成怪物"""
        for _ in range(count):
            monster_data = self.data_manager.get_random_monster()
            if monster_data:
                x = random.randint(100, 700)
                y = random.randint(250, 500)
                monster = Monster(monster_data, x, y)
                self.monsters.append(monster)

    def run(self):
        """主循环"""
        print("[Game] Starting main loop...")
        print("Controls: Arrow Keys = Move, Space = Attack, ESC = Quit")

        while self.running:
            dt = self.clock.tick(CONFIG['fps']) / 1000.0

            # 事件处理
            for event in pygame.event.get():
                if event.type == QUIT:
                    self.running = False
                elif event.type == KEYDOWN:
                    if event.key == K_ESCAPE:
                        self.running = False
                    elif event.key == K_SPACE:
                        self.combat.player_attack(self.player, self.monsters)

            # 更新
            keys = pygame.key.get_pressed()
            self._update(dt, keys)

            # 渲染
            self._render()

        self.cleanup()

    def _update(self, dt: float, keys):
        """更新游戏状态"""
        # 更新玩家
        self.player.update(dt, keys)

        # 更新怪物
        alive_count = 0
        for monster in self.monsters:
            monster.update(dt, self.player)
            if monster.alive:
                alive_count += 1
                # 怪物攻击
                if monster.ai_state == 'attack':
                    if random.random() < 0.02:  # 低概率攻击
                        self.combat.monster_attack(monster, self.player)
            else:
                self.kills += 1

        # 移除死亡怪物
        self.monsters = [m for m in self.monsters if m.alive]

        # 自动生成新怪物
        self.spawn_timer += dt
        if self.spawn_timer > 3.0 and len(self.monsters) < 5:
            self._spawn_monsters(1)
            self.spawn_timer = 0

        # 更新战斗系统
        self.combat.update(dt)

    def _render(self):
        """渲染"""
        # 背景
        self.screen.fill((50, 50, 80))

        # 地面
        pygame.draw.rect(self.screen, (80, 60, 40),
                        (0, 450, 800, 150))

        # 绘制怪物
        for monster in self.monsters:
            monster.draw(self.screen)

        # 绘制玩家
        self.player.draw(self.screen)

        # 绘制伤害数字
        self.combat.draw(self.screen)

        # UI
        self._draw_ui()

        pygame.display.flip()

    def _draw_ui(self):
        """绘制UI"""
        # 玩家状态
        hp_text = f"HP: {self.player.hp}/{self.player.max_hp}"
        hp_surf = self.font.render(hp_text, True, (255, 255, 255))
        self.screen.blit(hp_surf, (20, 20))

        atk_text = f"ATK: {self.player.get_attack_power()}"
        atk_surf = self.font.render(atk_text, True, (255, 255, 255))
        self.screen.blit(atk_surf, (20, 45))

        # 武器
        if self.player.weapon:
            weapon_text = f"Weapon: +{self.player.weapon.phys_attack}"
            weapon_surf = self.font.render(weapon_text, True, (255, 200, 100))
            self.screen.blit(weapon_surf, (20, 70))

        # 统计
        kills_text = f"Kills: {self.kills}"
        kills_surf = self.font.render(kills_text, True, (255, 255, 0))
        self.screen.blit(kills_surf, (700, 20))

        monsters_text = f"Monsters: {len(self.monsters)}"
        monsters_surf = self.font.render(monsters_text, True, (255, 100, 100))
        self.screen.blit(monsters_surf, (650, 45))

        # 数据统计
        data_text = f"PVF: {len(self.data_manager.monsters)} mobs loaded"
        data_surf = self.font.render(data_text, True, (150, 150, 150))
        self.screen.blit(data_surf, (20, 570))

    def cleanup(self):
        """清理资源"""
        self.data_manager.close()
        pygame.quit()


# ============================================================================
# 主函数
# ============================================================================

def main():
    print("=" * 60)
    print("DNF Client Prototype V4 - Data Driven")
    print("=" * 60)

    game = Game()
    if not game.load():
        print("Failed to load game resources")
        return 1

    game.run()
    print("Goodbye!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
