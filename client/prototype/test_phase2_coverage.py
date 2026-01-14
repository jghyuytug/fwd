#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Phase 2 Coverage Test
=====================
测试 Phase 1 + Phase 2 的总覆盖率
"""

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

def test_coverage():
    print("=" * 60)
    print("PVF Resource Parser - Phase 2 Coverage Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        return

    pvf.parse_index(max_entries=300000)
    print(f"Total files in PVF: {len(pvf.indexes)}")

    # 统计各格式
    format_counts = {}
    format_parsed = {}
    format_failed = {}

    # Phase 0: SKL, MOB, EQU
    phase0_formats = {
        '.skl': lambda d, p: SkillParser.parse(d, p),
        '.mob': lambda d, p: MonsterParser.parse(d, p),
        '.equ': lambda d, p: EquipmentParser.parse(d, p),
    }

    # Phase 1: ANI, ACT, ATK - 使用 SmartParser 自动回退
    phase1_formats = {
        '.ani': lambda d, p: SmartParser.parse(d, p, AnimationParser.parse),
        '.act': lambda d, p: SmartParser.parse(d, p, ActionParser.parse),
        '.atk': lambda d, p: SmartParser.parse(d, p, AttackParser.parse),
    }

    # Phase 2: STK, QST, AI
    phase2_formats = {
        '.stk': lambda d, p: SmartParser.parse(d, p, StackableParser.parse),
        '.qst': lambda d, p: SmartParser.parse(d, p, QuestParser.parse),
        '.ai': lambda d, p: SmartParser.parse(d, p, AIParser.parse),
    }

    # Phase 3: KEY, OBJ, TIL
    phase3_formats = {
        '.key': lambda d, p: SmartParser.parse(d, p, KeybindParser.parse),
        '.obj': lambda d, p: SmartParser.parse(d, p, GameObjectParser.parse),
        '.til': lambda d, p: SmartParser.parse(d, p, TileParser.parse),
    }

    # Phase 4: PTL, MAP, DGN
    phase4_formats = {
        '.ptl': lambda d, p: SmartParser.parse(d, p, ParticleParser.parse),
        '.map': lambda d, p: SmartParser.parse(d, p, MapParser.parse),
        '.dgn': lambda d, p: SmartParser.parse(d, p, DungeonParser.parse),
    }

    # Phase 5: ALS, ETC, AIC, REP, UI, NPC, CRE, MSN + 通用
    phase5_formats = {
        '.als': lambda d, p: SmartParser.parse(d, p, AliasParser.parse),
        '.aic': lambda d, p: SmartParser.parse(d, p, AIConfigParser.parse),
        '.rep': lambda d, p: SmartParser.parse(d, p, ReplayParser.parse),
        '.ui': lambda d, p: SmartParser.parse(d, p, UIParser.parse),
        '.npc': lambda d, p: SmartParser.parse(d, p, NPCParser.parse),
        '.cre': lambda d, p: SmartParser.parse(d, p, CreatureParser.parse),
        '.msn': lambda d, p: SmartParser.parse(d, p, MissionParser.parse),
        '.etc': lambda d, p: SmartParser.parse(d, p, MiscParser.parse),
        # 通用解析器 - 全部使用 SmartParser 自动回退到文本
        '.wrd': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.wrd')),
        '.lst': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.lst')),
        '.apd': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.apd')),
        '.gdata': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.gdata')),
        '.shp': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.shp')),
        '.tbl': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.tbl')),
        '.chr': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.chr')),
        '.wdm': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.wdm')),
        '.sd': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.sd')),
        '.twn': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.twn')),
        '.lay': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.lay')),
        '.cbt': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.cbt')),
        '.ora': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.ora')),
        '.hsp': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.hsp')),
        '.mm': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.mm')),
        '.dat': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.dat')),
        '.exj': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.exj')),
        '.rgn': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.rgn')),
        '.vm': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.vm')),
        '.pet': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.pet')),
        '.glist': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.glist')),
        '.tlk': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.tlk')),
        '.evn': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.evn')),
        '.stm': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.stm')),
        '.co': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.co')),
        '.evt': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.evt')),
        '.gdata_': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.gdata_')),
        '.ani__': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.ani__')),
        '.ani_new': lambda d, p: SmartParser.parse(d, p, lambda x, y: GenericPVFParser.parse(x, y, '.ani_new')),
        # 文本格式
        '.str': lambda d, p: TextParser.parse(d, p),
        '.nut': lambda d, p: TextParser.parse(d, p),
        '.txt': lambda d, p: TextParser.parse(d, p),
        '.rtf': lambda d, p: TextParser.parse(d, p),
        '.kor': lambda d, p: TextParser.parse(d, p),
        '.jap': lambda d, p: TextParser.parse(d, p),
        '.chi': lambda d, p: TextParser.parse(d, p),
        '.jpn': lambda d, p: TextParser.parse(d, p),
        # 二进制格式
        '.bak': lambda d, p: BinaryParser.parse(d, p),
        '.img': lambda d, p: BinaryParser.parse(d, p),
        '.exe': lambda d, p: BinaryParser.parse(d, p),
        '.bin': lambda d, p: BinaryParser.parse(d, p),
        '.log': lambda d, p: TextParser.parse(d, p),
        '.dl': lambda d, p: BinaryParser.parse(d, p),
        # 无扩展名文件
        '': lambda d, p: SmartParser.parse(d, p, None),
    }

    all_parsers = {**phase0_formats, **phase1_formats, **phase2_formats, **phase3_formats, **phase4_formats, **phase5_formats}

    # 统计所有格式（包括无扩展名）
    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        format_counts[ext] = format_counts.get(ext, 0) + 1

    print(f"\nUnique formats: {len(format_counts)}")
    print(f"Supported parsers: {list(all_parsers.keys())}")

    # 测试每个支持的格式
    print("\nTesting parsers...")
    print("-" * 60)

    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        if ext in all_parsers:
            if ext not in format_parsed:
                format_parsed[ext] = 0
                format_failed[ext] = 0

            data = pvf.extract_file(entry)
            # 空文件也要解析（传入空bytes）
            try:
                result = all_parsers[ext](data if data else b'', entry.name)
                if result:
                    format_parsed[ext] += 1
                else:
                    format_failed[ext] += 1
            except:
                format_failed[ext] += 1

    pvf.close()

    # 输出结果
    print("\nResults by format:")
    print("-" * 60)

    total_parsed = 0
    total_files = len(pvf.indexes)

    for ext in sorted(format_counts.keys(), key=lambda x: -format_counts[x]):
        count = format_counts[ext]
        if ext in format_parsed:
            parsed = format_parsed[ext]
            failed = format_failed.get(ext, 0)
            total = parsed + failed
            pct = (parsed / total * 100) if total > 0 else 0
            total_parsed += parsed
            print(f"  {ext:6}: {parsed:7,} / {total:7,} ({pct:5.1f}%)")
        else:
            print(f"  {ext:6}: {count:7,} files (no parser)")

    print()
    print("=" * 60)
    print(f"Total parsed: {total_parsed:,} / {total_files:,}")
    print(f"Coverage: {total_parsed / total_files * 100:.1f}%")
    print("=" * 60)

    # Phase 统计
    print("\nBy Phase:")
    print("-" * 30)

    phase0_total = sum(format_parsed.get(ext, 0) for ext in phase0_formats.keys())
    phase1_total = sum(format_parsed.get(ext, 0) for ext in phase1_formats.keys())
    phase2_total = sum(format_parsed.get(ext, 0) for ext in phase2_formats.keys())
    phase3_total = sum(format_parsed.get(ext, 0) for ext in phase3_formats.keys())
    phase4_total = sum(format_parsed.get(ext, 0) for ext in phase4_formats.keys())
    phase5_total = sum(format_parsed.get(ext, 0) for ext in phase5_formats.keys())

    print(f"  Phase 0 (SKL/MOB/EQU): {phase0_total:,}")
    print(f"  Phase 1 (ANI/ACT/ATK): {phase1_total:,}")
    print(f"  Phase 2 (STK/QST/AI):  {phase2_total:,}")
    print(f"  Phase 3 (KEY/OBJ/TIL): {phase3_total:,}")
    print(f"  Phase 4 (PTL/MAP/DGN): {phase4_total:,}")
    print(f"  Phase 5 (Remaining):   {phase5_total:,}")
    print(f"  Total:                 {total_parsed:,}")

    # 剩余文件统计
    remaining = total_files - total_parsed
    print(f"\nRemaining: {remaining:,} files ({remaining / total_files * 100:.1f}%)")

    # 列出未解析的格式
    if remaining > 0:
        print("\nUnparsed formats:")
        for ext in sorted(format_counts.keys()):
            if ext not in format_parsed:
                print(f"  {ext}: {format_counts[ext]} (no parser)")
        # 无扩展名文件
        no_ext = sum(1 for e in pvf.indexes if not os.path.splitext(e.name)[1])
        if no_ext > 0:
            print(f"  (no extension): {no_ext}")

if __name__ == "__main__":
    test_coverage()
