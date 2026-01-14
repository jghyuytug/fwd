#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF语义解析器
=============
使用语义映射表解析PVF内容
"""

import sys
import os
from dataclasses import dataclass, field
from typing import Dict, Any, List, Optional

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_semantic_keys import EQU_KEYS, MOB_KEYS, SKL_KEYS, STK_KEYS, QST_KEYS, ACT_KEYS, ATK_KEYS


@dataclass
class Equipment:
    """装备数据"""
    path: str = ""
    name: str = ""
    item_code: int = 0
    item_level: int = 0
    grade: int = 0
    physical_attack: int = 0
    physical_defense: int = 0
    required_level: int = 0
    durability: int = 0
    tradeable: bool = True
    raw_keys: Dict[int, Any] = field(default_factory=dict)


@dataclass
class Monster:
    """怪物数据"""
    path: str = ""
    name: str = ""
    hp: int = 0
    attack: int = 0
    defense: int = 0
    exp: int = 0
    level: int = 0
    move_speed: int = 0
    ai_type: int = 0
    boss_flag: int = 0
    raw_keys: Dict[int, Any] = field(default_factory=dict)


@dataclass
class Skill:
    """技能数据"""
    path: str = ""
    name: str = ""
    max_level: int = 0
    sp_cost: int = 0
    mp_cost: int = 0
    cooldown: int = 0
    base_damage: int = 0
    skill_type: int = 0
    raw_keys: Dict[int, Any] = field(default_factory=dict)


class SemanticParser:
    """语义解析器"""

    def __init__(self, pvf_path: str):
        self.pvf = PVFReader(pvf_path)
        self.pvf.open()
        self.pvf.parse_index(max_entries=300000)

    def close(self):
        self.pvf.close()

    def _extract_keyed_values(self, data: bytes) -> Dict[int, Any]:
        """提取所有有键的值"""
        result = {}
        if not data or len(data) < 2:
            return result
        if data[0] != 0xB0 or data[1] != 0xD0:
            return result

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.key > 0:  # 只取有键的条目
                    result[e.key] = e.value
        except:
            pass

        return result

    def parse_equipment(self, path: str) -> Optional[Equipment]:
        """解析装备文件"""
        entry = None
        for e in self.pvf.indexes:
            if e.name == path:
                entry = e
                break

        if not entry:
            return None

        data = self.pvf.extract_file(entry)
        keys = self._extract_keyed_values(data)

        equ = Equipment()
        equ.path = path
        equ.raw_keys = keys

        # 映射语义字段
        equ.name = str(keys.get(0x2A12, ""))
        equ.item_code = keys.get(0x5DB0, 0)
        equ.item_level = keys.get(0x5DAE, 0)
        equ.grade = keys.get(0x417D, 0)
        equ.physical_attack = keys.get(0x418C, 0)
        equ.physical_defense = keys.get(0x418D, 0)
        equ.required_level = keys.get(0x5B8C, 0)
        equ.durability = keys.get(0x0522, 0)
        equ.tradeable = keys.get(0x5B8B, 1) != 0

        return equ

    def parse_monster(self, path: str) -> Optional[Monster]:
        """解析怪物文件"""
        entry = None
        for e in self.pvf.indexes:
            if e.name == path:
                entry = e
                break

        if not entry:
            return None

        data = self.pvf.extract_file(entry)
        keys = self._extract_keyed_values(data)

        mob = Monster()
        mob.path = path
        mob.raw_keys = keys

        # 映射语义字段
        mob.name = str(keys.get(0x2A12, ""))
        mob.hp = keys.get(0x0B69, 0)
        mob.attack = keys.get(0x0B68, 0)
        mob.defense = keys.get(0x1A7A, 0)
        mob.exp = keys.get(0x1A7C, 0)
        mob.level = keys.get(0x4338, 0)
        mob.move_speed = keys.get(0x020E, 0)
        mob.ai_type = keys.get(0x01E0, 0)
        mob.boss_flag = keys.get(0x2A14, 0)

        return mob

    def parse_skill(self, path: str) -> Optional[Skill]:
        """解析技能文件"""
        entry = None
        for e in self.pvf.indexes:
            if e.name == path:
                entry = e
                break

        if not entry:
            return None

        data = self.pvf.extract_file(entry)
        keys = self._extract_keyed_values(data)

        skl = Skill()
        skl.path = path
        skl.raw_keys = keys

        # 映射语义字段
        skl.name = str(keys.get(0x2A12, ""))
        skl.max_level = keys.get(0x2283, 0)
        skl.mp_cost = keys.get(0x22A0, 0)
        skl.cooldown = keys.get(0x427E, 0)
        skl.base_damage = keys.get(0x2280, 0)
        skl.skill_type = keys.get(0x2282, 0)

        # SP cost 是元组格式
        sp = keys.get(0x0500, 0)
        if isinstance(sp, tuple):
            skl.sp_cost = sp[1] if len(sp) > 1 else 0
        else:
            skl.sp_cost = sp

        return skl

    def list_files(self, ext: str, limit: int = 20) -> List[str]:
        """列出指定扩展名的文件"""
        files = []
        for e in self.pvf.indexes:
            if e.name.endswith(ext):
                files.append(e.name)
                if len(files) >= limit:
                    break
        return files


def demo():
    """演示语义解析"""
    print("=" * 70)
    print("PVF Semantic Parser Demo")
    print("=" * 70)

    parser = SemanticParser('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')

    # 解析怪物
    print("\n--- Monster Examples ---")
    mob_files = parser.list_files('.mob', 5)
    for path in mob_files:
        mob = parser.parse_monster(path)
        if mob:
            name = mob.name.encode('ascii', 'replace').decode('ascii')[:20]
            print(f"  {path.split('/')[-1][:30]:32} HP={mob.hp:>6} ATK={mob.attack:>5} DEF={mob.defense:>4} EXP={mob.exp:>6} LV={mob.level:>2}")

    # 解析装备
    print("\n--- Equipment Examples ---")
    equ_files = parser.list_files('.equ', 5)
    for path in equ_files:
        equ = parser.parse_equipment(path)
        if equ:
            print(f"  {path.split('/')[-1][:32]:34} Code={equ.item_code:>5} LV={equ.item_level:>4} ATK={equ.physical_attack:>4} DEF={equ.physical_defense:>4}")

    # 解析技能
    print("\n--- Skill Examples ---")
    skl_files = parser.list_files('.skl', 5)
    for path in skl_files:
        skl = parser.parse_skill(path)
        if skl:
            print(f"  {path.split('/')[-1][:32]:34} MaxLV={skl.max_level:>2} SP={skl.sp_cost:>6} MP={skl.mp_cost:>4} DMG={skl.base_damage:>4}")

    parser.close()
    print("\nDone!")


if __name__ == "__main__":
    demo()
