#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
STK Stackable Item Parser
=========================
解析PVF中的堆叠物品定义文件 (.stk)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构中等 (112-3,842字节，25-773条目)
- 定义消耗品、材料、货币等可堆叠物品

关键字段 (从样本分析):
  0x2A12 = 物品名称引用
  0x4182 = 物品类型引用
  0x4183 = 物品子类型
  0x417E = 图标引用
  0x0522 = 物品品质/稀有度
  0x4185 = 描述引用
  0x4606 = 堆叠上限
  0x1A7C = 价格/价值
  0x418B = 使用效果值
  0x418D = 售价
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

class STK:
    """堆叠物品文件键ID"""
    NAME_REF = 0x2A12           # 物品名称引用
    ITEM_TYPE = 0x4182          # 物品类型引用
    ITEM_SUBTYPE = 0x4183       # 物品子类型
    ICON_REF = 0x417E           # 图标引用
    QUALITY = 0x0522            # 物品品质
    DESC_REF = 0x4185           # 描述引用
    STACK_LIMIT = 0x4606        # 堆叠上限
    VALUE = 0x1A7C              # 价格/价值
    USE_EFFECT = 0x418B         # 使用效果值
    SELL_PRICE = 0x418D         # 售价

    # 扩展字段
    CATEGORY = 0x4601           # 分类引用
    EFFECT_TYPE = 0x418E        # 效果类型引用
    LEVEL_REQ = 0x10FC          # 等级需求
    TRADEABLE = 0x417D          # 可交易标志
    COOLDOWN = 0x418C           # 使用冷却
    DURATION = 0x41AE           # 效果持续时间


# 物品类型定义
class StackableType:
    CONSUMABLE = 1              # 消耗品
    MATERIAL = 2                # 材料
    CURRENCY = 3                # 货币
    QUEST_ITEM = 4              # 任务物品
    EVENT_ITEM = 5              # 活动物品
    MISC = 6                    # 杂项

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            1: 'consumable',
            2: 'material',
            3: 'currency',
            4: 'quest_item',
            5: 'event_item',
            6: 'misc',
        }
        return names.get(code, f'type_{code}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class StackableData:
    """堆叠物品数据"""
    name_ref: int = 0           # 名称引用
    item_type: int = 0          # 物品类型
    item_subtype: int = 0       # 物品子类型
    icon_ref: int = 0           # 图标引用
    quality: int = 0            # 品质
    desc_ref: int = 0           # 描述引用
    stack_limit: int = 99       # 堆叠上限
    value: int = 0              # 基础价值
    use_effect: int = 0         # 使用效果
    sell_price: int = 0         # 售价

    # 扩展属性
    category: int = 0           # 分类
    effect_type: int = 0        # 效果类型
    level_req: int = 0          # 等级需求
    tradeable: int = 1          # 可交易
    cooldown: int = 0           # 使用冷却 (毫秒)
    duration: int = 0           # 效果持续时间 (毫秒)

    raw_entries: int = 0        # 原始条目数
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return StackableType.get_name(self.item_type)

    def __repr__(self):
        return (f"Stackable(type={self.type_name}, quality={self.quality}, "
                f"stack={self.stack_limit}, value={self.value})")


# ============================================================================
# 解析器
# ============================================================================

class StackableParser:
    """STK堆叠物品文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[StackableData]:
        """
        解析STK文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径

        Returns:
            StackableData 或 None
        """
        # 检查魔数
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            stk = StackableData()
            stk.raw_entries = len(container.entries)
            stk.path = path

            # 核心属性
            stk.name_ref = container.get_ref(STK.NAME_REF)
            stk.item_type = container.get_ref(STK.ITEM_TYPE)
            stk.item_subtype = container.get_int(STK.ITEM_SUBTYPE)
            stk.icon_ref = container.get_ref(STK.ICON_REF)
            stk.quality = container.get_int(STK.QUALITY)
            stk.desc_ref = container.get_ref(STK.DESC_REF)
            stk.stack_limit = container.get_int(STK.STACK_LIMIT, 99)
            stk.value = container.get_int(STK.VALUE)
            stk.use_effect = container.get_int(STK.USE_EFFECT)
            stk.sell_price = container.get_int(STK.SELL_PRICE)

            # 扩展属性
            stk.category = container.get_ref(STK.CATEGORY)
            stk.effect_type = container.get_ref(STK.EFFECT_TYPE)
            stk.level_req = container.get_int(STK.LEVEL_REQ)
            stk.tradeable = container.get_int(STK.TRADEABLE, 1)
            stk.cooldown = container.get_int(STK.COOLDOWN)
            stk.duration = container.get_int(STK.DURATION)

            return stk

        except Exception as e:
            return None

    @staticmethod
    def is_valid_stk(data: bytes) -> bool:
        """检查数据是否为有效的STK格式"""
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析STK文件结构（调试用）"""
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
    print("STK Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析STK文件
    print()
    print("Sample STK files:")
    print("-" * 60)

    count = 0
    type_stats = {}

    for entry in pvf.indexes:
        if entry.name.endswith('.stk'):
            data = pvf.extract_file(entry)
            if data:
                stk = StackableParser.parse(data, entry.name)
                if stk:
                    # 统计类型
                    type_stats[stk.item_type] = type_stats.get(stk.item_type, 0) + 1

                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}:")
                        print(f"    {stk}")
                        print(f"    stack={stk.stack_limit}, level_req={stk.level_req}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} STK files")
    print()
    print("Type distribution:")
    for t, cnt in sorted(type_stats.items(), key=lambda x: -x[1])[:10]:
        print(f"  type_{t}: {cnt}")

    pvf.close()
    print()
    print("Done!")
