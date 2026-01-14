#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
分析解析失败的文件
"""

import sys
import os
from collections import defaultdict

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
    GenericPVFParser, TextParser, BinaryParser
)

# 所有解析器
PARSERS = {
    '.skl': SkillParser.parse, '.mob': MonsterParser.parse, '.equ': EquipmentParser.parse,
    '.ani': AnimationParser.parse, '.act': ActionParser.parse, '.atk': AttackParser.parse,
    '.stk': StackableParser.parse, '.qst': QuestParser.parse, '.ai': AIParser.parse,
    '.key': KeybindParser.parse, '.obj': GameObjectParser.parse, '.til': TileParser.parse,
    '.ptl': ParticleParser.parse, '.map': MapParser.parse, '.dgn': DungeonParser.parse,
    '.als': AliasParser.parse, '.aic': AIConfigParser.parse, '.rep': ReplayParser.parse,
    '.ui': UIParser.parse, '.npc': NPCParser.parse, '.cre': CreatureParser.parse,
    '.msn': MissionParser.parse, '.etc': MiscParser.parse,
}

def analyze_failures():
    print("=" * 70)
    print("Failure Analysis")
    print("=" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    failures = defaultdict(list)

    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        if ext in PARSERS:
            data = pvf.extract_file(entry)
            if data:
                result = PARSERS[ext](data, entry.name)
                if result is None:
                    failures[ext].append({
                        'name': entry.name,
                        'size': len(data),
                        'first_bytes': data[:16].hex() if len(data) >= 16 else data.hex(),
                        'is_pvf': len(data) >= 2 and data[0] == 0xB0 and data[1] == 0xD0,
                    })

    pvf.close()

    # 输出失败分析
    total_failures = sum(len(f) for f in failures.values())
    print(f"\nTotal failures: {total_failures}")
    print()

    for ext in sorted(failures.keys(), key=lambda x: -len(failures[x])):
        failed = failures[ext]
        print(f"\n{ext}: {len(failed)} failures")
        print("-" * 50)

        # 分析失败原因
        pvf_count = sum(1 for f in failed if f['is_pvf'])
        non_pvf_count = len(failed) - pvf_count

        print(f"  PVF format: {pvf_count}, Non-PVF: {non_pvf_count}")

        # 显示前3个失败样本
        for i, f in enumerate(failed[:3]):
            print(f"  [{i+1}] {f['name'].split('/')[-1]}")
            print(f"      size={f['size']}, is_pvf={f['is_pvf']}")
            print(f"      bytes: {f['first_bytes'][:32]}...")

if __name__ == "__main__":
    analyze_failures()
