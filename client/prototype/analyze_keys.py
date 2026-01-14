#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF键值语义分析器
=================
建立键ID到语义名称的映射
"""

import sys
import os
from collections import defaultdict, Counter

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable

def load_stringtable():
    """加载StringTable"""
    paths = [
        'C:/Users/waw/Desktop/game/stringtable.bin',
        'stringtable.bin',
        '../stringtable.bin',
    ]

    for path in paths:
        try:
            st = PVFStringTable()
            if st.load_from_file(path):
                print(f"[StringTable] Loaded from {path}: {st.count} entries")
                return st
        except Exception as e:
            continue

    print("[StringTable] Failed to load from any path")
    return None

def analyze_all_keys():
    """分析所有键的使用情况和语义"""
    stringtable = load_stringtable()

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 全局键统计
    all_keys = Counter()
    key_by_type = defaultdict(Counter)  # 按值类型
    key_by_ext = defaultdict(Counter)   # 按文件扩展名
    key_values = defaultdict(list)      # 采样值

    sample = 0
    for entry in pvf.indexes:
        ext = os.path.splitext(entry.name)[1].lower()
        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                all_keys[e.key] += 1
                key_by_type[e.type][e.key] += 1
                key_by_ext[ext][e.key] += 1

                # 采样值
                if len(key_values[e.key]) < 5:
                    key_values[e.key].append((ext, e.type, e.value))
        except:
            pass

        sample += 1
        if sample >= 50000:
            break

    pvf.close()

    print(f"\nAnalyzed {sample} files")
    print(f"Unique keys found: {len(all_keys)}")

    # 输出Top 100键及其语义
    print("\n" + "=" * 80)
    print("Top 100 Keys with Semantic Names")
    print("=" * 80)
    print(f"{'Key':>8} {'Name':40} {'Count':>10} {'Type':>10}")
    print("-" * 80)

    for key, count in all_keys.most_common(100):
        name = "unknown"
        if stringtable:
            st_name = stringtable.get_tag(key)  # 使用get_tag获取[xxx]格式
            if st_name:
                # ASCII安全
                name = st_name.encode('ascii', errors='replace').decode('ascii')

        # 推断主要类型
        type_counts = key_by_type[PVFValueType.INTEGER].get(key, 0)
        ref_counts = key_by_type[PVFValueType.REFERENCE].get(key, 0)
        main_type = "INT" if type_counts > ref_counts else "REF" if ref_counts > 0 else "?"

        print(f"0x{key:04X}   {name:40} {count:>10,} {main_type:>10}")

    # 分类输出
    print("\n" + "=" * 80)
    print("Keys by File Type (Top 20 per type)")
    print("=" * 80)

    for ext in ['.skl', '.mob', '.equ', '.act', '.atk', '.stk', '.qst', '.ai']:
        if ext not in key_by_ext:
            continue
        print(f"\n{ext}:")
        for key, count in key_by_ext[ext].most_common(20):
            name = stringtable.get_tag(key) if stringtable else None
            if name:
                name = name.encode('ascii', errors='replace').decode('ascii')
            else:
                name = f"0x{key:04X}"

            # 获取采样值
            samples = [v for e, t, v in key_values[key] if e == ext][:3]
            sample_str = str(samples)[:50] if samples else ""
            sample_str = sample_str.encode('ascii', errors='replace').decode('ascii')

            print(f"  {name:35}: {count:>6} {sample_str}")

    # 输出键名映射表
    if stringtable:
        print("\n" + "=" * 80)
        print("Key Name Mapping (from StringTable)")
        print("=" * 80)

        mapped = 0
        for key in sorted(all_keys.keys()):
            name = stringtable.get_tag(key)
            if name and name != f"0x{key:04X}":
                mapped += 1
                if mapped <= 200:
                    name_safe = name.encode('ascii', errors='replace').decode('ascii')
                    print(f"  0x{key:04X} = {name_safe}")

        print(f"\nTotal mapped: {mapped} / {len(all_keys)}")

def build_semantic_mapping():
    """构建语义映射表"""
    stringtable = load_stringtable()
    if not stringtable:
        print("Cannot build semantic mapping without StringTable")
        return

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 收集所有键
    all_keys = set()
    for entry in pvf.indexes[:50000]:
        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue
        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                all_keys.add(e.key)
        except:
            pass

    pvf.close()

    # 输出映射
    print("\n" + "=" * 80)
    print("Semantic Mapping Dictionary")
    print("=" * 80)
    print("KEY_NAMES = {")

    for key in sorted(all_keys):
        name = stringtable.get_string(key)
        if name:
            # 清理名称
            clean_name = name.replace('"', '\\"')
            print(f"    0x{key:04X}: \"{clean_name}\",")

    print("}")

if __name__ == "__main__":
    analyze_all_keys()
    build_semantic_mapping()
