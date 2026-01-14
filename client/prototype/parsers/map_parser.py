#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MAP Map Parser
==============
解析PVF中的地图定义文件 (.map)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构中等 (232-3,672字节，43-969条目)
- 定义游戏地图布局

关键字段:
  0x1507 = 地图类型
  0x1508 = 尺寸/偏移
  0x42F1 = 资源引用
  0x5A67 = 区域引用
  0x1512 = 地形引用
  0x3B90 = NPC/对象引用
  0x1510 = 宽度
  0x150F = 高度
  0x151A = 入口/出口
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

class MAP:
    """地图文件键ID"""
    MAP_TYPE = 0x1507           # 地图类型
    OFFSET = 0x1508             # 偏移
    RESOURCE = 0x42F1           # 资源引用
    REGION = 0x5A67             # 区域引用
    TERRAIN = 0x1512            # 地形引用
    OBJECTS = 0x3B90            # 对象引用
    WIDTH = 0x1510              # 宽度
    HEIGHT = 0x150F             # 高度
    ENTRY = 0x151A              # 入口
    EXIT = 0x1521               # 出口
    TYPE_ID = 0x44FC            # 类型ID
    ATTR = 0x1518               # 属性
    PARAM = 0x0EA4              # 参数
    PORTAL = 0x154B             # 传送门
    NPC_REF = 0x160B            # NPC引用
    CONNECT = 0x151D            # 连接区域


# 地图类型
class MapType:
    TOWN = 1                    # 城镇
    FIELD = 2                   # 野外
    DUNGEON = 3                 # 副本
    PVP = 4                     # PVP
    EVENT = 5                   # 活动

    @staticmethod
    def get_name(code: int) -> str:
        names = {1: 'town', 2: 'field', 3: 'dungeon', 4: 'pvp', 5: 'event'}
        return names.get(code, f'map_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class MapData:
    """地图数据"""
    map_type: int = 0           # 地图类型
    width: int = 0              # 宽度
    height: int = 0             # 高度
    offset: int = 0             # 偏移
    resource: int = 0           # 资源引用
    region: int = 0             # 区域引用
    terrain: int = 0            # 地形引用
    entry: int = 0              # 入口
    exit: int = 0               # 出口
    type_id: int = 0            # 类型ID
    portal: int = 0             # 传送门
    npc_ref: int = 0            # NPC引用
    connect: int = 0            # 连接区域

    raw_entries: int = 0
    path: str = ""

    @property
    def type_name(self) -> str:
        return MapType.get_name(self.map_type)

    def __repr__(self):
        return (f"Map(type={self.type_name}, size={self.width}x{self.height}, "
                f"entries={self.raw_entries})")


# ============================================================================
# 解析器
# ============================================================================

class MapParser:
    """MAP地图文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[MapData]:
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            m = MapData()
            m.raw_entries = len(container.entries)
            m.path = path

            m.map_type = container.get_int(MAP.MAP_TYPE)
            m.width = container.get_int(MAP.WIDTH)
            m.height = container.get_int(MAP.HEIGHT)
            m.offset = container.get_int(MAP.OFFSET)
            m.resource = container.get_int(MAP.RESOURCE)
            m.region = container.get_int(MAP.REGION)
            m.terrain = container.get_ref(MAP.TERRAIN)
            m.entry = container.get_int(MAP.ENTRY)
            m.exit = container.get_int(MAP.EXIT)
            m.type_id = container.get_int(MAP.TYPE_ID)
            m.portal = container.get_ref(MAP.PORTAL)
            m.npc_ref = container.get_ref(MAP.NPC_REF)
            m.connect = container.get_int(MAP.CONNECT)

            return m

        except Exception as e:
            return None

    @staticmethod
    def is_valid_map(data: bytes) -> bool:
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0


if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("MAP Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    count = 0
    type_stats = {}
    for entry in pvf.indexes:
        if entry.name.endswith('.map'):
            data = pvf.extract_file(entry)
            if data:
                m = MapParser.parse(data, entry.name)
                if m:
                    type_stats[m.map_type] = type_stats.get(m.map_type, 0) + 1
                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}: {m}")
                    count += 1
                    if count >= 1000:
                        break

    print(f"\nParsed: {count} MAP files")
    print("Type distribution:")
    for t, c in sorted(type_stats.items(), key=lambda x: -x[1]):
        print(f"  {MapType.get_name(t)}: {c}")
    pvf.close()
