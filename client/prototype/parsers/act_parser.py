#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ACT Action Parser
=================
解析PVF中的动作定义文件 (.act)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构高度变化 (22-3,352字节，6-828条目)
- 定义角色/怪物的动作行为

关键字段:
  0x02A9 = 动作帧编号
  0x02AB = 帧时序
  0x02ED = 动作类型
  0x0780 = 效果属性
  0x02B2 = 事件触发
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

class ACT:
    """动作文件键ID"""
    FRAME_INDEX = 0x02A9        # 动作帧编号
    FRAME_TIMING = 0x02AB       # 帧时序
    ACTION_TYPE = 0x02ED        # 动作类型标识
    EFFECT_ATTR = 0x0780        # 效果属性
    EVENT_TRIGGER = 0x02B2      # 事件触发

    # 扩展字段
    LOOP = 0x02AC               # 是否循环
    SPEED = 0x02AD              # 动作速度
    BLEND_TIME = 0x02AE         # 混合时间
    PRIORITY = 0x02AF           # 优先级
    CANCEL_TYPE = 0x02B0        # 取消类型
    NEXT_ACTION = 0x02B1        # 下一动作
    SOUND_ID = 0x02B3           # 音效ID
    HITBOX = 0x02B4             # 碰撞框
    MOVEMENT = 0x02B5           # 移动参数


# 动作类型定义
class ActionType:
    IDLE = 0                    # 待机
    WALK = 1                    # 行走
    RUN = 2                     # 奔跑
    JUMP = 3                    # 跳跃
    ATTACK = 4                  # 攻击
    SKILL = 5                   # 技能
    HIT = 6                     # 受击
    KNOCKDOWN = 7               # 倒地
    GETUP = 8                   # 起身
    DEATH = 9                   # 死亡
    SIT = 10                    # 坐下
    REST = 11                   # 休息

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            0: 'idle', 1: 'walk', 2: 'run', 3: 'jump',
            4: 'attack', 5: 'skill', 6: 'hit', 7: 'knockdown',
            8: 'getup', 9: 'death', 10: 'sit', 11: 'rest',
        }
        return names.get(code, f'action_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class ActionFrame:
    """动作帧数据"""
    index: int = 0              # 帧索引
    timing: int = 0             # 时序 (毫秒)
    event: int = 0              # 事件类型

    def __repr__(self):
        return f"Frame(idx={self.index}, timing={self.timing}ms)"


@dataclass
class ActionData:
    """动作数据"""
    action_type: int = 0        # 动作类型
    frames: List[ActionFrame] = field(default_factory=list)
    effect_attr: int = 0        # 效果属性
    loop: bool = False          # 是否循环
    speed: float = 1.0          # 动作速度
    blend_time: int = 0         # 混合时间
    priority: int = 0           # 优先级
    cancel_type: int = 0        # 取消类型
    next_action: int = 0        # 下一动作
    sound_id: int = 0           # 音效ID

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return ActionType.get_name(self.action_type)

    @property
    def frame_count(self) -> int:
        return len(self.frames)

    @property
    def duration(self) -> int:
        """动作总时长 (毫秒)"""
        if not self.frames:
            return 0
        return sum(f.timing for f in self.frames)

    def __repr__(self):
        return (f"Action(type={self.type_name}, frames={self.frame_count}, "
                f"duration={self.duration}ms, loop={self.loop})")


# ============================================================================
# 解析器
# ============================================================================

class ActionParser:
    """ACT动作文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[ActionData]:
        """
        解析ACT文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            ActionData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            act = ActionData()
            act.raw_entries = len(container.entries)
            act.path = path

            # 基础属性
            act.action_type = container.get_int(ACT.ACTION_TYPE)
            act.effect_attr = container.get_int(ACT.EFFECT_ATTR)
            act.loop = container.get_int(ACT.LOOP) != 0
            act.speed = container.get_float(ACT.SPEED, 1.0)
            act.blend_time = container.get_int(ACT.BLEND_TIME)
            act.priority = container.get_int(ACT.PRIORITY)
            act.cancel_type = container.get_int(ACT.CANCEL_TYPE)
            act.next_action = container.get_int(ACT.NEXT_ACTION)
            act.sound_id = container.get_int(ACT.SOUND_ID)

            # 收集帧数据
            frame_indices = container.get_ints(ACT.FRAME_INDEX)
            frame_timings = container.get_ints(ACT.FRAME_TIMING)
            frame_events = container.get_ints(ACT.EVENT_TRIGGER)

            # 构建帧列表
            max_frames = max(len(frame_indices), len(frame_timings))
            for i in range(max_frames):
                frame = ActionFrame()
                if i < len(frame_indices):
                    frame.index = frame_indices[i]
                if i < len(frame_timings):
                    frame.timing = frame_timings[i]
                if i < len(frame_events):
                    frame.event = frame_events[i]
                act.frames.append(frame)

            return act

        except Exception as e:
            return None

    @staticmethod
    def is_valid_act(data: bytes) -> bool:
        """检查数据是否为有效的ACT格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析ACT文件结构（调试用）"""
        result = {
            'size': len(data),
            'valid': False,
            'has_magic': False,
            'entry_count': 0,
            'unique_keys': 0,
            'key_freq': {},
        }

        if len(data) < 2:
            return result

        if data[0] == 0xB0 and data[1] == 0xD0:
            result['has_magic'] = True
            result['valid'] = True

            container = parse_pvf_data(data)
            result['entry_count'] = len(container.entries)

            # 统计键频率
            key_freq = {}
            for entry in container.entries:
                if entry.key > 0:
                    key_freq[entry.key] = key_freq.get(entry.key, 0) + 1

            result['unique_keys'] = len(key_freq)
            # 只保留前10个高频键
            result['key_freq'] = dict(sorted(key_freq.items(),
                                             key=lambda x: -x[1])[:10])

        return result


# ============================================================================
# 测试
# ============================================================================

if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("ACT Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析ACT文件
    print()
    print("Sample ACT files:")
    print("-" * 60)

    count = 0
    type_stats = {}
    size_range = [float('inf'), 0]

    for entry in pvf.indexes:
        if entry.name.endswith('.act'):
            data = pvf.extract_file(entry)
            if data:
                size_range[0] = min(size_range[0], len(data))
                size_range[1] = max(size_range[1], len(data))

                act = ActionParser.parse(data, entry.name)
                if act:
                    # 统计类型
                    type_stats[act.action_type] = type_stats.get(act.action_type, 0) + 1

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {act}")
                        if act.frames:
                            print(f"    First frame: {act.frames[0]}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} ACT files")
    print(f"Size range: {size_range[0]} - {size_range[1]} bytes")
    print()
    print("Action type distribution:")
    for at, cnt in sorted(type_stats.items(), key=lambda x: -x[1])[:10]:
        print(f"  {ActionType.get_name(at)}: {cnt}")

    pvf.close()
    print()
    print("Done!")
