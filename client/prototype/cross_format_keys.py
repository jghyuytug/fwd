#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
跨格式键分析
============
找出在多种格式中都使用的键，这些键通常有通用语义
"""

import sys
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Set

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


def analyze_keys_by_format(pvf: PVFReader, formats: list, sample_per_format: int = 300):
    """分析每种格式的键使用情况"""
    format_keys = defaultdict(lambda: defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'int_range': [float('inf'), float('-inf')],
        'sample_values': []
    }))

    for ext in formats:
        count = 0
        for entry in pvf.indexes:
            if not entry.name.endswith(ext):
                continue

            data = pvf.extract_file(entry)
            if not data or len(data) < 4:
                continue
            if data[0] != 0xB0 or data[1] != 0xD0:
                continue

            try:
                container = parse_pvf_data(data)
                for e in container.entries:
                    if e.key == 0:
                        continue

                    info = format_keys[ext][e.key]
                    info['count'] += 1
                    info['types'][e.type] += 1

                    if e.type == PVFValueType.INTEGER:
                        info['int_range'][0] = min(info['int_range'][0], e.value)
                        info['int_range'][1] = max(info['int_range'][1], e.value)
                        if len(info['sample_values']) < 5:
                            info['sample_values'].append(e.value)
            except:
                pass

            count += 1
            if count >= sample_per_format:
                break

    return format_keys


def find_cross_format_keys(format_keys: dict, min_formats: int = 2):
    """找出跨格式使用的键"""
    key_formats = defaultdict(list)

    for ext, keys in format_keys.items():
        for key in keys:
            key_formats[key].append(ext)

    # 过滤出在多个格式中出现的键
    cross_keys = {k: v for k, v in key_formats.items() if len(v) >= min_formats}

    return cross_keys


def infer_semantic(key: int, format_keys: dict, formats_used: list) -> str:
    """基于数据模式推断键的语义"""
    all_types = Counter()
    all_min = float('inf')
    all_max = float('-inf')
    all_values = []

    for ext in formats_used:
        if key in format_keys[ext]:
            info = format_keys[ext][key]
            for t, c in info['types'].items():
                all_types[t] += c

            if info['int_range'][0] != float('inf'):
                all_min = min(all_min, info['int_range'][0])
                all_max = max(all_max, info['int_range'][1])

            all_values.extend(info['sample_values'])

    # 确定主类型
    main_type = all_types.most_common(1)[0][0] if all_types else None

    if main_type == PVFValueType.REFERENCE:
        return "reference"
    elif main_type == PVFValueType.STRING:
        return "text_field"
    elif main_type == PVFValueType.FLOAT:
        if all_values:
            avg = sum(all_values) / len(all_values) if all_values else 0
            if 0 <= avg <= 1:
                return "ratio"
            elif avg <= 100:
                return "percentage"
        return "float_value"
    elif main_type == PVFValueType.INTEGER:
        if all_values:
            unique = len(set(all_values))
            if set(all_values) <= {0, 1}:
                return "boolean"
            elif unique <= 10 and all_max < 100:
                return "enum"
            elif 1 <= all_min and all_max <= 100:
                return "level_or_grade"
            elif all_max > 10000:
                return "large_value"
            elif all_max < 1000:
                return "small_value"
        return "integer"

    return "unknown"


def main():
    print("=" * 60)
    print("跨格式键分析")
    print("=" * 60)

    # 加载StringTable用于标签查询
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=100000)

    formats = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk', '.ai']

    print("\n分析各格式的键使用...")
    format_keys = analyze_keys_by_format(pvf, formats, sample_per_format=300)

    # 统计每种格式的键数量
    print("\n格式键数量统计:")
    for ext in formats:
        print(f"  {ext}: {len(format_keys[ext])} 个唯一键")

    # 找出跨格式键
    print("\n" + "-" * 60)
    print("跨格式键 (在2+格式中出现)")
    print("-" * 60)

    cross_keys = find_cross_format_keys(format_keys, min_formats=2)

    # 按出现格式数排序
    sorted_keys = sorted(cross_keys.items(), key=lambda x: -len(x[1]))

    print(f"\n共 {len(cross_keys)} 个跨格式键\n")
    print(f"{'Key':>8} {'格式数':>6} {'格式列表':40} {'推断语义'}")
    print("-" * 80)

    new_mappings = {}

    for key, formats_used in sorted_keys[:50]:
        tag = st.get_tag(key)
        tag_str = ""
        if tag:
            tag_str = tag.encode('ascii', errors='replace').decode('ascii')[:15]

        formats_str = ','.join(formats_used)
        semantic = infer_semantic(key, format_keys, formats_used)

        print(f"0x{key:04X}  {len(formats_used):>6}  {formats_str:40} {semantic}")

        # 收集新映射
        if semantic != "unknown":
            new_mappings[key] = {
                'semantic': semantic,
                'formats': formats_used,
                'tag': tag_str
            }

    pvf.close()

    # 输出可添加的新映射
    print("\n" + "=" * 60)
    print("建议添加的新键映射")
    print("=" * 60)

    print("\nCROSS_FORMAT_KEYS = {")
    for key in sorted(new_mappings.keys()):
        info = new_mappings[key]
        formats_str = ','.join(info['formats'])
        print(f"    0x{key:04X}: \"{info['semantic']}\",  # [{formats_str}]")
    print("}")


if __name__ == "__main__":
    main()
