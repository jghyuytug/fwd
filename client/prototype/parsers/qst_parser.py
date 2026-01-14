#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
QST Quest Parser
================
解析PVF中的任务定义文件 (.qst)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构中等 (107-2,372字节，27-491条目)
- 定义主线/支线/日常任务

关键字段 (从样本分析):
  0x417D = 任务ID引用
  0x10FD = 任务类型
  0x10FC = 等级需求
  0x82DB = 前置任务
  0x4338 = 推荐等级
  0x1A65 = 接任务NPC
  0x160B = 交任务NPC
  0x82DA = 经验奖励
  0x82EB = 金币奖励
  0x82EC = 物品奖励
  0x82E0 = 任务目标
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

class QST:
    """任务文件键ID"""
    QUEST_ID = 0x417D           # 任务ID引用
    QUEST_TYPE = 0x10FD         # 任务类型
    LEVEL_REQ = 0x10FC          # 等级需求
    PREREQ_QUEST = 0x82DB       # 前置任务ID
    RECOMMENDED_LV = 0x4338     # 推荐等级
    START_NPC = 0x1A65          # 接任务NPC引用
    END_NPC = 0x160B            # 交任务NPC引用
    EXP_REWARD = 0x82DA         # 经验奖励
    GOLD_REWARD = 0x82EB        # 金币奖励
    ITEM_REWARD = 0x82EC        # 物品奖励引用
    OBJECTIVE = 0x82E0          # 任务目标

    # 扩展字段
    NAME_REF = 0x2A12           # 任务名称引用
    DESC = 0x82DF               # 任务描述
    OBJECTIVE_COUNT = 0x82D9    # 目标数量
    REPEATABLE = 0x53FD         # 可重复 (-1=不可重复)
    TIME_LIMIT = 0x5B8B         # 时间限制
    REGION = 0x5749             # 区域引用
    CHAIN_NEXT = 0x82DD         # 后续任务


# 任务类型定义
class QuestType:
    MAIN = 1                    # 主线
    SIDE = 2                    # 支线
    DAILY = 3                   # 日常
    WEEKLY = 4                  # 周常
    EVENT = 5                   # 活动
    HIDDEN = 6                  # 隐藏
    EPIC = 7                    # 史诗
    TUTORIAL = 8                # 教程

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            1: 'main',
            2: 'side',
            3: 'daily',
            4: 'weekly',
            5: 'event',
            6: 'hidden',
            7: 'epic',
            8: 'tutorial',
        }
        return names.get(code, f'type_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class QuestData:
    """任务数据"""
    quest_id: int = 0           # 任务ID
    quest_type: int = 0         # 任务类型
    level_req: int = 0          # 等级需求
    prereq_quest: int = -1      # 前置任务 (-1=无)
    recommended_lv: int = 0     # 推荐等级
    start_npc: int = 0          # 接任务NPC
    end_npc: int = 0            # 交任务NPC
    exp_reward: int = 0         # 经验奖励
    gold_reward: int = 0        # 金币奖励
    item_reward: int = 0        # 物品奖励引用
    objective: int = 0          # 任务目标

    # 扩展属性
    name_ref: int = 0           # 名称引用
    desc: int = 0               # 描述
    objective_count: int = 1    # 目标数量
    repeatable: bool = False    # 可重复
    time_limit: int = 0         # 时间限制 (秒)
    region: int = 0             # 区域
    chain_next: int = 0         # 后续任务

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return QuestType.get_name(self.quest_type)

    def __repr__(self):
        return (f"Quest(type={self.type_name}, lv={self.level_req}, "
                f"exp={self.exp_reward}, gold={self.gold_reward})")


# ============================================================================
# 解析器
# ============================================================================

class QuestParser:
    """QST任务文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[QuestData]:
        """
        解析QST文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            QuestData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            qst = QuestData()
            qst.raw_entries = len(container.entries)
            qst.path = path

            # 核心属性
            qst.quest_id = container.get_ref(QST.QUEST_ID)
            qst.quest_type = container.get_int(QST.QUEST_TYPE)
            qst.level_req = container.get_int(QST.LEVEL_REQ)
            qst.prereq_quest = container.get_int(QST.PREREQ_QUEST, -1)
            qst.recommended_lv = container.get_int(QST.RECOMMENDED_LV)
            qst.start_npc = container.get_ref(QST.START_NPC)
            qst.end_npc = container.get_ref(QST.END_NPC)
            qst.exp_reward = container.get_int(QST.EXP_REWARD)
            qst.gold_reward = container.get_int(QST.GOLD_REWARD)
            qst.item_reward = container.get_int(QST.ITEM_REWARD)
            qst.objective = container.get_int(QST.OBJECTIVE)

            # 扩展属性
            qst.name_ref = container.get_ref(QST.NAME_REF)
            qst.desc = container.get_int(QST.DESC)
            qst.objective_count = container.get_int(QST.OBJECTIVE_COUNT, 1)
            repeatable_val = container.get_int(QST.REPEATABLE, -1)
            qst.repeatable = repeatable_val != -1
            qst.time_limit = container.get_int(QST.TIME_LIMIT)
            qst.region = container.get_ref(QST.REGION)
            qst.chain_next = container.get_int(QST.CHAIN_NEXT)

            return qst

        except Exception as e:
            return None

    @staticmethod
    def is_valid_qst(data: bytes) -> bool:
        """检查数据是否为有效的QST格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析QST文件结构（调试用）"""
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
    print("QST Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析QST文件
    print()
    print("Sample QST files:")
    print("-" * 60)

    count = 0
    type_stats = {}
    level_range = [999, 0]

    for entry in pvf.indexes:
        if entry.name.endswith('.qst'):
            data = pvf.extract_file(entry)
            if data:
                qst = QuestParser.parse(data, entry.name)
                if qst:
                    # 统计类型
                    type_stats[qst.quest_type] = type_stats.get(qst.quest_type, 0) + 1

                    if qst.level_req > 0:
                        level_range[0] = min(level_range[0], qst.level_req)
                        level_range[1] = max(level_range[1], qst.level_req)

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {qst}")
                        print(f"    prereq={qst.prereq_quest}, objective_count={qst.objective_count}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} QST files")
    print(f"Level range: {level_range[0]} - {level_range[1]}")
    print()
    print("Type distribution:")
    for t, cnt in sorted(type_stats.items(), key=lambda x: -x[1])[:10]:
        print(f"  {QuestType.get_name(t)}: {cnt}")

    pvf.close()
    print()
    print("Done!")
