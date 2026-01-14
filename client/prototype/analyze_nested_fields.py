#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
嵌套结构内部字段分析
====================
分析嵌套KEY_VALUE(05->05)结构内部包含的数据
"""

import sys
import os
import struct
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Tuple, Optional

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


# 已知的嵌套结构键
NESTED_KEYS = {
    '.equ': [0x4183],
    '.mob': [0x2BA4, 0x299E],
    '.skl': [0x0500],
    '.stk': [0x4183],
    '.qst': [0x82EC],
    '.act': [0x02ED],
    '.atk': [0x1A5C],
    '.ai': [0x01E0],
}


def parse_nested_content(data: bytes, offset: int) -> List[Dict]:
    """
    解析嵌套KEY_VALUE之后的内容
    格式: 05 [key:2] [extra:2] 05 [sub_key:2] ...
    """
    results = []
    pos = offset

    while pos < len(data) - 6:
        if data[pos] != 0x05:
            break

        # 读取外层KEY_VALUE
        outer_key = struct.unpack_from('<H', data, pos + 1)[0]
        extra = struct.unpack_from('<H', data, pos + 3)[0]
        inner_type = data[pos + 5]

        entry = {
            'offset': pos,
            'outer_key': outer_key,
            'extra': extra,
            'inner_type': inner_type,
            'inner_value': None,
        }

        pos += 6

        # 根据内部类型解析值
        if inner_type == 0x02:  # INTEGER
            if pos + 4 <= len(data):
                entry['inner_value'] = struct.unpack_from('<i', data, pos)[0]
                pos += 4
        elif inner_type == 0x04:  # FLOAT
            if pos + 4 <= len(data):
                entry['inner_value'] = struct.unpack_from('<f', data, pos)[0]
                pos += 4
        elif inner_type == 0x07:  # REFERENCE
            if pos + 2 <= len(data):
                entry['inner_value'] = struct.unpack_from('<H', data, pos)[0]
                pos += 2
        elif inner_type == 0x05:  # NESTED KEY_VALUE
            if pos + 2 <= len(data):
                entry['inner_value'] = struct.unpack_from('<H', data, pos)[0]
                pos += 2
        elif inner_type == 0x09:  # STRING
            if pos + 5 <= len(data):
                str_len = struct.unpack_from('<I', data, pos)[0]
                marker = data[pos + 4]
                if marker == 0x0a and str_len > 0 and pos + 5 + str_len <= len(data):
                    entry['inner_value'] = data[pos + 5:pos + 5 + str_len].decode('utf-8', errors='replace')
                    pos += 5 + str_len
                else:
                    pos += 5
        else:
            # 未知类型，尝试读取4字节
            if pos + 4 <= len(data):
                entry['inner_value'] = struct.unpack_from('<I', data, pos)[0]
                pos += 4

        results.append(entry)

        # 限制解析数量
        if len(results) >= 50:
            break

    return results


def find_nested_structure(data: bytes, nested_key: int) -> List[int]:
    """查找嵌套结构的位置"""
    positions = []
    i = 2 if len(data) >= 2 and data[0] == 0xB0 and data[1] == 0xD0 else 0

    while i < len(data) - 8:
        if data[i] == 0x05:
            key = struct.unpack_from('<H', data, i + 1)[0]
            if key == nested_key:
                inner_type = data[i + 5]
                if inner_type == 0x05:  # 嵌套KEY_VALUE
                    positions.append(i)
        i += 1

    return positions


def analyze_nested_fields_by_format(pvf: PVFReader, ext: str, nested_keys: List[int], sample_size: int = 100):
    """分析特定格式的嵌套结构内部字段"""
    print(f"\n{'='*60}")
    print(f"分析 {ext} 格式的嵌套结构")
    print(f"嵌套键: {[f'0x{k:04X}' for k in nested_keys]}")
    print('='*60)

    # 收集所有嵌套内容
    all_inner_keys = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'values': [],
    })

    file_count = 0
    nested_count = 0

    for entry in pvf.indexes:
        if not entry.name.endswith(ext):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        file_count += 1
        if file_count > sample_size:
            break

        # 查找每个嵌套键
        for nested_key in nested_keys:
            positions = find_nested_structure(data, nested_key)
            if not positions:
                continue

            nested_count += len(positions)

            # 解析嵌套内容
            for pos in positions[:3]:  # 每个文件最多解析3个
                # 跳过外层KEY_VALUE头部(6字节)到内部数据
                inner_pos = pos + 6 + 2  # +2 for the sub_key
                nested_entries = parse_nested_content(data, inner_pos)

                for ne in nested_entries:
                    key = ne['outer_key']
                    info = all_inner_keys[key]
                    info['count'] += 1

                    type_name = {
                        0x02: "INT",
                        0x04: "FLOAT",
                        0x05: "NESTED",
                        0x07: "REF",
                        0x09: "STRING",
                    }.get(ne['inner_type'], f"0x{ne['inner_type']:02X}")

                    info['types'][type_name] += 1

                    if ne['inner_value'] is not None and len(info['values']) < 10:
                        info['values'].append(ne['inner_value'])

    print(f"\n采样: {file_count} 个文件, {nested_count} 个嵌套结构")

    if not all_inner_keys:
        print("未发现嵌套内部字段")
        return {}

    # 输出分析结果
    print(f"\n发现 {len(all_inner_keys)} 个内部字段键:")
    print(f"\n{'Key':>8} {'Count':>6} {'Type':>8} {'Sample Values'}")
    print('-'*60)

    sorted_keys = sorted(all_inner_keys.items(), key=lambda x: -x[1]['count'])

    for key, info in sorted_keys[:30]:
        main_type = info['types'].most_common(1)[0][0] if info['types'] else "?"
        values_str = str(info['values'][:3])[:30] if info['values'] else ""
        print(f"0x{key:04X}  {info['count']:>6}  {main_type:>8}  {values_str}")

    return all_inner_keys


def analyze_specific_nested_key(pvf: PVFReader, ext: str, nested_key: int, sample_count: int = 5):
    """详细分析特定嵌套键的完整结构"""
    print(f"\n{'='*60}")
    print(f"详细分析嵌套键 0x{nested_key:04X} ({ext})")
    print('='*60)

    samples = []

    for entry in pvf.indexes:
        if len(samples) >= sample_count:
            break

        if not entry.name.endswith(ext):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        positions = find_nested_structure(data, nested_key)
        if not positions:
            continue

        for pos in positions[:1]:
            # 提取嵌套结构的原始数据
            end_pos = min(pos + 100, len(data))
            raw_hex = data[pos:end_pos].hex()

            samples.append({
                'file': entry.name.split('/')[-1],
                'offset': pos,
                'hex': raw_hex,
                'parsed': parse_nested_content(data, pos),
            })

    # 输出样本
    for i, s in enumerate(samples, 1):
        print(f"\n[{i}] {s['file']}")
        print(f"    Offset: {s['offset']}")
        print(f"    Hex (前50字节): {s['hex'][:100]}")
        print(f"    解析结果:")
        for j, p in enumerate(s['parsed'][:10]):
            type_name = {0x02:"INT", 0x04:"FLOAT", 0x05:"NESTED", 0x07:"REF", 0x09:"STR"}.get(p['inner_type'], "?")
            print(f"      [{j}] key=0x{p['outer_key']:04X} type={type_name} val={p['inner_value']}")


def main():
    print("=" * 60)
    print("嵌套结构内部字段分析")
    print("=" * 60)

    # 加载StringTable
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=100000)

    all_nested_fields = {}

    # 分析每种格式
    for ext, keys in NESTED_KEYS.items():
        result = analyze_nested_fields_by_format(pvf, ext, keys, sample_size=200)
        all_nested_fields[ext] = result

    # 详细分析几个关键嵌套键
    print("\n" + "=" * 60)
    print("详细样本分析")
    print("=" * 60)

    analyze_specific_nested_key(pvf, '.skl', 0x0500, sample_count=3)
    analyze_specific_nested_key(pvf, '.mob', 0x299E, sample_count=3)

    pvf.close()

    # 汇总跨格式的内部键
    print("\n" + "=" * 60)
    print("跨格式内部键汇总")
    print("=" * 60)

    inner_key_formats = defaultdict(list)
    for ext, fields in all_nested_fields.items():
        for key in fields.keys():
            inner_key_formats[key].append(ext)

    cross_format = {k: v for k, v in inner_key_formats.items() if len(v) >= 2}
    print(f"\n在2+格式中出现的内部键: {len(cross_format)}")

    for key in sorted(cross_format.keys())[:20]:
        formats = cross_format[key]
        tag = st.get_tag(key)
        tag_str = tag.encode('ascii', errors='replace').decode('ascii')[:20] if tag else ""
        print(f"  0x{key:04X}  {','.join(formats):30} {tag_str}")


if __name__ == "__main__":
    main()
