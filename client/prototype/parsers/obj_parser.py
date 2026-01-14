#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
OBJ Game Object Parser
======================
解析PVF中的游戏对象定义文件 (.obj)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构中等 (27-427字节，3-94条目)
- 定义场景中的可交互对象

关键字段 (从样本分析):
  0x136E = 对象类型引用
  0x153A = 资源引用
  0x2A14 = 可交互标志
  0x42F6 = 动画引用
  0x1A6D = 层级/数量
  0x2A22 = 效果引用
  0x10FD = 类型
  0x2A12 = 名称引用
  0x0922 = 持续时间/数值
  0x1B4C = 碰撞引用
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

class OBJ:
    """游戏对象文件键ID"""
    OBJ_TYPE = 0x136E           # 对象类型引用
    RESOURCE = 0x153A           # 资源引用
    INTERACTIVE = 0x2A14        # 可交互标志
    ANIMATION = 0x42F6          # 动画引用
    LAYER = 0x1A6D              # 层级/数量
    EFFECT = 0x2A22             # 效果引用
    TYPE_ID = 0x10FD            # 类型ID
    NAME_REF = 0x2A12           # 名称引用
    DURATION = 0x0922           # 持续时间
    COLLISION = 0x1B4C          # 碰撞引用

    # 扩展字段
    FLAG_1 = 0xA8AB             # 标志1
    HP = 0xA89B                 # 生命值 (可破坏对象)
    RESPAWN = 0xA898            # 重生时间
    LEVEL = 0x10FC              # 等级
    DROP_REF = 0x1611           # 掉落引用
    SCALE = 0xA893              # 缩放
    TRIGGER = 0x0318            # 触发器引用
    SOUND = 0xA8A5              # 音效引用
    SPAWN = 0xA9EE              # 生成引用
    SPAWN_RATE = 0xA9F1         # 生成频率


# 对象类型定义
class ObjectType:
    STATIC = 0                  # 静态装饰
    DESTRUCTIBLE = 1            # 可破坏
    INTERACTIVE = 2             # 可交互
    TRIGGER = 3                 # 触发器
    SPAWNER = 4                 # 生成点
    PORTAL = 5                  # 传送门
    CHEST = 6                   # 宝箱
    NPC_SPAWN = 7               # NPC生成点

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            0: 'static',
            1: 'destructible',
            2: 'interactive',
            3: 'trigger',
            4: 'spawner',
            5: 'portal',
            6: 'chest',
            7: 'npc_spawn',
        }
        return names.get(code, f'obj_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class GameObjectData:
    """游戏对象数据"""
    obj_type: int = 0           # 对象类型
    resource: int = 0           # 资源引用
    interactive: bool = False   # 可交互
    animation: int = 0          # 动画引用
    layer: int = 0              # 层级
    effect: int = 0             # 效果引用
    type_id: int = 0            # 类型ID
    name_ref: int = 0           # 名称引用
    duration: int = 0           # 持续时间
    collision: int = 0          # 碰撞引用

    # 扩展属性
    hp: int = 0                 # 生命值
    respawn: int = 0            # 重生时间 (毫秒)
    level: int = 0              # 等级
    drop_ref: int = 0           # 掉落引用
    scale: int = 100            # 缩放百分比
    trigger: int = 0            # 触发器
    sound: int = 0              # 音效
    spawn_ref: int = 0          # 生成引用
    spawn_rate: int = 0         # 生成频率

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return ObjectType.get_name(self.obj_type)

    def __repr__(self):
        return (f"Object(type={self.type_name}, interactive={self.interactive}, "
                f"layer={self.layer}, hp={self.hp})")


# ============================================================================
# 解析器
# ============================================================================

class GameObjectParser:
    """OBJ游戏对象文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[GameObjectData]:
        """
        解析OBJ文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            GameObjectData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            obj = GameObjectData()
            obj.raw_entries = len(container.entries)
            obj.path = path

            # 核心属性
            obj.obj_type = container.get_ref(OBJ.OBJ_TYPE)
            obj.resource = container.get_ref(OBJ.RESOURCE)
            obj.interactive = container.get_int(OBJ.INTERACTIVE) != 0
            obj.animation = container.get_ref(OBJ.ANIMATION)
            obj.layer = container.get_int(OBJ.LAYER)
            obj.effect = container.get_ref(OBJ.EFFECT)
            obj.type_id = container.get_int(OBJ.TYPE_ID)
            obj.name_ref = container.get_ref(OBJ.NAME_REF)
            obj.duration = container.get_int(OBJ.DURATION)
            obj.collision = container.get_ref(OBJ.COLLISION)

            # 扩展属性
            obj.hp = container.get_int(OBJ.HP)
            obj.respawn = container.get_int(OBJ.RESPAWN)
            obj.level = container.get_int(OBJ.LEVEL)
            obj.drop_ref = container.get_ref(OBJ.DROP_REF)
            obj.scale = container.get_int(OBJ.SCALE, 100)
            obj.trigger = container.get_ref(OBJ.TRIGGER)
            obj.sound = container.get_int(OBJ.SOUND)
            obj.spawn_ref = container.get_ref(OBJ.SPAWN)
            obj.spawn_rate = container.get_int(OBJ.SPAWN_RATE)

            return obj

        except Exception as e:
            return None

    @staticmethod
    def is_valid_obj(data: bytes) -> bool:
        """检查数据是否为有效的OBJ格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析OBJ文件结构（调试用）"""
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
    print("OBJ Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    print()
    print("Sample OBJ files:")
    print("-" * 60)

    count = 0
    type_stats = {}

    for entry in pvf.indexes:
        if entry.name.endswith('.obj'):
            data = pvf.extract_file(entry)
            if data:
                obj = GameObjectParser.parse(data, entry.name)
                if obj:
                    type_stats[obj.obj_type] = type_stats.get(obj.obj_type, 0) + 1

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {obj}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} OBJ files")
    print()
    print("Type distribution:")
    for t, cnt in sorted(type_stats.items(), key=lambda x: -x[1])[:10]:
        print(f"  type_{t}: {cnt}")

    pvf.close()
    print("Done!")
