#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
KEY Keybind Parser
==================
解析PVF中的按键绑定定义文件 (.key)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构较小 (27-982字节，8-252条目)
- 定义技能/动作的快捷键绑定

关键字段 (从样本分析):
  0x0227 = 绑定键/动作引用
  0x0301 = 绑定类型
  0x0303 = 技能/动作引用
  0x0304 = 按键码
  0x0305 = 修饰键
  0x0323 = 触发条件
  0x0228 = 附加绑定
  0x049E = 槽位索引
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

class KEY:
    """按键绑定文件键ID"""
    BIND_ACTION = 0x0227        # 绑定动作引用
    BIND_TYPE = 0x0301          # 绑定类型
    SKILL_REF = 0x0303          # 技能/动作引用
    KEY_CODE = 0x0304           # 按键码
    MODIFIER = 0x0305           # 修饰键 (Shift/Ctrl/Alt)
    TRIGGER = 0x0323            # 触发条件
    EXTRA_BIND = 0x0228         # 附加绑定
    SLOT_INDEX = 0x049E         # 槽位索引

    # 扩展字段
    PRIORITY = 0x0326           # 优先级
    CONTEXT = 0x121C            # 上下文引用
    COMBO = 0x0325              # 组合键
    ALT_BIND = 0x0308           # 替代绑定
    ACTION_ID = 0x0302          # 动作ID


# 绑定类型定义
class BindType:
    SKILL = 1                   # 技能
    ITEM = 2                    # 物品
    ACTION = 3                  # 动作
    MENU = 4                    # 菜单
    EMOTE = 5                   # 表情
    MACRO = 6                   # 宏

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            1: 'skill',
            2: 'item',
            3: 'action',
            4: 'menu',
            5: 'emote',
            6: 'macro',
        }
        return names.get(code, f'bind_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class KeybindEntry:
    """单个按键绑定"""
    action_ref: int = 0         # 动作引用
    key_code: int = 0           # 按键码
    modifier: int = 0           # 修饰键
    slot: int = 0               # 槽位

    def __repr__(self):
        return f"Keybind(action={self.action_ref}, key={self.key_code}, mod={self.modifier})"


@dataclass
class KeybindData:
    """按键绑定数据"""
    bind_type: int = 0          # 绑定类型
    bindings: List[KeybindEntry] = field(default_factory=list)
    skill_ref: int = 0          # 技能引用
    trigger: int = 0            # 触发条件

    # 扩展属性
    priority: int = 0           # 优先级
    context: int = 0            # 上下文
    action_id: int = 0          # 动作ID

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return BindType.get_name(self.bind_type)

    @property
    def binding_count(self) -> int:
        return len(self.bindings)

    def __repr__(self):
        return (f"Keybind(type={self.type_name}, bindings={self.binding_count}, "
                f"skill_ref={self.skill_ref})")


# ============================================================================
# 解析器
# ============================================================================

class KeybindParser:
    """KEY按键绑定文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[KeybindData]:
        """
        解析KEY文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            KeybindData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            key = KeybindData()
            key.raw_entries = len(container.entries)
            key.path = path

            # 核心属性
            key.bind_type = container.get_int(KEY.BIND_TYPE)
            key.skill_ref = container.get_ref(KEY.SKILL_REF)
            key.trigger = container.get_int(KEY.TRIGGER)

            # 收集绑定
            key.bindings = []
            action_refs = container.get_ints(KEY.BIND_ACTION)
            key_codes = container.get_ints(KEY.KEY_CODE)
            modifiers = container.get_ints(KEY.MODIFIER)
            slots = container.get_ints(KEY.SLOT_INDEX)

            max_binds = max(len(action_refs), len(key_codes))
            for i in range(max_binds):
                entry = KeybindEntry()
                if i < len(action_refs):
                    entry.action_ref = action_refs[i]
                if i < len(key_codes):
                    entry.key_code = key_codes[i]
                if i < len(modifiers):
                    entry.modifier = modifiers[i]
                if i < len(slots):
                    entry.slot = slots[i]
                key.bindings.append(entry)

            # 扩展属性
            key.priority = container.get_int(KEY.PRIORITY)
            key.context = container.get_ref(KEY.CONTEXT)
            key.action_id = container.get_int(KEY.ACTION_ID)

            return key

        except Exception as e:
            return None

    @staticmethod
    def is_valid_key(data: bytes) -> bool:
        """检查数据是否为有效的KEY格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析KEY文件结构（调试用）"""
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
    print("KEY Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    print()
    print("Sample KEY files:")
    print("-" * 60)

    count = 0
    for entry in pvf.indexes:
        if entry.name.endswith('.key'):
            data = pvf.extract_file(entry)
            if data:
                key = KeybindParser.parse(data, entry.name)
                if key:
                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {key}")
                        if key.bindings:
                            print(f"    First bind: {key.bindings[0]}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} KEY files")

    pvf.close()
    print("Done!")
