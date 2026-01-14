#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI Script Parser
================
解析PVF中的AI脚本定义文件 (.ai)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构较小 (22-1,507字节，4-404条目)
- 定义怪物AI行为树

关键字段 (从样本分析):
  0x01E0 = AI类型
  0x01E1 = 行为1
  0x01E2 = 行为引用
  0x01E4 = 主行为
  0x01E6 = 行为引用2
  0x01E8 = 行为3
  0x01EA = 行为4
  0x01F2 = 目标选择
  0x01F7 = 技能引用1
  0x01FA = 参数1
  0x01FB = 参数2
  0x01FC = 技能引用2
  0x02BF = 目标类型
  0x02C1 = AI参数
  0x02C2 = 技能引用3
  0x0444 = 攻击间隔
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

class AI:
    """AI脚本文件键ID"""
    AI_TYPE = 0x01E0            # AI类型
    BEHAVIOR_1 = 0x01E1         # 行为1
    BEHAVIOR_REF = 0x01E2       # 行为引用
    MAIN_BEHAVIOR = 0x01E4      # 主行为
    BEHAVIOR_REF_2 = 0x01E6     # 行为引用2
    BEHAVIOR_3 = 0x01E8         # 行为3
    BEHAVIOR_4 = 0x01EA         # 行为4
    TARGET_SELECT = 0x01F2      # 目标选择
    SKILL_REF_1 = 0x01F7        # 技能引用1
    PARAM_1 = 0x01FA            # 参数1
    PARAM_2 = 0x01FB            # 参数2
    SKILL_REF_2 = 0x01FC        # 技能引用2
    TARGET_TYPE = 0x02BF        # 目标类型
    AI_PARAM = 0x02C1           # AI参数
    SKILL_REF_3 = 0x02C2        # 技能引用3
    ATTACK_INTERVAL = 0x0444    # 攻击间隔

    # 扩展字段
    AGGRO_RANGE = 0x0329        # 仇恨范围
    PATROL_REF = 0x032B         # 巡逻路径引用
    FLEE_HP = 0x0309            # 逃跑血量阈值
    SPECIAL_1 = 0x046B          # 特殊行为1
    SPECIAL_2 = 0x046E          # 特殊行为2


# AI类型定义
class AIType:
    PASSIVE = 0                 # 被动
    AGGRESSIVE = 1              # 主动攻击
    PATROL = 2                  # 巡逻
    GUARD = 3                   # 守卫
    BOSS = 4                    # Boss
    SUPPORT = 5                 # 辅助
    FLEE = 6                    # 逃跑型
    SUMMON = 7                  # 召唤
    ELITE = 8                   # 精英

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            0: 'passive',
            1: 'aggressive',
            2: 'patrol',
            3: 'guard',
            4: 'boss',
            5: 'support',
            6: 'flee',
            7: 'summon',
            8: 'elite',
        }
        return names.get(code, f'ai_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class AIBehavior:
    """AI行为节点"""
    behavior_id: int = 0        # 行为ID
    priority: int = 0           # 优先级
    condition: int = 0          # 触发条件

    def __repr__(self):
        return f"Behavior(id={self.behavior_id}, priority={self.priority})"


@dataclass
class AIData:
    """AI脚本数据"""
    ai_type: int = 0            # AI类型
    main_behavior: int = 0      # 主行为
    behaviors: List[int] = field(default_factory=list)  # 行为列表
    target_select: int = 0      # 目标选择方式
    target_type: int = 0        # 目标类型
    attack_interval: int = 0    # 攻击间隔 (毫秒)

    # 技能引用
    skill_refs: List[int] = field(default_factory=list)

    # 参数
    param_1: int = 0
    param_2: int = 0
    ai_param: float = 0.0

    # 扩展属性
    aggro_range: int = 0        # 仇恨范围
    patrol_ref: int = 0         # 巡逻路径
    flee_hp: float = 0.0        # 逃跑血量百分比

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return AIType.get_name(self.ai_type)

    @property
    def behavior_count(self) -> int:
        return len(self.behaviors)

    @property
    def skill_count(self) -> int:
        return len(self.skill_refs)

    def __repr__(self):
        return (f"AI(type={self.type_name}, behaviors={self.behavior_count}, "
                f"skills={self.skill_count}, interval={self.attack_interval}ms)")


# ============================================================================
# 解析器
# ============================================================================

class AIParser:
    """AI脚本文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[AIData]:
        """
        解析AI文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            AIData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            ai = AIData()
            ai.raw_entries = len(container.entries)
            ai.path = path

            # 核心属性
            ai.ai_type = container.get_int(AI.AI_TYPE)
            ai.main_behavior = container.get_int(AI.MAIN_BEHAVIOR)
            ai.target_select = container.get_int(AI.TARGET_SELECT)
            ai.target_type = container.get_ref(AI.TARGET_TYPE)
            ai.attack_interval = container.get_int(AI.ATTACK_INTERVAL)

            # 收集行为
            ai.behaviors = []
            for key in [AI.BEHAVIOR_1, AI.BEHAVIOR_REF, AI.MAIN_BEHAVIOR,
                       AI.BEHAVIOR_REF_2, AI.BEHAVIOR_3, AI.BEHAVIOR_4]:
                val = container.get_int(key)
                if val > 0:
                    ai.behaviors.append(val)

            # 收集技能引用
            ai.skill_refs = []
            for key in [AI.SKILL_REF_1, AI.SKILL_REF_2, AI.SKILL_REF_3]:
                ref = container.get_ref(key)
                if ref > 0:
                    ai.skill_refs.append(ref)

            # 参数
            ai.param_1 = container.get_int(AI.PARAM_1)
            ai.param_2 = container.get_int(AI.PARAM_2)
            ai.ai_param = container.get_float(AI.AI_PARAM, 0.0)

            # 扩展属性
            ai.aggro_range = container.get_int(AI.AGGRO_RANGE)
            ai.patrol_ref = container.get_ref(AI.PATROL_REF)
            ai.flee_hp = container.get_float(AI.FLEE_HP, 0.0)

            return ai

        except Exception as e:
            return None

    @staticmethod
    def is_valid_ai(data: bytes) -> bool:
        """检查数据是否为有效的AI格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析AI文件结构（调试用）"""
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
    print("AI Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析AI文件
    print()
    print("Sample AI files:")
    print("-" * 60)

    count = 0
    type_stats = {}

    for entry in pvf.indexes:
        if entry.name.endswith('.ai'):
            data = pvf.extract_file(entry)
            if data:
                ai = AIParser.parse(data, entry.name)
                if ai:
                    # 统计类型
                    type_stats[ai.ai_type] = type_stats.get(ai.ai_type, 0) + 1

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {ai}")
                        if ai.skill_refs:
                            print(f"    skill_refs={ai.skill_refs[:3]}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} AI files")
    print()
    print("Type distribution:")
    for t, cnt in sorted(type_stats.items(), key=lambda x: -x[1])[:10]:
        print(f"  {AIType.get_name(t)}: {cnt}")

    pvf.close()
    print()
    print("Done!")
