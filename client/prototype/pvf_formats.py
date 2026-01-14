#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF 格式解析器集合
为 SKL/MOB/EQU 等文件提供专用解析器

使用方法:
    from pvf_formats import SkillParser, MonsterParser, EquipmentParser
    from pvf_reader import PVFReader

    pvf = PVFReader("Script.pvf")
    pvf.open()
    pvf.parse_index()

    # 解析技能
    skill_data = pvf.extract_file(pvf.get_file("skill/fighter/..."))
    skill = SkillParser.parse(skill_data)
"""

from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any
from pvf_data_parser import (
    parse_pvf_data, PVFDataContainer, PVFValueType
)


# ============================================================================
# 键ID常量 (从StringTable获取)
# ============================================================================

# 技能文件键
class SKL:
    """技能文件键ID"""
    SKILL_ID = 0x227d           # 技能ID
    NAME = 0x2a12               # 名称引用
    ICON = 0x4185               # 图标
    MAX_LEVEL = 0x2283          # 最大等级
    REQUIRED_LEVEL = 0x2285     # 需求等级
    SP_COST = 0x2286            # SP消耗
    COOLDOWN = 0x2289           # 冷却时间
    MP_COST = 0x2222            # MP消耗
    DAMAGE_TYPE = 0x2280        # 伤害类型
    COMMAND = 0x1a47            # 指令键
    PHYSICAL_ATTACK = 0x1265    # 物理攻击
    MAGICAL_ATTACK = 0x1d0c     # 魔法攻击
    LEVEL_DATA = 0x222a         # 等级数据
    DURATION = 0x17b5           # 持续时间
    RANGE = 0x1a48              # 范围
    CAST_TIME = 0x17a9          # 施法时间


# 怪物文件键
class MOB:
    """怪物文件键ID"""
    MONSTER_ID = 0x1a6d         # 怪物ID
    NAME = 0x2a12               # 名称引用
    HP = 0x0b69                 # 生命值
    MP = 0x0b6a                 # 魔法值
    ATTACK = 0x0b68             # 攻击力
    DEFENSE = 0x1a7a            # 防御
    EXP = 0x1277                # 经验值
    LEVEL = 0x291a              # 等级
    AI_TYPE = 0x2904            # AI类型
    MOVE_SPEED = 0x1a7c         # 移动速度
    ATTACK_SPEED = 0x0b2c       # 攻击速度
    ELEMENT = 0x1a78            # 元素属性
    SIZE = 0x1a79               # 体型
    DROP_TABLE = 0x1a7b         # 掉落表


# 装备文件键 (从实际PVF数据分析)
class EQU:
    """装备文件键ID"""
    ITEM_TYPE = 0x417D          # 物品类型
    NAME = 0x417E               # 名称引用
    ICON = 0x4185               # 图标
    EQUIP_TYPE = 0x0522         # 装备类型
    LEVEL_REQ = 0x5B8C          # 需求等级
    PHYS_ATTACK = 0x1302        # 物理攻击
    MAG_ATTACK = 0x12E0         # 魔法攻击
    PRICE = 0x418B              # 价格
    REPAIR_PRICE = 0x418C       # 修理价格
    DURABILITY = 0x418D         # 耐久度
    WEIGHT = 0x1A72             # 重量
    GRADE = 0x5B8B              # 品质等级
    RARITY = 0x11A9             # 稀有度
    # 以下键需要进一步验证
    JOB_REQ = 0x4189            # 职业限制引用
    PHYS_DEFENSE = 0x1303       # 物理防御
    MAG_DEFENSE = 0x12E1        # 魔法防御


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class SkillData:
    """技能数据"""
    skill_id: int = 0
    name_ref: int = 0
    icon_ref: int = 0
    max_level: int = 1
    required_level: int = 1
    sp_cost: int = 0
    mp_cost: int = 0
    cooldown: int = 0           # 毫秒
    damage_type: int = 0        # 0=物理, 1=魔法
    command: str = ""
    duration: int = 0           # 毫秒
    cast_time: int = 0          # 毫秒
    physical_attack: int = 0
    magical_attack: int = 0
    level_data: List[Dict] = field(default_factory=list)  # 每级数据
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return (f"Skill(id={self.skill_id}, max_lv={self.max_level}, "
                f"sp={self.sp_cost}, cd={self.cooldown}ms)")


@dataclass
class MonsterData:
    """怪物数据"""
    monster_id: int = 0
    name_ref: int = 0
    level: int = 1
    hp: int = 100
    mp: int = 0
    attack: int = 10
    defense: int = 0
    exp: int = 0
    move_speed: float = 1.0
    attack_speed: float = 1.0
    ai_type: int = 0
    element: int = 0
    size: int = 1               # 1=小, 2=中, 3=大
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return (f"Monster(id={self.monster_id}, lv={self.level}, "
                f"hp={self.hp}, atk={self.attack})")


@dataclass
class EquipmentData:
    """装备数据"""
    item_id: int = 0
    name_ref: int = 0
    grade: int = 0              # 品质等级
    rarity: int = 0
    level_req: int = 1
    job_req: int = 0            # 职业限制引用
    phys_attack: int = 0
    mag_attack: int = 0
    phys_defense: int = 0
    mag_defense: int = 0
    durability: int = 100
    price: int = 0
    icon_ref: int = 0
    equip_type: int = 0         # 装备类型
    weight: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        grade_names = ["common", "uncommon", "rare", "unique", "legendary"]
        g = grade_names[self.grade] if 0 <= self.grade < len(grade_names) else f"g{self.grade}"
        return (f"Equip(id={self.item_id}, grade={g}, "
                f"lv={self.level_req}, atk={self.phys_attack})")


# ============================================================================
# 解析器
# ============================================================================

class SkillParser:
    """技能文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> SkillData:
        """解析技能数据"""
        container = parse_pvf_data(data)
        skill = SkillData()

        skill.raw_entries = len(container.entries)
        skill.path = path

        # 基础属性
        skill.skill_id = container.get_int(SKL.SKILL_ID)
        skill.name_ref = container.get_ref(SKL.NAME)
        skill.icon_ref = container.get_ref(SKL.ICON)
        skill.max_level = container.get_int(SKL.MAX_LEVEL, 1)
        skill.required_level = container.get_int(SKL.REQUIRED_LEVEL, 1)
        skill.sp_cost = container.get_int(SKL.SP_COST)
        skill.mp_cost = container.get_int(SKL.MP_COST)
        skill.cooldown = container.get_int(SKL.COOLDOWN)
        skill.damage_type = container.get_int(SKL.DAMAGE_TYPE)
        skill.duration = container.get_int(SKL.DURATION)
        skill.cast_time = container.get_int(SKL.CAST_TIME)
        skill.physical_attack = container.get_int(SKL.PHYSICAL_ATTACK)
        skill.magical_attack = container.get_int(SKL.MAGICAL_ATTACK)
        skill.command = container.get_string(SKL.COMMAND)

        return skill

    @staticmethod
    def parse_with_stringtable(data: bytes, stringtable, path: str = "") -> Dict:
        """解析技能数据并转换键名"""
        skill = SkillParser.parse(data, path)

        return {
            'skill_id': skill.skill_id,
            'name': stringtable.get_tag(skill.name_ref) if skill.name_ref else None,
            'max_level': skill.max_level,
            'required_level': skill.required_level,
            'sp_cost': skill.sp_cost,
            'mp_cost': skill.mp_cost,
            'cooldown_ms': skill.cooldown,
            'damage_type': 'physical' if skill.damage_type == 0 else 'magical',
            'duration_ms': skill.duration,
            'path': skill.path,
        }


class MonsterParser:
    """怪物文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> MonsterData:
        """解析怪物数据"""
        container = parse_pvf_data(data)
        monster = MonsterData()

        monster.raw_entries = len(container.entries)
        monster.path = path

        # 基础属性
        monster.monster_id = container.get_int(MOB.MONSTER_ID)
        monster.name_ref = container.get_ref(MOB.NAME)
        monster.level = container.get_int(MOB.LEVEL, 1)
        monster.hp = container.get_int(MOB.HP, 100)
        monster.mp = container.get_int(MOB.MP)
        monster.attack = container.get_int(MOB.ATTACK, 10)
        monster.defense = container.get_int(MOB.DEFENSE)
        monster.exp = container.get_int(MOB.EXP)
        monster.move_speed = container.get_float(MOB.MOVE_SPEED, 1.0)
        monster.attack_speed = container.get_float(MOB.ATTACK_SPEED, 1.0)
        monster.ai_type = container.get_int(MOB.AI_TYPE)
        monster.element = container.get_int(MOB.ELEMENT)
        monster.size = container.get_int(MOB.SIZE, 1)

        return monster

    @staticmethod
    def parse_with_stringtable(data: bytes, stringtable, path: str = "") -> Dict:
        """解析怪物数据并转换键名"""
        monster = MonsterParser.parse(data, path)

        return {
            'monster_id': monster.monster_id,
            'name': stringtable.get_tag(monster.name_ref) if monster.name_ref else None,
            'level': monster.level,
            'hp': monster.hp,
            'mp': monster.mp,
            'attack': monster.attack,
            'defense': monster.defense,
            'exp': monster.exp,
            'move_speed': round(monster.move_speed, 2),
            'attack_speed': round(monster.attack_speed, 2),
            'ai_type': monster.ai_type,
            'element': monster.element,
            'size': ['small', 'medium', 'large'][min(monster.size, 2)],
            'path': monster.path,
        }


class EquipmentParser:
    """装备文件解析器"""

    # 品质名称
    GRADE_NAMES = ['common', 'uncommon', 'rare', 'unique', 'legendary']

    # 装备类型
    EQUIP_TYPES = {
        0: 'weapon',
        1: 'armor',
        2: 'pants',
        3: 'shoulder',
        4: 'shoes',
        5: 'belt',
        6: 'ring',
        7: 'necklace',
        8: 'bracelet',
        9: 'title',
        10: 'pet',
    }

    @staticmethod
    def parse(data: bytes, path: str = "") -> EquipmentData:
        """解析装备数据"""
        container = parse_pvf_data(data)
        equip = EquipmentData()

        equip.raw_entries = len(container.entries)
        equip.path = path

        # 基础属性 (使用更新后的键)
        equip.item_id = container.get_int(EQU.ITEM_TYPE)
        equip.name_ref = container.get_ref(EQU.NAME)
        equip.grade = container.get_int(EQU.GRADE)
        equip.rarity = container.get_int(EQU.RARITY)
        equip.level_req = container.get_int(EQU.LEVEL_REQ, 1)
        equip.job_req = container.get_ref(EQU.JOB_REQ)
        equip.phys_attack = container.get_int(EQU.PHYS_ATTACK)
        equip.mag_attack = container.get_int(EQU.MAG_ATTACK)
        equip.phys_defense = container.get_int(EQU.PHYS_DEFENSE)
        equip.mag_defense = container.get_int(EQU.MAG_DEFENSE)
        equip.durability = container.get_int(EQU.DURABILITY, 100)
        equip.price = container.get_int(EQU.PRICE)
        equip.icon_ref = container.get_ref(EQU.ICON)
        equip.equip_type = container.get_int(EQU.EQUIP_TYPE)
        equip.weight = container.get_int(EQU.WEIGHT)

        return equip

    @staticmethod
    def parse_with_stringtable(data: bytes, stringtable, path: str = "") -> Dict:
        """解析装备数据并转换键名"""
        equip = EquipmentParser.parse(data, path)

        grade_name = (EquipmentParser.GRADE_NAMES[equip.grade]
                      if 0 <= equip.grade < len(EquipmentParser.GRADE_NAMES)
                      else f"grade_{equip.grade}")

        equip_type = EquipmentParser.EQUIP_TYPES.get(
            equip.equip_type, f"type_{equip.equip_type}")

        return {
            'item_id': equip.item_id,
            'name': stringtable.get_tag(equip.name_ref) if equip.name_ref else None,
            'grade': grade_name,
            'rarity': equip.rarity,
            'level_req': equip.level_req,
            'job_req': equip.job_req,
            'phys_attack': equip.phys_attack,
            'mag_attack': equip.mag_attack,
            'phys_defense': equip.phys_defense,
            'mag_defense': equip.mag_defense,
            'durability': equip.durability,
            'price': equip.price,
            'equip_type': equip_type,
            'path': equip.path,
        }


# ============================================================================
# 便捷函数
# ============================================================================

def parse_file(data: bytes, ext: str, path: str = "") -> Any:
    """
    根据扩展名自动选择解析器

    Args:
        data: 文件二进制数据
        ext: 文件扩展名 (.skl, .mob, .equ)
        path: 文件路径

    Returns:
        SkillData, MonsterData, EquipmentData 或 None
    """
    ext = ext.lower().strip('.')

    if ext == 'skl':
        return SkillParser.parse(data, path)
    elif ext == 'mob':
        return MonsterParser.parse(data, path)
    elif ext == 'equ':
        return EquipmentParser.parse(data, path)
    else:
        return None


# ============================================================================
# 测试
# ============================================================================

if __name__ == "__main__":
    import sys
    sys.path.insert(0, '.')

    from pvf_reader import PVFReader
    from pvf_stringtable import PVFStringTable

    # 加载资源
    pvf_path = "C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf"
    st_path = "C:/Users/waw/Desktop/game/stringtable.bin"

    print("=" * 70)
    print("PVF Format Parsers Test")
    print("=" * 70)

    # 加载 StringTable
    st = PVFStringTable()
    st.load_from_file(st_path)

    # 加载 PVF
    pvf = PVFReader(pvf_path)
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    if not pvf.parse_index(max_entries=50000):
        print("Failed to parse PVF index")
        sys.exit(1)

    # 测试技能解析
    print()
    print("-" * 70)
    print("Sample Skills:")
    print("-" * 70)

    skill_count = 0
    for entry in pvf.indexes:
        if entry.name.endswith('.skl') and 'fighter' in entry.name:
            data = pvf.extract_file(entry)
            if data:
                skill = SkillParser.parse(data, entry.name)
                try:
                    print(f"  {skill}")
                except:
                    print(f"  Skill(id={skill.skill_id})")
                skill_count += 1
                if skill_count >= 5:
                    break

    # 测试怪物解析
    print()
    print("-" * 70)
    print("Sample Monsters:")
    print("-" * 70)

    mob_count = 0
    for entry in pvf.indexes:
        if entry.name.endswith('.mob'):
            data = pvf.extract_file(entry)
            if data:
                monster = MonsterParser.parse(data, entry.name)
                try:
                    print(f"  {monster}")
                except:
                    print(f"  Monster(id={monster.monster_id})")
                mob_count += 1
                if mob_count >= 5:
                    break

    # 测试装备解析
    print()
    print("-" * 70)
    print("Sample Equipment:")
    print("-" * 70)

    equ_count = 0
    for entry in pvf.indexes:
        if entry.name.endswith('.equ') and 'weapon' in entry.name:
            data = pvf.extract_file(entry)
            if data:
                equip = EquipmentParser.parse(data, entry.name)
                try:
                    print(f"  {equip}")
                except:
                    print(f"  Equip(id={equip.item_id})")
                equ_count += 1
                if equ_count >= 5:
                    break

    pvf.close()
    print()
    print("Done!")
