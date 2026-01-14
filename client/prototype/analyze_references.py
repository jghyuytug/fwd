#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF引用关系分析器
=================
严谨分析文件间的引用机制
"""

import sys
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Set, Tuple, Optional

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


@dataclass
class ReferenceInfo:
    """引用信息"""
    source_file: str
    source_key: int
    ref_value: int
    ref_type: str  # 'index', 'stringtable', 'unknown'
    resolved_path: str = ""


def analyze_reference_mechanism():
    """分析引用机制"""
    print("=" * 70)
    print("PVF Reference Mechanism Analysis")
    print("=" * 70)

    # 加载StringTable
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')
    print(f"StringTable: {st.count} entries")

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)
    print(f"PVF Files: {len(pvf.indexes)}")

    # 构建文件索引映射
    file_index_map = {i: e.name for i, e in enumerate(pvf.indexes)}
    file_name_to_index = {e.name.lower(): i for i, e in enumerate(pvf.indexes)}

    # 收集所有REFERENCE类型的值
    ref_values = Counter()
    ref_by_key = defaultdict(Counter)
    ref_samples = defaultdict(list)

    sample_count = 0
    for entry in pvf.indexes:
        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.type == PVFValueType.REFERENCE and e.key > 0:
                    ref_values[e.value] += 1
                    ref_by_key[e.key][e.value] += 1
                    if len(ref_samples[(e.key, e.value)]) < 2:
                        ref_samples[(e.key, e.value)].append(entry.name)
        except:
            pass

        sample_count += 1
        if sample_count >= 10000:
            break

    pvf.close()

    print(f"\nAnalyzed {sample_count} files")
    print(f"Unique reference values: {len(ref_values)}")

    # 分析引用值的范围
    print("\n" + "-" * 70)
    print("Reference Value Range Analysis")
    print("-" * 70)

    all_refs = list(ref_values.keys())
    if all_refs:
        min_ref = min(all_refs)
        max_ref = max(all_refs)
        print(f"Min reference value: {min_ref} (0x{min_ref:04X})")
        print(f"Max reference value: {max_ref} (0x{max_ref:04X})")
        print(f"PVF file count: {len(pvf.indexes)}")
        print(f"StringTable count: {st.count}")

        # 判断引用类型
        if max_ref < len(pvf.indexes):
            print("\n=> References likely point to PVF file indices")
        elif max_ref < st.count:
            print("\n=> References likely point to StringTable indices")
        else:
            print("\n=> References use unknown mechanism")

    # 尝试解析引用
    print("\n" + "-" * 70)
    print("Reference Resolution Test")
    print("-" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    resolved_as_index = 0
    resolved_as_stringtable = 0
    unresolved = 0

    test_refs = list(ref_values.most_common(100))
    for ref_val, count in test_refs:
        # 尝试作为文件索引
        if 0 <= ref_val < len(pvf.indexes):
            resolved_path = pvf.indexes[ref_val].name
            resolved_as_index += 1
            if count > 50:  # 只显示高频引用
                print(f"  REF {ref_val:>6} (count={count:>5}) -> INDEX -> {resolved_path[:50]}")
        # 尝试作为StringTable索引
        elif ref_val < st.count:
            st_string = st.get_string(ref_val)
            if st_string:
                resolved_as_stringtable += 1
                if count > 50:
                    st_safe = st_string.encode('ascii', 'replace').decode('ascii')[:40]
                    print(f"  REF {ref_val:>6} (count={count:>5}) -> STRTBL -> {st_safe}")
            else:
                unresolved += 1
        else:
            unresolved += 1

    pvf.close()

    print(f"\nResolution summary (top 100 refs):")
    print(f"  Resolved as file index: {resolved_as_index}")
    print(f"  Resolved as StringTable: {resolved_as_stringtable}")
    print(f"  Unresolved: {unresolved}")

    # 分析引用键的语义
    print("\n" + "-" * 70)
    print("Reference Keys Analysis (which keys contain references)")
    print("-" * 70)

    for key in sorted(ref_by_key.keys(), key=lambda k: -sum(ref_by_key[k].values()))[:20]:
        total = sum(ref_by_key[key].values())
        unique = len(ref_by_key[key])
        tag = st.get_tag(key)
        if tag:
            tag = tag.encode('ascii', 'replace').decode('ascii')[:30]
        else:
            tag = f"0x{key:04X}"
        print(f"  Key {tag:32}: {total:>6} refs, {unique:>5} unique values")


def analyze_file_dependencies():
    """分析文件依赖关系"""
    print("\n" + "=" * 70)
    print("File Dependency Analysis")
    print("=" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 收集依赖关系
    dependencies = defaultdict(set)  # file -> set of referenced files
    reverse_deps = defaultdict(set)   # file -> set of files that reference it

    sample = 0
    for entry in pvf.indexes:
        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.type == PVFValueType.REFERENCE:
                    ref_val = e.value
                    if 0 <= ref_val < len(pvf.indexes):
                        ref_file = pvf.indexes[ref_val].name
                        dependencies[entry.name].add(ref_file)
                        reverse_deps[ref_file].add(entry.name)
        except:
            pass

        sample += 1
        if sample >= 5000:
            break

    pvf.close()

    print(f"\nAnalyzed {sample} files")
    print(f"Files with outgoing references: {len(dependencies)}")
    print(f"Files with incoming references: {len(reverse_deps)}")

    # 最常被引用的文件
    print("\n" + "-" * 70)
    print("Most Referenced Files (Top 20)")
    print("-" * 70)

    for path, refs in sorted(reverse_deps.items(), key=lambda x: -len(x[1]))[:20]:
        print(f"  {len(refs):>5} refs <- {path[:55]}")

    # 引用最多的文件
    print("\n" + "-" * 70)
    print("Files with Most References (Top 20)")
    print("-" * 70)

    for path, deps in sorted(dependencies.items(), key=lambda x: -len(x[1]))[:20]:
        print(f"  {len(deps):>5} deps -> {path[:55]}")

    # 按文件类型统计依赖
    print("\n" + "-" * 70)
    print("Dependencies by File Type")
    print("-" * 70)

    ext_deps = defaultdict(lambda: defaultdict(int))
    for src, dsts in dependencies.items():
        src_ext = os.path.splitext(src)[1].lower()
        for dst in dsts:
            dst_ext = os.path.splitext(dst)[1].lower()
            ext_deps[src_ext][dst_ext] += 1

    for src_ext in sorted(ext_deps.keys()):
        targets = ext_deps[src_ext]
        total = sum(targets.values())
        top_targets = sorted(targets.items(), key=lambda x: -x[1])[:5]
        targets_str = ", ".join(f"{ext}({cnt})" for ext, cnt in top_targets)
        print(f"  {src_ext:6} ({total:>5} refs) -> {targets_str}")


if __name__ == "__main__":
    analyze_reference_mechanism()
    analyze_file_dependencies()
    print("\n" + "=" * 70)
    print("Analysis Complete")
    print("=" * 70)
