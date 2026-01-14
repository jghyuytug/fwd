#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF引用验证器
=============
严谨验证引用解析的正确性
"""

import sys
import os
from collections import Counter

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType


def verify_reference_resolution():
    """验证引用解析正确性"""
    print("=" * 70)
    print("PVF Reference Resolution Verification")
    print("=" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    total_files = len(pvf.indexes)
    print(f"Total files in PVF: {total_files}")

    # 统计
    total_refs = 0
    valid_refs = 0
    invalid_refs = 0
    out_of_range = 0

    invalid_samples = []
    ref_target_ext = Counter()

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
                    total_refs += 1
                    ref_val = e.value

                    if 0 <= ref_val < total_files:
                        valid_refs += 1
                        target_path = pvf.indexes[ref_val].name
                        target_ext = os.path.splitext(target_path)[1].lower()
                        ref_target_ext[target_ext] += 1
                    else:
                        invalid_refs += 1
                        if ref_val >= total_files:
                            out_of_range += 1
                        if len(invalid_samples) < 10:
                            invalid_samples.append((entry.name, e.key, ref_val))
        except:
            pass

        sample += 1
        if sample >= 20000:
            break

    pvf.close()

    # 输出结果
    print(f"\nAnalyzed {sample} files")
    print(f"\n--- Reference Statistics ---")
    print(f"Total references found: {total_refs:,}")
    print(f"Valid (resolvable): {valid_refs:,} ({valid_refs/total_refs*100:.2f}%)")
    print(f"Invalid: {invalid_refs:,} ({invalid_refs/total_refs*100:.2f}%)")
    print(f"  - Out of range: {out_of_range}")

    if invalid_samples:
        print(f"\nInvalid reference samples:")
        for src, key, val in invalid_samples:
            print(f"  {src.split('/')[-1]}: key=0x{key:04X}, ref={val}")

    print(f"\n--- Referenced File Types ---")
    for ext, count in ref_target_ext.most_common(15):
        print(f"  {ext or '(none)':8}: {count:>8,}")

    # 验证双向一致性
    print("\n--- Bidirectional Consistency Check ---")
    verify_bidirectional('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')


def verify_bidirectional(pvf_path='C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf'):
    """验证引用的双向一致性"""
    pvf = PVFReader(pvf_path)
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 采样验证：选择一些被高频引用的文件，检查引用它们的文件
    # 选择索引16852（equipment/character/fighter/growtype/at_striker_ba...）

    test_indices = [16852, 13832, 23437, 559, 16776]

    for target_idx in test_indices:
        if target_idx >= len(pvf.indexes):
            continue

        target_path = pvf.indexes[target_idx].name
        print(f"\nTarget: index={target_idx}, path={target_path[:50]}...")

        # 找引用它的文件
        referrers = []
        for i, entry in enumerate(pvf.indexes[:10000]):
            data = pvf.extract_file(entry)
            if not data or len(data) < 2:
                continue
            if data[0] != 0xB0 or data[1] != 0xD0:
                continue

            try:
                container = parse_pvf_data(data)
                for e in container.entries:
                    if e.type == PVFValueType.REFERENCE and e.value == target_idx:
                        referrers.append(entry.name)
                        break
            except:
                pass

        print(f"  Found {len(referrers)} files referencing this target")
        for r in referrers[:3]:
            print(f"    <- {r.split('/')[-1][:50]}")

    pvf.close()


if __name__ == "__main__":
    verify_reference_resolution()
    print("\n" + "=" * 70)
    print("Verification Complete")
    print("=" * 70)
