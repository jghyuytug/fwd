#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DGN Dungeon Parser
==================
解析PVF中的副本定义文件 (.dgn)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构较大 (292-23,827字节，60-4,941条目)
- 定义副本配置

关键字段:
  0x3B93 = 地图引用列表
  0x3B90 = 入口/出口
  0x3B95 = 难度设置
  0x3B97 = 奖励引用
  0x3B8E = 怪物组引用
  0x3B83 = Boss引用
  0x3B8A = 等级要求
  0x3B80 = 疲劳消耗
  0x3B87 = 玩家数量
  0x2A12 = 名称
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

class DGN:
    """副本文件键ID"""
    MAPS = 0x3B93               # 地图引用列表
    ENTRY = 0x3B90              # 入口
    DIFFICULTY = 0x3B95         # 难度设置
    REWARD = 0x3B97             # 奖励引用
    MONSTERS = 0x3B8E           # 怪物组引用
    BOSS = 0x3B83               # Boss引用
    LEVEL_REQ = 0x3B8A          # 等级要求
    FATIGUE = 0x3B80            # 疲劳消耗
    PLAYER_COUNT = 0x3B87       # 玩家数量
    NAME = 0x2A12               # 名称
    BACKGROUND = 0x3B85         # 背景引用
    DIFFICULTY_LV = 0x3B89      # 难度等级
    NAME_REF = 0x3B81           # 名称引用
    ID = 0x3B8D                 # 副本ID
    FLAG = 0x3B8C               # 标志
    SCALE = 0x3B8B              # 缩放
    ROOM_COUNT = 0x3B8F         # 房间数
    TYPE = 0x3B92               # 类型
    TIMER = 0x3B94              # 时间限制


# 副本类型
class DungeonType:
    NORMAL = 0                  # 普通
    SPECIAL = 1                 # 特殊
    EVENT = 2                   # 活动
    RAID = 3                    # 团本
    TOWER = 4                   # 塔

    @staticmethod
    def get_name(code: int) -> str:
        names = {0: 'normal', 1: 'special', 2: 'event', 3: 'raid', 4: 'tower'}
        return names.get(code, f'dgn_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class DungeonData:
    """副本数据"""
    dungeon_id: int = 0         # 副本ID
    name_ref: int = 0           # 名称引用
    level_req: int = 0          # 等级要求
    fatigue: int = 0            # 疲劳消耗
    player_count: int = 1       # 玩家数量
    difficulty_lv: int = 1      # 难度等级
    room_count: int = 1         # 房间数
    dungeon_type: int = 0       # 副本类型
    boss: int = 0               # Boss引用
    reward: int = 0             # 奖励引用
    entry: int = 0              # 入口引用
    background: int = 0         # 背景引用
    timer: int = 0              # 时间限制
    flag: int = 0               # 标志
    scale: float = 1.0          # 缩放

    raw_entries: int = 0
    path: str = ""

    @property
    def type_name(self) -> str:
        return DungeonType.get_name(self.dungeon_type)

    def __repr__(self):
        return (f"Dungeon(id={self.dungeon_id}, lv={self.level_req}, "
                f"rooms={self.room_count}, fatigue={self.fatigue})")


# ============================================================================
# 解析器
# ============================================================================

class DungeonParser:
    """DGN副本文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[DungeonData]:
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            dgn = DungeonData()
            dgn.raw_entries = len(container.entries)
            dgn.path = path

            dgn.dungeon_id = container.get_int(DGN.ID)
            dgn.name_ref = container.get_ref(DGN.NAME_REF)
            dgn.level_req = container.get_int(DGN.LEVEL_REQ)
            dgn.fatigue = container.get_int(DGN.FATIGUE)
            dgn.player_count = container.get_int(DGN.PLAYER_COUNT, 1)
            dgn.difficulty_lv = container.get_int(DGN.DIFFICULTY_LV, 1)
            dgn.room_count = container.get_int(DGN.ROOM_COUNT, 1)
            dgn.dungeon_type = container.get_int(DGN.TYPE)
            dgn.boss = container.get_ref(DGN.BOSS)
            dgn.reward = container.get_int(DGN.REWARD)
            dgn.entry = container.get_ref(DGN.ENTRY)
            dgn.background = container.get_ref(DGN.BACKGROUND)
            dgn.timer = container.get_int(DGN.TIMER)
            dgn.flag = container.get_int(DGN.FLAG)
            dgn.scale = container.get_float(DGN.SCALE, 1.0)

            return dgn

        except Exception as e:
            return None

    @staticmethod
    def is_valid_dgn(data: bytes) -> bool:
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0


if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("DGN Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    count = 0
    level_range = [999, 0]
    for entry in pvf.indexes:
        if entry.name.endswith('.dgn'):
            data = pvf.extract_file(entry)
            if data:
                dgn = DungeonParser.parse(data, entry.name)
                if dgn:
                    if dgn.level_req > 0:
                        level_range[0] = min(level_range[0], dgn.level_req)
                        level_range[1] = max(level_range[1], dgn.level_req)
                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}: {dgn}")
                    count += 1
                    if count >= 500:
                        break

    print(f"\nParsed: {count} DGN files")
    if level_range[1] > 0:
        print(f"Level range: {level_range[0]} - {level_range[1]}")
    pvf.close()
