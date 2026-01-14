#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF Data Parser - Python版本
基于C版本 pvf_data_parser.c 移植

解析PVF中的二进制键值对数据格式
支持 .skl, .mob, .equ, .stk, .ani, .act 等文件
"""

import struct
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any, Tuple
from enum import IntEnum


# ============================================================================
# 常量定义
# ============================================================================

PVF_DATA_MAGIC = 0xD0B0  # 小端序: 0xB0 0xD0

class PVFValueType(IntEnum):
    NULL = 0x00         # 空值
    INTEGER = 0x02      # 整数
    FLOAT = 0x04        # 浮点数
    KEY_VALUE = 0x05    # 键值对
    REFERENCE = 0x07    # 引用
    STRING = 0x09       # 字符串 (后跟长度+0x0a+数据)
    ARRAY = 0x0A        # 数组
    NESTED = 0x0B       # 嵌套对象
    UNKNOWN = 0xFF


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class PVFDataEntry:
    """单个数据条目"""
    key: int = 0
    type: PVFValueType = PVFValueType.UNKNOWN
    value: Any = None
    children: List['PVFDataEntry'] = field(default_factory=list)  # 嵌套子条目

    def __repr__(self):
        if self.type == PVFValueType.INTEGER:
            return f"Entry(key=0x{self.key:04X}, int={self.value})"
        elif self.type == PVFValueType.FLOAT:
            return f"Entry(key=0x{self.key:04X}, float={self.value:.4f})"
        elif self.type == PVFValueType.REFERENCE:
            return f"Entry(key=0x{self.key:04X}, ref=0x{self.value:04X})"
        elif self.type == PVFValueType.STRING:
            s = self.value[:30] + '...' if len(str(self.value)) > 30 else self.value
            return f"Entry(key=0x{self.key:04X}, str='{s}')"
        elif self.type == PVFValueType.ARRAY:
            return f"Entry(key=0x{self.key:04X}, array[{len(self.children)}])"
        elif self.type == PVFValueType.NESTED:
            return f"Entry(key=0x{self.key:04X}, nested[{len(self.children)}])"
        else:
            return f"Entry(key=0x{self.key:04X}, type={self.type.name}, value={self.value})"

    def has_children(self) -> bool:
        """是否有子条目"""
        return len(self.children) > 0

    def get_child(self, key: int) -> Optional['PVFDataEntry']:
        """获取指定键的子条目"""
        for child in self.children:
            if child.key == key:
                return child
        return None

    def get_child_int(self, key: int, default: int = 0) -> int:
        """获取子条目的整数值"""
        child = self.get_child(key)
        if child and child.type == PVFValueType.INTEGER:
            return child.value
        return default

    def get_child_float(self, key: int, default: float = 0.0) -> float:
        """获取子条目的浮点值"""
        child = self.get_child(key)
        if child and child.type == PVFValueType.FLOAT:
            return child.value
        return default


@dataclass
class PVFDataContainer:
    """数据容器 - 存储解析后的所有条目"""
    entries: List[PVFDataEntry] = field(default_factory=list)
    _by_key: Dict[int, List[PVFDataEntry]] = field(default_factory=dict)

    def add(self, entry: PVFDataEntry):
        self.entries.append(entry)
        if entry.key not in self._by_key:
            self._by_key[entry.key] = []
        self._by_key[entry.key].append(entry)

    def find(self, key: int) -> Optional[PVFDataEntry]:
        """查找第一个匹配的条目"""
        entries = self._by_key.get(key)
        return entries[0] if entries else None

    def find_all(self, key: int) -> List[PVFDataEntry]:
        """查找所有匹配的条目"""
        return self._by_key.get(key, [])

    def get_int(self, key: int, default: int = 0) -> int:
        entry = self.find(key)
        if entry and entry.type == PVFValueType.INTEGER:
            return entry.value
        return default

    def get_float(self, key: int, default: float = 0.0) -> float:
        entry = self.find(key)
        if entry and entry.type == PVFValueType.FLOAT:
            return entry.value
        return default

    def get_ref(self, key: int, default: int = 0) -> int:
        entry = self.find(key)
        if entry and entry.type == PVFValueType.REFERENCE:
            return entry.value
        return default

    def get_string(self, key: int, default: str = "") -> str:
        entry = self.find(key)
        if entry and entry.type == PVFValueType.STRING:
            return entry.value
        return default

    def get_ints(self, key: int) -> List[int]:
        """获取所有匹配键的整数值列表"""
        return [e.value for e in self.find_all(key)
                if e.type == PVFValueType.INTEGER]

    def get_floats(self, key: int) -> List[float]:
        """获取所有匹配键的浮点值列表"""
        return [e.value for e in self.find_all(key)
                if e.type == PVFValueType.FLOAT]

    def get_nested(self, key: int) -> Optional[PVFDataEntry]:
        """获取嵌套条目"""
        entry = self.find(key)
        if entry and entry.type in (PVFValueType.NESTED, PVFValueType.ARRAY):
            return entry
        return None

    def to_dict(self) -> Dict[str, Any]:
        """转换为字典格式"""
        result = {}
        for entry in self.entries:
            key_str = f"0x{entry.key:04X}"
            if entry.type == PVFValueType.INTEGER:
                result[key_str] = entry.value
            elif entry.type == PVFValueType.FLOAT:
                result[key_str] = round(entry.value, 4)
            elif entry.type == PVFValueType.REFERENCE:
                result[f"{key_str}_ref"] = f"0x{entry.value:04X}"
            elif entry.type == PVFValueType.STRING:
                result[f"{key_str}_str"] = entry.value
            elif entry.has_children():
                result[key_str] = [self._entry_to_dict(c) for c in entry.children]
        return result

    def _entry_to_dict(self, entry: PVFDataEntry) -> Any:
        """将单个条目转换为字典值"""
        if entry.type == PVFValueType.INTEGER:
            return entry.value
        elif entry.type == PVFValueType.FLOAT:
            return round(entry.value, 4)
        elif entry.type == PVFValueType.REFERENCE:
            return f"ref:0x{entry.value:04X}"
        elif entry.type == PVFValueType.STRING:
            return entry.value
        elif entry.has_children():
            return {f"0x{c.key:04X}": self._entry_to_dict(c) for c in entry.children}
        else:
            return entry.value


# ============================================================================
# 解析器
# ============================================================================

class PVFDataParser:
    """PVF数据解析器"""

    def __init__(self, data: bytes):
        self.data = data
        self.size = len(data)
        self.pos = 0
        self.has_magic = False

        # 检查魔数
        if self.size >= 2 and data[0] == 0xB0 and data[1] == 0xD0:
            self.has_magic = True
            self.pos = 2

    def _read_u8(self) -> int:
        if self.pos >= self.size:
            raise EOFError("End of data")
        val = self.data[self.pos]
        self.pos += 1
        return val

    def _read_u16(self) -> int:
        if self.pos + 2 > self.size:
            raise EOFError("End of data")
        val = struct.unpack_from('<H', self.data, self.pos)[0]
        self.pos += 2
        return val

    def _read_u32(self) -> int:
        if self.pos + 4 > self.size:
            raise EOFError("End of data")
        val = struct.unpack_from('<I', self.data, self.pos)[0]
        self.pos += 4
        return val

    def _read_i32(self) -> int:
        if self.pos + 4 > self.size:
            raise EOFError("End of data")
        val = struct.unpack_from('<i', self.data, self.pos)[0]
        self.pos += 4
        return val

    def _read_f32(self) -> float:
        if self.pos + 4 > self.size:
            raise EOFError("End of data")
        val = struct.unpack_from('<f', self.data, self.pos)[0]
        self.pos += 4
        return val

    def _read_string(self, length: int) -> str:
        if self.pos + length > self.size:
            raise EOFError("End of data")
        val = self.data[self.pos:self.pos + length]
        self.pos += length
        return val.decode('utf-8', errors='replace')

    def has_more(self) -> bool:
        return self.pos < self.size

    def parse_one(self) -> Optional[PVFDataEntry]:
        """解析单个条目"""
        if not self.has_more():
            return None

        entry = PVFDataEntry()
        type_byte = self._read_u8()

        if type_byte == 0x05:  # KEY_VALUE
            # 格式: 05 [key:2] [extra:2] [value_type:1] [value:...]
            entry.key = self._read_u16()
            _extra = self._read_u16()  # 额外的2字节（可能是key高位或修饰符）
            inner_type = self._read_u8()

            if inner_type == 0x02:  # INTEGER
                entry.type = PVFValueType.INTEGER
                entry.value = self._read_i32()
            elif inner_type == 0x04:  # FLOAT
                entry.type = PVFValueType.FLOAT
                entry.value = self._read_f32()
            elif inner_type == 0x07:  # REFERENCE
                entry.type = PVFValueType.REFERENCE
                entry.value = self._read_u16()
            elif inner_type == 0x05:  # NESTED KEY_VALUE
                entry.type = PVFValueType.KEY_VALUE
                entry.value = self._read_u16()
            elif inner_type == 0x09:  # STRING
                str_len = self._read_u32()
                marker = self._read_u8()  # 应该是 0x0a
                if marker == 0x0a and str_len > 0:
                    entry.type = PVFValueType.STRING
                    entry.value = self._read_string(str_len)
                else:
                    entry.type = PVFValueType.UNKNOWN
                    entry.value = (str_len, marker)
            else:
                # 其他类型，读取4字节
                entry.type = PVFValueType.UNKNOWN
                entry.value = (inner_type, self._read_u32() if self.pos + 4 <= self.size else 0)

        elif type_byte == 0x02:  # INTEGER (no key)
            entry.key = 0
            entry.type = PVFValueType.INTEGER
            entry.value = self._read_i32()

        elif type_byte == 0x04:  # FLOAT (no key)
            entry.key = 0
            entry.type = PVFValueType.FLOAT
            entry.value = self._read_f32()

        elif type_byte == 0x07:  # REFERENCE (no key)
            entry.key = 0
            entry.type = PVFValueType.REFERENCE
            entry.value = self._read_u16()

        elif type_byte == 0x09:  # STRING
            str_len = self._read_u32()
            marker = self._read_u8()
            if marker == 0x0a and str_len > 0:
                entry.key = 0
                entry.type = PVFValueType.STRING
                entry.value = self._read_string(str_len)
            else:
                entry.type = PVFValueType.UNKNOWN
                entry.value = (str_len, marker)
        else:
            # 未知类型
            entry.type = PVFValueType.UNKNOWN
            entry.value = type_byte

        return entry

    def parse_all(self) -> PVFDataContainer:
        """解析所有条目"""
        container = PVFDataContainer()

        while self.has_more():
            try:
                entry = self.parse_one()
                if entry:
                    container.add(entry)
            except EOFError:
                break
            except Exception as e:
                # 跳过错误继续
                self.pos += 1
                continue

        return container


# ============================================================================
# 便捷函数
# ============================================================================

def parse_pvf_data(data: bytes) -> PVFDataContainer:
    """解析PVF二进制数据"""
    parser = PVFDataParser(data)
    return parser.parse_all()


def dump_pvf_data(data: bytes, max_entries: int = 50) -> None:
    """打印PVF数据内容（调试用）"""
    container = parse_pvf_data(data)

    print(f"Total entries: {len(container.entries)}")
    print("-" * 60)

    for i, entry in enumerate(container.entries[:max_entries]):
        print(f"[{i:3d}] {entry}")

    if len(container.entries) > max_entries:
        print(f"... ({len(container.entries) - max_entries} more entries)")


def detect_structure(data: bytes) -> str:
    """
    检测数据结构类型

    Returns:
        "flat" - 平面键值对
        "nested" - 包含嵌套结构
        "array" - 包含数组
        "mixed" - 混合结构
    """
    container = parse_pvf_data(data)

    has_nested = False
    has_array = False

    for entry in container.entries:
        if entry.type == PVFValueType.KEY_VALUE:
            has_nested = True
        if entry.has_children():
            if entry.type == PVFValueType.ARRAY:
                has_array = True
            else:
                has_nested = True

    if has_nested and has_array:
        return "mixed"
    elif has_array:
        return "array"
    elif has_nested:
        return "nested"
    else:
        return "flat"


def analyze_pvf_data(data: bytes) -> Dict[str, Any]:
    """
    分析PVF数据结构

    Returns:
        包含统计信息的字典
    """
    container = parse_pvf_data(data)

    stats = {
        'total_entries': len(container.entries),
        'structure': detect_structure(data),
        'type_counts': {},
        'unique_keys': len(container._by_key),
        'keys': list(container._by_key.keys())[:20],  # 前20个键
    }

    # 统计类型分布
    for entry in container.entries:
        type_name = entry.type.name
        stats['type_counts'][type_name] = stats['type_counts'].get(type_name, 0) + 1

    return stats


# ============================================================================
# 特定格式解析
# ============================================================================

# 已知的键ID (从stringtable或逆向获得)
class SkillKeys:
    """技能文件常用键"""
    SKILL_ID = 0x227d        # 技能ID
    NAME = 0x2a12            # 名称引用
    ICON = 0x4185            # 图标
    MAX_LEVEL = 0x2283       # 最大等级
    REQUIRED_LEVEL = 0x2285  # 需求等级
    SP_COST = 0x2286         # SP消耗
    COOLDOWN = 0x2289        # 冷却时间
    MP_COST = 0x2222         # MP消耗
    DAMAGE_TYPE = 0x2280     # 伤害类型


class MonsterKeys:
    """怪物文件常用键"""
    MONSTER_ID = 0x1a6d      # 怪物ID
    NAME = 0x2a12            # 名称引用
    HP = 0x0b69              # 生命值
    ATTACK = 0x0b68          # 攻击力
    DEFENSE = 0x1a7a         # 防御
    EXP = 0x1277             # 经验值
    LEVEL = 0x291a           # 等级
    AI_TYPE = 0x2904         # AI类型
    MOVE_SPEED = 0x1a7c      # 移动速度


def parse_skill_file(data: bytes) -> dict:
    """解析技能文件(.skl)"""
    container = parse_pvf_data(data)

    skill = {
        'raw_entries': len(container.entries),
        'skill_id': container.get_int(SkillKeys.SKILL_ID),
        'max_level': container.get_int(SkillKeys.MAX_LEVEL),
        'required_level': container.get_int(SkillKeys.REQUIRED_LEVEL),
        'sp_cost': container.get_int(SkillKeys.SP_COST),
        'cooldown': container.get_int(SkillKeys.COOLDOWN),
        'mp_cost': container.get_int(SkillKeys.MP_COST),
        'damage_type': container.get_int(SkillKeys.DAMAGE_TYPE),
        'icon': container.get_ref(SkillKeys.ICON),
    }

    return skill


def parse_monster_file(data: bytes) -> dict:
    """解析怪物文件(.mob)"""
    container = parse_pvf_data(data)

    monster = {
        'raw_entries': len(container.entries),
        'monster_id': container.get_int(MonsterKeys.MONSTER_ID),
        'hp': container.get_int(MonsterKeys.HP),
        'attack': container.get_int(MonsterKeys.ATTACK),
        'defense': container.get_int(MonsterKeys.DEFENSE),
        'exp': container.get_int(MonsterKeys.EXP),
        'level': container.get_int(MonsterKeys.LEVEL),
        'ai_type': container.get_int(MonsterKeys.AI_TYPE),
        'move_speed': container.get_int(MonsterKeys.MOVE_SPEED),
    }

    return monster


# ============================================================================
# 主程序
# ============================================================================

if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python pvf_data_parser.py <file.skl|file.mob|file.equ>")
        sys.exit(1)

    filepath = sys.argv[1]

    with open(filepath, 'rb') as f:
        data = f.read()

    print(f"File: {filepath}")
    print(f"Size: {len(data)} bytes")
    print("=" * 60)

    # 根据扩展名选择解析方式
    if filepath.endswith('.skl'):
        skill = parse_skill_file(data)
        print("Skill Data:")
        for k, v in skill.items():
            print(f"  {k}: {v}")
    elif filepath.endswith('.mob'):
        monster = parse_monster_file(data)
        print("Monster Data:")
        for k, v in monster.items():
            print(f"  {k}: {v}")
    else:
        # 通用解析
        dump_pvf_data(data, max_entries=30)
