#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF内容分析器
=============
深入理解已解析数据的语义内容
"""

import sys
import os
from collections import defaultdict, Counter

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable

# 加载StringTable用于键名解析
stringtable = None
try:
    stringtable = PVFStringTable('C:/Users/waw/Desktop/新建文件夹 (2)/stringtable.bin')
    if stringtable.load():
        print(f"[StringTable] Loaded {len(stringtable.strings)} strings")
except:
    print("[StringTable] Failed to load")

def analyze_key_usage():
    """分析所有文件中键的使用频率和分布"""
    print("\n" + "=" * 60)
    print("Key Usage Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 按扩展名分组统计键
    key_by_ext = defaultdict(Counter)
    key_names = {}

    sample_count = 0
    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        if ext not in ['.skl', '.mob', '.equ', '.act', '.atk', '.stk', '.qst']:
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                key_by_ext[ext][e.key] += 1
                if e.key not in key_names and stringtable:
                    name = stringtable.get_string(e.key)
                    if name:
                        key_names[e.key] = name
        except:
            pass

        sample_count += 1
        if sample_count >= 10000:
            break

    pvf.close()

    # 输出每种格式的top键
    for ext in sorted(key_by_ext.keys()):
        print(f"\n{ext} Top 15 Keys:")
        print("-" * 50)
        for key, count in key_by_ext[ext].most_common(15):
            name = key_names.get(key, f"unknown_{key:04X}")
            print(f"  0x{key:04X} ({name:30}): {count:,}")

    return key_names

def analyze_references():
    """分析文件间的引用关系"""
    print("\n" + "=" * 60)
    print("Reference Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 建立路径索引
    path_index = {e.name.lower(): e for e in pvf.indexes}

    # 分析引用
    ref_count = Counter()
    ref_sources = defaultdict(list)

    sample = 0
    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        if ext not in ['.skl', '.mob', '.equ']:
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.type == PVFValueType.REF:
                    ref_path = e.value.lower() if isinstance(e.value, str) else str(e.value)
                    ref_count[ref_path] += 1
                    if len(ref_sources[ref_path]) < 3:
                        ref_sources[ref_path].append(entry.name)
        except:
            pass

        sample += 1
        if sample >= 5000:
            break

    pvf.close()

    # 输出引用统计
    print(f"\nTotal unique references: {len(ref_count)}")
    print("\nTop 20 most referenced files:")
    print("-" * 50)
    for path, count in ref_count.most_common(20):
        exists = "EXISTS" if path in path_index else "MISSING"
        print(f"  {count:5} refs: {path[:50]} [{exists}]")

    # 引用类型统计
    ref_ext = Counter()
    for path in ref_count:
        ext = os.path.splitext(path)[1].lower()
        ref_ext[ext] += ref_count[path]

    print("\nReferences by file type:")
    for ext, count in ref_ext.most_common(10):
        print(f"  {ext or '(none)':8}: {count:,}")

def analyze_skill_content():
    """深入分析技能文件内容"""
    print("\n" + "=" * 60)
    print("Skill Content Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    skills = []
    for entry in pvf.indexes:
        if not entry.name.endswith('.skl'):
            continue
        data = pvf.extract_file(entry)
        if not data:
            continue

        try:
            container = parse_pvf_data(data)
            skill_info = {
                'path': entry.name,
                'entries': len(container.entries),
                'keys': set(),
                'refs': [],
                'ints': {},
            }

            for e in container.entries:
                skill_info['keys'].add(e.key)
                if e.type == PVFValueType.REF:
                    skill_info['refs'].append((e.key, e.value))
                elif e.type == PVFValueType.INT:
                    if e.key not in skill_info['ints']:
                        skill_info['ints'][e.key] = []
                    skill_info['ints'][e.key].append(e.value)

            skills.append(skill_info)
        except:
            pass

    pvf.close()

    print(f"Total skills: {len(skills)}")

    # 分析技能目录结构
    skill_dirs = Counter()
    for s in skills:
        parts = s['path'].split('/')
        if len(parts) >= 3:
            skill_dirs['/'.join(parts[:3])] += 1

    print("\nSkill directories:")
    for dir_path, count in skill_dirs.most_common(15):
        print(f"  {dir_path}: {count}")

    # 分析常见键值
    all_keys = Counter()
    for s in skills:
        for k in s['keys']:
            all_keys[k] += 1

    print("\nMost common skill keys:")
    for key, count in all_keys.most_common(20):
        name = stringtable.get_string(key) if stringtable else None
        name_str = name if name else f"0x{key:04X}"
        pct = count / len(skills) * 100
        print(f"  {name_str:30}: {count:5} ({pct:.1f}%)")

def analyze_equipment_content():
    """深入分析装备文件内容"""
    print("\n" + "=" * 60)
    print("Equipment Content Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    equips = []
    equip_types = Counter()

    for entry in pvf.indexes:
        if not entry.name.endswith('.equ'):
            continue
        data = pvf.extract_file(entry)
        if not data:
            continue

        try:
            container = parse_pvf_data(data)

            # 提取装备类型（通过目录判断）
            parts = entry.name.split('/')
            if len(parts) >= 2:
                equip_types[parts[1]] += 1

            equip_info = {
                'path': entry.name,
                'entries': len(container.entries),
                'keys': set(e.key for e in container.entries),
            }
            equips.append(equip_info)
        except:
            pass

    pvf.close()

    print(f"Total equipment: {len(equips)}")

    print("\nEquipment by type:")
    for etype, count in equip_types.most_common(20):
        print(f"  {etype:20}: {count:,}")

def analyze_monster_content():
    """深入分析怪物文件内容"""
    print("\n" + "=" * 60)
    print("Monster Content Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    monsters = []
    monster_dirs = Counter()

    for entry in pvf.indexes:
        if not entry.name.endswith('.mob'):
            continue
        data = pvf.extract_file(entry)
        if not data:
            continue

        try:
            container = parse_pvf_data(data)

            parts = entry.name.split('/')
            if len(parts) >= 2:
                monster_dirs[parts[1]] += 1

            monsters.append({
                'path': entry.name,
                'entries': len(container.entries),
            })
        except:
            pass

    pvf.close()

    print(f"Total monsters: {len(monsters)}")

    print("\nMonster directories:")
    for mdir, count in monster_dirs.most_common(15):
        print(f"  {mdir:30}: {count}")

def analyze_animation_content():
    """分析动画文件内容"""
    print("\n" + "=" * 60)
    print("Animation Content Analysis")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    ani_types = Counter()
    ani_dirs = Counter()

    sample = 0
    for entry in pvf.indexes:
        if not entry.name.endswith('.ani'):
            continue

        # 统计目录
        parts = entry.name.split('/')
        if len(parts) >= 2:
            ani_dirs[parts[1]] += 1

        data = pvf.extract_file(entry)
        if data and len(data) >= 2:
            # 检查类型
            if data[0] == 0xB0 and data[1] == 0xD0:
                ani_types['pvf_binary'] += 1
            else:
                type_code = data[0] | (data[1] << 8)
                ani_types[f'type_{type_code:04X}'] += 1

        sample += 1
        if sample >= 50000:
            break

    pvf.close()

    print(f"Sampled {sample} animation files")

    print("\nAnimation format types:")
    for atype, count in ani_types.most_common(15):
        print(f"  {atype:20}: {count:,}")

    print("\nAnimation directories:")
    for adir, count in ani_dirs.most_common(15):
        print(f"  {adir:30}: {count:,}")

def main():
    print("=" * 60)
    print("PVF Content Understanding Analysis")
    print("=" * 60)

    # 运行各项分析
    key_names = analyze_key_usage()
    analyze_references()
    analyze_skill_content()
    analyze_equipment_content()
    analyze_monster_content()
    analyze_animation_content()

    print("\n" + "=" * 60)
    print("Analysis Complete")
    print("=" * 60)

if __name__ == "__main__":
    main()
