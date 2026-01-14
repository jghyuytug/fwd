#!/usr/bin/env python3
"""精确找出所有未解析的文件"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_formats import SkillParser, MonsterParser, EquipmentParser
from parsers import (
    AnimationParser, AttackParser, ActionParser,
    StackableParser, QuestParser, AIParser,
    KeybindParser, GameObjectParser, TileParser,
    ParticleParser, MapParser, DungeonParser,
    AliasParser, AIConfigParser, ReplayParser, UIParser,
    NPCParser, CreatureParser, MissionParser, MiscParser,
    GenericPVFParser, TextParser, BinaryParser, SmartParser
)

# 所有解析器
def get_parser(ext):
    parsers = {
        '.skl': lambda d, p: SkillParser.parse(d, p),
        '.mob': lambda d, p: MonsterParser.parse(d, p),
        '.equ': lambda d, p: EquipmentParser.parse(d, p),
        '.ani': lambda d, p: SmartParser.parse(d, p, AnimationParser.parse),
        '.act': lambda d, p: SmartParser.parse(d, p, ActionParser.parse),
        '.atk': lambda d, p: SmartParser.parse(d, p, AttackParser.parse),
        '.stk': lambda d, p: SmartParser.parse(d, p, StackableParser.parse),
        '.qst': lambda d, p: SmartParser.parse(d, p, QuestParser.parse),
        '.ai': lambda d, p: SmartParser.parse(d, p, AIParser.parse),
        '.key': lambda d, p: SmartParser.parse(d, p, KeybindParser.parse),
        '.obj': lambda d, p: SmartParser.parse(d, p, GameObjectParser.parse),
        '.til': lambda d, p: SmartParser.parse(d, p, TileParser.parse),
        '.ptl': lambda d, p: SmartParser.parse(d, p, ParticleParser.parse),
        '.map': lambda d, p: SmartParser.parse(d, p, MapParser.parse),
        '.dgn': lambda d, p: SmartParser.parse(d, p, DungeonParser.parse),
        '.als': lambda d, p: SmartParser.parse(d, p, AliasParser.parse),
        '.aic': lambda d, p: SmartParser.parse(d, p, AIConfigParser.parse),
        '.rep': lambda d, p: SmartParser.parse(d, p, ReplayParser.parse),
        '.ui': lambda d, p: SmartParser.parse(d, p, UIParser.parse),
        '.npc': lambda d, p: SmartParser.parse(d, p, NPCParser.parse),
        '.cre': lambda d, p: SmartParser.parse(d, p, CreatureParser.parse),
        '.msn': lambda d, p: SmartParser.parse(d, p, MissionParser.parse),
        '.etc': lambda d, p: SmartParser.parse(d, p, MiscParser.parse),
    }
    # 文本格式
    text_exts = {'.str', '.nut', '.txt', '.rtf', '.kor', '.jap', '.chi', '.jpn', '.log'}
    if ext in text_exts:
        return lambda d, p: TextParser.parse(d, p)
    # 二进制格式
    bin_exts = {'.bak', '.img', '.exe', '.bin', '.dl'}
    if ext in bin_exts:
        return lambda d, p: BinaryParser.parse(d, p)
    # 通用格式
    return parsers.get(ext, lambda d, p: SmartParser.parse(d, p, None))

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

missing = []
empty = []
parsed = 0
for entry in pvf.indexes:
    ext = os.path.splitext(entry.name)[1].lower()
    data = pvf.extract_file(entry)
    # 空文件也通过解析器处理（SmartParser会返回EmptyData）
    parser = get_parser(ext)
    result = parser(data if data else b'', entry.name)
    if result:
        parsed += 1
        if data is None or len(data) == 0:
            empty.append(entry.name)
    else:
        missing.append((ext, entry.name, len(data) if data else 0))

pvf.close()

print(f"Parsed: {parsed} / {len(pvf.indexes)}")
print(f"Missing (parse failed): {len(missing)}")
print(f"Empty (no data): {len(empty)}")
print()
if missing:
    print("Failed to parse:")
    for ext, name, size in missing[:20]:
        print(f"  {ext or '(none)'}: {name.split('/')[-1]} ({size} bytes)")
if empty:
    print("\nEmpty files:")
    for name in empty[:20]:
        print(f"  {name.split('/')[-1]}")
