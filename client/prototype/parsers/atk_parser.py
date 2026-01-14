#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ATK Attack Parser
=================
解析PVF中的攻击定义文件 (.atk)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构非常稳定 (97-192字节，24-39条目)
- 定义攻击的伤害、范围、冷却等参数

关键字段:
  0x1A42 = 基础伤害
  0x03F4 = 攻击名称引用
  0x03F8 = 动画ID
  0x03FA = 特效粒子
  0x03FC = 攻击范围
  0x03FD = 冷却/攻速
  0x0407 = 击退/硬直
"""

import struct
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
from pvf_data_parser import parse_pvf_data, PVFDataContainer, PVFValueType


# ============================================================================
# 键ID常量
# ============================================================================

class ATK:
    """攻击文件键ID"""
    DAMAGE = 0x1A42             # 基础伤害
    NAME_REF = 0x03F4           # 攻击名称引用
    ANIMATION_ID = 0x03F8       # 动画ID
    EFFECT_ID = 0x03FA          # 特效粒子ID
    RANGE = 0x03FC              # 攻击范围
    COOLDOWN = 0x03FD           # 冷却时间/攻击速度
    KNOCKBACK = 0x0407          # 击退/硬直时间

    # 扩展字段 (从样本分析)
    ATTACK_TYPE = 0x03F5        # 攻击类型
    ELEMENT = 0x03F6            # 元素属性
    HIT_COUNT = 0x03F7          # 命中次数
    ANGLE = 0x03F9              # 攻击角度
    HEIGHT = 0x03FB             # 攻击高度
    STUN_DURATION = 0x0408      # 眩晕持续时间
    DAMAGE_TYPE = 0x0409        # 伤害类型 (物理/魔法)


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class AttackData:
    """攻击数据"""
    damage: int = 0             # 基础伤害
    name_ref: int = 0           # 名称引用
    animation_id: int = 0       # 动画ID
    effect_id: int = 0          # 特效ID
    range: int = 0              # 攻击范围
    cooldown: int = 0           # 冷却时间 (毫秒)
    knockback: int = 0          # 击退/硬直 (毫秒)

    # 扩展属性
    attack_type: int = 0        # 攻击类型
    element: int = 0            # 元素属性
    hit_count: int = 1          # 命中次数
    angle: int = 0              # 攻击角度
    height: int = 0             # 攻击高度
    stun_duration: int = 0      # 眩晕时间
    damage_type: int = 0        # 0=物理, 1=魔法

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    def __repr__(self):
        dtype = "phys" if self.damage_type == 0 else "mag"
        return (f"Attack(dmg={self.damage}, range={self.range}, "
                f"cd={self.cooldown}ms, type={dtype})")

    @property
    def is_physical(self) -> bool:
        return self.damage_type == 0

    @property
    def is_magical(self) -> bool:
        return self.damage_type == 1


# ============================================================================
# 解析器
# ============================================================================

class AttackParser:
    """ATK攻击文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[AttackData]:
        """
        解析ATK文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            AttackData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            atk = AttackData()
            atk.raw_entries = len(container.entries)
            atk.path = path

            # 核心属性
            atk.damage = container.get_int(ATK.DAMAGE)
            atk.name_ref = container.get_ref(ATK.NAME_REF)
            atk.animation_id = container.get_int(ATK.ANIMATION_ID)
            atk.effect_id = container.get_ref(ATK.EFFECT_ID)
            atk.range = container.get_int(ATK.RANGE)
            atk.cooldown = container.get_int(ATK.COOLDOWN)
            atk.knockback = container.get_int(ATK.KNOCKBACK)

            # 扩展属性
            atk.attack_type = container.get_int(ATK.ATTACK_TYPE)
            atk.element = container.get_int(ATK.ELEMENT)
            atk.hit_count = container.get_int(ATK.HIT_COUNT, 1)
            atk.angle = container.get_int(ATK.ANGLE)
            atk.height = container.get_int(ATK.HEIGHT)
            atk.stun_duration = container.get_int(ATK.STUN_DURATION)
            atk.damage_type = container.get_int(ATK.DAMAGE_TYPE)

            return atk

        except Exception as e:
            return None

    @staticmethod
    def is_valid_atk(data: bytes) -> bool:
        """检查数据是否为有效的ATK格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析ATK文件结构（调试用）"""
        result = {
            'size': len(data),
            'valid': False,
            'has_magic': False,
            'entry_count': 0,
            'keys': [],
        }

        if len(data) < 2:
            return result

        if data[0] == 0xB0 and data[1] == 0xD0:
            result['has_magic'] = True
            result['valid'] = True

            container = parse_pvf_data(data)
            result['entry_count'] = len(container.entries)

            # 收集所有键
            keys = set()
            for entry in container.entries:
                if entry.key > 0:
                    keys.add(entry.key)
            result['keys'] = sorted(keys)

        return result


# ============================================================================
# 测试
# ============================================================================

if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("ATK Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析ATK文件
    print()
    print("Sample ATK files:")
    print("-" * 60)

    count = 0
    damage_stats = []

    for entry in pvf.indexes:
        if entry.name.endswith('.atk'):
            data = pvf.extract_file(entry)
            if data:
                atk = AttackParser.parse(data, entry.name)
                if atk:
                    damage_stats.append(atk.damage)

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {atk}")
                        print(f"    knockback={atk.knockback}ms, hits={atk.hit_count}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} ATK files")

    if damage_stats:
        print(f"Damage range: {min(damage_stats)} - {max(damage_stats)}")
        print(f"Average damage: {sum(damage_stats) / len(damage_stats):.1f}")

    pvf.close()
    print()
    print("Done!")
