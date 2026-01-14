#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF StringTable 加载器 - Python版本
基于C版本 pvf_stringtable.h 移植

StringTable 是 PVF 数据文件中键值对的字符串标签查找表。
用于将二进制键（如 0x00004817）转换为可读标签（如 "[grade]"）。

格式：
    [0x00000000] count (uint32) = 条目总数
    [0x00000004] indices[count] (uint32 × count) = 偏移数组
    [...] string_data (null-terminated strings)
"""

import struct
from typing import Optional, Dict, Tuple
from dataclasses import dataclass, field


# ============================================================================
# 预定义标签常量 (从C版移植)
# ============================================================================

# 基础属性
TAG_NAME = 0x00002ca5                # [name]
TAG_GRADE = 0x00004817               # [grade]
TAG_RARITY = 0x0000066c              # [rarity]
TAG_MINIMUM_LEVEL = 0x0000481c       # [minimum level]
TAG_PRICE = 0x0000481d               # [price]
TAG_REPAIR_PRICE = 0x0000481e        # [repair price]
TAG_VALUE = 0x0000481f               # [value]
TAG_WEIGHT = 0x00001d14              # [weight]

# 物理属性
TAG_PHYSICAL_ATTACK = 0x00001265     # [physical attack]
TAG_PHYSICAL_DEFENSE = 0x00001d0b    # [physical defense]
TAG_PHYSICAL_CRITICAL = 0x00004961   # [physical critical hit]
TAG_EQUIP_PHYS_ATTACK = 0x000013ba   # [equipment physical attack]
TAG_EQUIP_PHYS_DEFENSE = 0x0000139a  # [equipment physical defense]

# 魔法属性
TAG_MAGICAL_ATTACK = 0x00001d0c      # [magical attack]
TAG_MAGICAL_DEFENSE = 0x00001d0d     # [magical defense]
TAG_MAGICAL_CRITICAL = 0x00004962    # [magical critical hit]
TAG_EQUIP_MAG_ATTACK = 0x000013bb    # [equipment magical attack]
TAG_EQUIP_MAG_DEFENSE = 0x0000139c   # [equipment magical defense]

# 元素属性
TAG_FIRE_ATTACK = 0x00004821         # [fire attack]
TAG_WATER_ATTACK = 0x00004964        # [water attack]
TAG_LIGHT_ATTACK = 0x00004853        # [light attack]
TAG_DARK_ATTACK = 0x0000490d         # [dark attack]
TAG_FIRE_RESISTANCE = 0x0000495e     # [fire resistance]
TAG_WATER_RESISTANCE = 0x00004927    # [water resistance]
TAG_LIGHT_RESISTANCE = 0x00001d0f    # [light resistance]
TAG_DARK_RESISTANCE = 0x00001d0e     # [dark resistance]

# 状态属性
TAG_HIT_RECOVERY = 0x0000139b        # [hit recovery]
TAG_ALL_STATUS_RESIST = 0x0000139d   # [all active status resistance]
TAG_ATTACK_SPEED = 0x00000b2c        # [attack speed]
TAG_JUMP_SPEED = 0x00001d15          # [jump speed]
TAG_HP = 0x00000b69                  # [HP]
TAG_MP = 0x00000b6a                  # [MP]

# 装备类型
TAG_EQUIPMENT_TYPE = 0x00004826      # [equipment type]
TAG_ATTACH_TYPE = 0x0000481a         # [attach type]
TAG_USABLE_JOB = 0x00004818          # [usable job]
TAG_SEALING = 0x0000481b             # [sealing]
TAG_DURABILITY = 0x000062d3          # [durability]
TAG_SUB_TYPE = 0x000062d1            # [sub type]

# 图像/资源
TAG_ICON = 0x00004822                # [icon]
TAG_FIELD_IMAGE = 0x00004824         # [field image]

# 说明文本
TAG_BASIC_EXPLAIN = 0x0000482f       # [basic explain]
TAG_DETAIL_EXPLAIN = 0x00004831      # [detail explain]
TAG_FLAVOR_TEXT = 0x00004860         # [flavor text]

# 技能相关
TAG_SKILL = 0x00000222               # [skill]
TAG_SKILL_END = 0x00000223           # [/skill]
TAG_SKILL_DATA_UP = 0x00000ec5       # [skill data up]
TAG_COOLTIME = 0x000017a8            # [cooltime]
TAG_COOL_TIME = 0x0000482c           # [cool time]
TAG_CREATION_RATE = 0x00004820       # [creation rate]

# 技能文件常用键
TAG_SKILL_ID = 0x0000227d            # 技能ID
TAG_SKILL_NAME = 0x00002a12          # 名称引用
TAG_SKILL_ICON = 0x00004185          # 图标
TAG_MAX_LEVEL = 0x00002283           # 最大等级
TAG_REQUIRED_LEVEL = 0x00002285      # 需求等级
TAG_SP_COST = 0x00002286             # SP消耗
TAG_MP_COST = 0x00002222             # MP消耗
TAG_SKILL_COOLDOWN = 0x00002289      # 冷却时间
TAG_DAMAGE_TYPE = 0x00002280         # 伤害类型

# 怪物文件常用键
TAG_MONSTER_ID = 0x00001a6d          # 怪物ID
TAG_MONSTER_HP = 0x00000b69          # 生命值
TAG_MONSTER_ATTACK = 0x00000b68      # 攻击力
TAG_MONSTER_DEFENSE = 0x00001a7a     # 防御
TAG_MONSTER_EXP = 0x00001277         # 经验值
TAG_MONSTER_LEVEL = 0x0000291a       # 等级
TAG_MONSTER_AI_TYPE = 0x00002904     # AI类型
TAG_MONSTER_MOVE_SPEED = 0x00001a7c  # 移动速度


# ============================================================================
# StringTable 类
# ============================================================================

@dataclass
class PVFStringTable:
    """PVF StringTable 加载器"""

    count: int = 0
    indices: bytes = field(default_factory=bytes)
    string_data: bytes = field(default_factory=bytes)
    data_offset: int = 0
    loaded: bool = False

    # 缓存
    _cache: Dict[int, str] = field(default_factory=dict)

    def load_from_file(self, filepath: str) -> bool:
        """从文件加载 StringTable"""
        try:
            with open(filepath, 'rb') as f:
                data = f.read()
            return self.load_from_buffer(data)
        except IOError as e:
            print(f"[StringTable] Failed to open file: {e}")
            return False

    def load_from_buffer(self, buffer: bytes) -> bool:
        """从内存缓冲区加载 StringTable"""
        if len(buffer) < 4:
            print("[StringTable] Buffer too small")
            return False

        # 读取条目数
        self.count = struct.unpack_from('<I', buffer, 0)[0]

        # 计算索引表大小
        indices_size = self.count * 4
        self.data_offset = 4 + indices_size

        if len(buffer) < self.data_offset:
            print(f"[StringTable] Buffer too small for {self.count} indices")
            return False

        # 存储整个buffer (偏移是绝对偏移)
        self.indices = buffer[4:self.data_offset]
        self.string_data = buffer  # 存储整个buffer，因为偏移是绝对的

        self.loaded = True
        self._cache.clear()

        print(f"[StringTable] Loaded: {self.count:,} entries, "
              f"file size: {len(buffer):,} bytes")

        return True

    def is_valid_index(self, index: int) -> bool:
        """检查索引是否有效"""
        return self.loaded and 0 <= index < self.count

    def get_offset(self, index: int) -> int:
        """获取指定索引的字符串偏移"""
        if not self.is_valid_index(index):
            return -1
        return struct.unpack_from('<I', self.indices, index * 4)[0]

    def get_string(self, index: int) -> Optional[str]:
        """根据索引获取原始字符串"""
        if not self.loaded:
            return None

        # 检查缓存
        if index in self._cache:
            return self._cache[index]

        if not self.is_valid_index(index):
            return None

        # 获取绝对偏移
        offset = self.get_offset(index)
        if offset < 0 or offset >= len(self.string_data):
            return None

        # 查找字符串结束位置 (null-terminated)
        end = offset
        while end < len(self.string_data) and self.string_data[end] != 0:
            end += 1

        # 解码字符串
        try:
            s = self.string_data[offset:end].decode('utf-8', errors='replace')
            self._cache[index] = s
            return s
        except Exception:
            return None

    def get_tag(self, index: int) -> Optional[str]:
        """
        根据索引获取第一个 [xxx] 格式的标签名

        StringTable 中的字符串格式多样，标签可能出现在任意位置。
        """
        raw = self.get_string(index)
        if not raw:
            return None

        # 查找第一个 [xxx] 格式
        if '[' in raw:
            start = raw.index('[')
            if ']' in raw[start:]:
                end = raw.index(']', start) + 1
                return raw[start:end]

        # 如果没有标签格式，返回前30个字符作为标识
        return raw[:30] if len(raw) > 30 else raw

    def lookup_tag_name(self, index: int) -> str:
        """获取标签名，如果无法解析则返回十六进制表示"""
        tag = self.get_tag(index)
        if tag:
            return tag
        return f"0x{index:04X}"

    def search(self, keyword: str, max_results: int = 20,
               search_range: int = 100000) -> list:
        """
        搜索包含关键词的条目（搜索标签格式）

        Args:
            keyword: 搜索关键词
            max_results: 最大结果数
            search_range: 搜索范围（前N个条目）
        """
        results = []
        keyword_lower = keyword.lower()

        for i in range(min(self.count, search_range)):
            tag = self.get_tag(i)
            if tag and keyword_lower in tag.lower():
                results.append((i, tag))
                if len(results) >= max_results:
                    break

        return results


# ============================================================================
# 便捷函数
# ============================================================================

_global_stringtable: Optional[PVFStringTable] = None


def load_stringtable(filepath: str) -> PVFStringTable:
    """加载全局 StringTable"""
    global _global_stringtable
    _global_stringtable = PVFStringTable()
    _global_stringtable.load_from_file(filepath)
    return _global_stringtable


def get_tag_name(index: int) -> str:
    """从全局 StringTable 获取标签名"""
    if _global_stringtable and _global_stringtable.loaded:
        return _global_stringtable.lookup_tag_name(index)
    return f"0x{index:04X}"


# ============================================================================
# 预定义标签映射 (作为备份)
# ============================================================================

KNOWN_TAGS = {
    TAG_NAME: "[name]",
    TAG_GRADE: "[grade]",
    TAG_RARITY: "[rarity]",
    TAG_MINIMUM_LEVEL: "[minimum level]",
    TAG_PRICE: "[price]",
    TAG_PHYSICAL_ATTACK: "[physical attack]",
    TAG_PHYSICAL_DEFENSE: "[physical defense]",
    TAG_MAGICAL_ATTACK: "[magical attack]",
    TAG_MAGICAL_DEFENSE: "[magical defense]",
    TAG_HP: "[HP]",
    TAG_MP: "[MP]",
    TAG_ATTACK_SPEED: "[attack speed]",
    TAG_ICON: "[icon]",
    TAG_EQUIPMENT_TYPE: "[equipment type]",
    TAG_DURABILITY: "[durability]",
    TAG_SKILL_ID: "[skill id]",
    TAG_MAX_LEVEL: "[max level]",
    TAG_REQUIRED_LEVEL: "[required level]",
    TAG_SP_COST: "[sp cost]",
    TAG_MP_COST: "[mp cost]",
    TAG_SKILL_COOLDOWN: "[cooldown]",
    TAG_MONSTER_ID: "[monster id]",
    TAG_MONSTER_HP: "[monster hp]",
    TAG_MONSTER_ATTACK: "[monster attack]",
    TAG_MONSTER_EXP: "[exp]",
    TAG_MONSTER_LEVEL: "[level]",
    TAG_MONSTER_AI_TYPE: "[ai type]",
    TAG_MONSTER_MOVE_SPEED: "[move speed]",
}


def get_known_tag(index: int) -> str:
    """从预定义映射获取标签名"""
    return KNOWN_TAGS.get(index, f"0x{index:04X}")


# ============================================================================
# 主程序
# ============================================================================

if __name__ == "__main__":
    import sys

    # 默认路径
    default_path = "C:/Users/waw/Desktop/game/stringtable.bin"

    filepath = sys.argv[1] if len(sys.argv) > 1 else default_path

    print("=" * 70)
    print("PVF StringTable Loader")
    print("=" * 70)

    st = PVFStringTable()
    if not st.load_from_file(filepath):
        print(f"Failed to load: {filepath}")
        sys.exit(1)

    # 显示已知标签
    print()
    print("-" * 70)
    print("Known Tag Indices:")
    print("-" * 70)

    test_indices = [
        (TAG_NAME, "TAG_NAME"),
        (TAG_GRADE, "TAG_GRADE"),
        (TAG_RARITY, "TAG_RARITY"),
        (TAG_PHYSICAL_ATTACK, "TAG_PHYSICAL_ATTACK"),
        (TAG_MAGICAL_ATTACK, "TAG_MAGICAL_ATTACK"),
        (TAG_HP, "TAG_HP"),
        (TAG_MP, "TAG_MP"),
        (TAG_ICON, "TAG_ICON"),
        (TAG_SKILL_ID, "TAG_SKILL_ID"),
        (TAG_MONSTER_ID, "TAG_MONSTER_ID"),
        (TAG_MONSTER_ATTACK, "TAG_MONSTER_ATTACK"),
        (TAG_COOLTIME, "TAG_COOLTIME"),
    ]

    for idx, name in test_indices:
        tag = st.get_tag(idx)
        # 安全打印
        if tag:
            tag_safe = tag.encode('ascii', errors='replace').decode('ascii')
            print(f"  {name:25s} 0x{idx:04X} -> {tag_safe}")
        else:
            print(f"  {name:25s} 0x{idx:04X} -> <not found>")

    print()
    print("Done!")
