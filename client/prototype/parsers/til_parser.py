#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TIL Tile Parser
===============
解析PVF中的地图块定义文件 (.til)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构中等 (242-2,132字节，50-428条目)
- 定义地图的地形块属性

关键字段 (从样本分析):
  0x2298 = 地形类型引用
  0x15E7 = 贴图引用
  0x153A = 层级/类型
  0x1539 = 尺寸
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

class TIL:
    """地图块文件键ID"""
    TERRAIN_TYPE = 0x2298       # 地形类型引用
    TEXTURE_REF = 0x15E7        # 贴图引用
    LAYER = 0x153A              # 层级/类型
    SIZE = 0x1539               # 尺寸


# 地形类型定义
class TerrainType:
    NORMAL = 0                  # 普通地面
    WATER = 1                   # 水
    LAVA = 2                    # 熔岩
    ICE = 3                     # 冰
    MUD = 4                     # 泥地
    GRASS = 5                   # 草地
    SAND = 6                    # 沙地
    ROCK = 7                    # 岩石
    BRIDGE = 8                  # 桥
    WALL = 9                    # 墙壁

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            0: 'normal',
            1: 'water',
            2: 'lava',
            3: 'ice',
            4: 'mud',
            5: 'grass',
            6: 'sand',
            7: 'rock',
            8: 'bridge',
            9: 'wall',
        }
        return names.get(code, f'terrain_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class TileEntry:
    """单个地图块条目"""
    terrain_ref: int = 0        # 地形引用
    texture_ref: int = 0        # 贴图引用
    layer: int = 0              # 层级

    def __repr__(self):
        return f"Tile(terrain={self.terrain_ref}, texture={self.texture_ref}, layer={self.layer})"


@dataclass
class TileData:
    """地图块数据"""
    terrain_types: List[int] = field(default_factory=list)   # 地形类型列表
    texture_refs: List[int] = field(default_factory=list)    # 贴图引用列表
    layers: List[int] = field(default_factory=list)          # 层级列表
    size: int = 0               # 尺寸

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def tile_count(self) -> int:
        return max(len(self.terrain_types), len(self.texture_refs))

    def __repr__(self):
        return (f"Tile(tiles={self.tile_count}, size={self.size}, "
                f"layers={len(set(self.layers))})")


# ============================================================================
# 解析器
# ============================================================================

class TileParser:
    """TIL地图块文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[TileData]:
        """
        解析TIL文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            TileData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            til = TileData()
            til.raw_entries = len(container.entries)
            til.path = path

            # 收集地形类型 (使用 get_ints 因为引用值存储为整数)
            til.terrain_types = container.get_ints(TIL.TERRAIN_TYPE)

            # 收集贴图引用
            til.texture_refs = container.get_ints(TIL.TEXTURE_REF)

            # 收集层级
            til.layers = container.get_ints(TIL.LAYER)

            # 尺寸
            til.size = container.get_int(TIL.SIZE)

            return til

        except Exception as e:
            return None

    @staticmethod
    def is_valid_til(data: bytes) -> bool:
        """检查数据是否为有效的TIL格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析TIL文件结构（调试用）"""
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
    print("TIL Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    print()
    print("Sample TIL files:")
    print("-" * 60)

    count = 0
    size_stats = []

    for entry in pvf.indexes:
        if entry.name.endswith('.til'):
            data = pvf.extract_file(entry)
            if data:
                til = TileParser.parse(data, entry.name)
                if til:
                    size_stats.append(til.tile_count)

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {til}")
                        if til.terrain_types:
                            print(f"    terrain_types: {til.terrain_types[:5]}...")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} TIL files")
    if size_stats:
        print(f"Tile count range: {min(size_stats)} - {max(size_stats)}")
        print(f"Average tiles: {sum(size_stats) / len(size_stats):.1f}")

    pvf.close()
    print("Done!")
