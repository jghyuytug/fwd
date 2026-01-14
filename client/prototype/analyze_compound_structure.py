#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
复合数据结构分析
================
分析PVF文件中的ARRAY(0x0A)、NESTED(0x0B)和嵌套KEY_VALUE(0x05->0x05)结构
"""

import sys
import os
import struct
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader


def scan_type_bytes(data: bytes, offset: int = 2) -> Counter:
    """扫描数据中所有可能的类型字节分布"""
    type_counts = Counter()
    i = offset  # 跳过魔数

    while i < len(data):
        byte = data[i]
        type_counts[byte] += 1
        i += 1

    return type_counts


def find_array_patterns(data: bytes) -> list:
    """查找可能的ARRAY(0x0A)模式"""
    patterns = []
    i = 0

    while i < len(data) - 6:
        if data[i] == 0x0A:
            # 可能的ARRAY结构: 0x0A [element_type:1] [count:4]
            if i + 5 < len(data):
                elem_type = data[i + 1]
                count = struct.unpack_from('<I', data, i + 2)[0]

                # 合理性检查
                if elem_type in [0x02, 0x04, 0x07] and 0 < count < 1000:
                    patterns.append({
                        'offset': i,
                        'elem_type': elem_type,
                        'count': count,
                        'context': data[max(0, i-4):i+10].hex()
                    })
        i += 1

    return patterns


def find_nested_patterns(data: bytes) -> list:
    """查找可能的NESTED(0x0B)模式"""
    patterns = []
    i = 0

    while i < len(data) - 5:
        if data[i] == 0x0B:
            # 可能的NESTED结构: 0x0B [field_count:4]
            if i + 4 < len(data):
                field_count = struct.unpack_from('<I', data, i + 1)[0]

                # 合理性检查
                if 0 < field_count < 100:
                    patterns.append({
                        'offset': i,
                        'field_count': field_count,
                        'context': data[max(0, i-4):i+10].hex()
                    })
        i += 1

    return patterns


def find_nested_keyvalue(data: bytes) -> list:
    """查找嵌套KEY_VALUE (0x05内部再出现0x05)"""
    patterns = []
    i = 2 if len(data) >= 2 and data[0] == 0xB0 and data[1] == 0xD0 else 0

    while i < len(data) - 8:
        if data[i] == 0x05:
            # KEY_VALUE: 05 [key:2] [extra:2] [inner_type:1] [value:...]
            if i + 5 < len(data):
                key = struct.unpack_from('<H', data, i + 1)[0]
                extra = struct.unpack_from('<H', data, i + 3)[0]
                inner_type = data[i + 5]

                if inner_type == 0x05:
                    # 嵌套KEY_VALUE!
                    patterns.append({
                        'offset': i,
                        'outer_key': key,
                        'extra': extra,
                        'context': data[i:i+12].hex()
                    })
        i += 1

    return patterns


def analyze_format_compound_usage(pvf: PVFReader, ext: str, sample_size: int = 500):
    """分析特定格式的复合结构使用"""
    print(f"\n分析 {ext} 格式:")
    print("-" * 50)

    array_files = []
    nested_files = []
    nested_kv_files = []
    type_distribution = Counter()

    count = 0
    for entry in pvf.indexes:
        if not entry.name.endswith(ext):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue

        # 只分析PVF格式
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        count += 1
        if count > sample_size:
            break

        # 扫描类型字节
        types = scan_type_bytes(data)
        for t, c in types.items():
            type_distribution[t] += c

        # 查找复合结构
        arrays = find_array_patterns(data)
        if arrays:
            array_files.append((entry.name, arrays[:3]))

        nested = find_nested_patterns(data)
        if nested:
            nested_files.append((entry.name, nested[:3]))

        nested_kv = find_nested_keyvalue(data)
        if nested_kv:
            nested_kv_files.append((entry.name, nested_kv[:3]))

    print(f"  采样: {count} 个文件")

    # 类型分布
    print(f"\n  类型字节分布 (前10):")
    for byte, cnt in type_distribution.most_common(10):
        type_name = {
            0x02: "INT",
            0x04: "FLOAT",
            0x05: "KEY_VALUE",
            0x07: "REF",
            0x09: "STRING",
            0x0A: "ARRAY?",
            0x0B: "NESTED?",
        }.get(byte, f"0x{byte:02X}")
        print(f"    {type_name:12} (0x{byte:02X}): {cnt:>7}")

    # 复合结构发现
    print(f"\n  可能的ARRAY(0x0A)文件: {len(array_files)}")
    for fname, patterns in array_files[:3]:
        print(f"    {fname.split('/')[-1][:40]}")
        for p in patterns[:1]:
            print(f"      offset={p['offset']}, type={p['elem_type']}, count={p['count']}")

    print(f"\n  可能的NESTED(0x0B)文件: {len(nested_files)}")
    for fname, patterns in nested_files[:3]:
        print(f"    {fname.split('/')[-1][:40]}")
        for p in patterns[:1]:
            print(f"      offset={p['offset']}, fields={p['field_count']}")

    print(f"\n  嵌套KEY_VALUE(05->05)文件: {len(nested_kv_files)}")
    for fname, patterns in nested_kv_files[:3]:
        print(f"    {fname.split('/')[-1][:40]}")
        for p in patterns[:1]:
            print(f"      offset={p['offset']}, key=0x{p['outer_key']:04X}")

    return {
        'array_count': len(array_files),
        'nested_count': len(nested_files),
        'nested_kv_count': len(nested_kv_files),
        'array_samples': array_files[:5],
        'nested_samples': nested_files[:5],
        'nested_kv_samples': nested_kv_files[:5],
    }


def main():
    print("=" * 60)
    print("PVF复合数据结构分析")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=100000)

    formats = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk', '.ai']
    all_results = {}

    for ext in formats:
        result = analyze_format_compound_usage(pvf, ext, sample_size=500)
        all_results[ext] = result

    pvf.close()

    # 总结
    print("\n" + "=" * 60)
    print("总结")
    print("=" * 60)

    print("\n复合结构发现:")
    print(f"{'格式':8} {'ARRAY':>8} {'NESTED':>8} {'嵌套KV':>8}")
    print("-" * 40)

    total_array = 0
    total_nested = 0
    total_kv = 0

    for ext, r in all_results.items():
        print(f"{ext:8} {r['array_count']:>8} {r['nested_count']:>8} {r['nested_kv_count']:>8}")
        total_array += r['array_count']
        total_nested += r['nested_count']
        total_kv += r['nested_kv_count']

    print("-" * 40)
    print(f"{'总计':8} {total_array:>8} {total_nested:>8} {total_kv:>8}")

    # 结论
    print("\n" + "=" * 60)
    print("结论")
    print("=" * 60)

    if total_array == 0 and total_nested == 0:
        print("""
发现: 在已分析的PVF文件中，没有发现明确的ARRAY(0x0A)或NESTED(0x0B)结构。

可能的解释:
1. 这些类型码在实际数据中很少使用
2. 复合数据以其他形式存在（如多个同键的KEY_VALUE）
3. 0x0A字节是STRING格式的一部分(长度后的标记)

建议:
- 嵌套KEY_VALUE(05->05)是主要的复合数据形式
- 不需要实现ARRAY/NESTED解析器
- 专注于扩展现有KEY_VALUE解析的深度
""")
    else:
        print(f"""
发现: 存在复合数据结构
- ARRAY文件: {total_array}
- NESTED文件: {total_nested}
- 嵌套KEY_VALUE: {total_kv}

需要实现完整的复合结构解析。
""")


if __name__ == "__main__":
    main()
