#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Phase 2 格式分析脚本
分析 STK/QST/AI 文件的键值结构
"""

import sys
import os
from collections import defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType

def analyze_format(pvf, extension, max_files=100):
    """分析特定扩展名的文件格式"""
    print(f"\n{'='*60}")
    print(f"Analyzing {extension} files")
    print('='*60)

    key_stats = defaultdict(lambda: {'count': 0, 'types': set(), 'values': []})
    file_count = 0
    size_range = [float('inf'), 0]
    entry_range = [float('inf'), 0]

    for entry in pvf.indexes:
        if entry.name.endswith(extension):
            data = pvf.extract_file(entry)
            if data and len(data) >= 2:
                # 检查魔数
                if data[0] != 0xB0 or data[1] != 0xD0:
                    continue

                size_range[0] = min(size_range[0], len(data))
                size_range[1] = max(size_range[1], len(data))

                container = parse_pvf_data(data)
                entry_range[0] = min(entry_range[0], len(container.entries))
                entry_range[1] = max(entry_range[1], len(container.entries))

                for e in container.entries:
                    if e.key > 0:
                        key_stats[e.key]['count'] += 1
                        key_stats[e.key]['types'].add(e.type.name)
                        if len(key_stats[e.key]['values']) < 5:
                            key_stats[e.key]['values'].append(e.value)

                file_count += 1
                if file_count >= max_files:
                    break

    print(f"\nAnalyzed {file_count} files")
    print(f"Size range: {size_range[0]} - {size_range[1]} bytes")
    print(f"Entry range: {entry_range[0]} - {entry_range[1]} entries")

    # 按出现频率排序
    sorted_keys = sorted(key_stats.items(), key=lambda x: -x[1]['count'])

    print(f"\nTop 30 keys (by frequency):")
    print("-" * 70)
    print(f"{'Key (hex)':<12} {'Count':<8} {'Types':<20} {'Sample Values'}")
    print("-" * 70)

    for key, stats in sorted_keys[:30]:
        types = ','.join(stats['types'])
        # 安全处理值，避免编码问题
        try:
            samples = repr(stats['values'][:3])[:40]
            samples = samples.encode('ascii', 'replace').decode('ascii')
        except:
            samples = "[...]"
        print(f"0x{key:04X}       {stats['count']:<8} {types:<20} {samples}")

    return sorted_keys

if __name__ == "__main__":
    print("Phase 5 Format Analysis - Remaining Formats")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=300000)

    # 分析剩余格式
    formats = ['.als', '.etc', '.aic', '.rep', '.ui', '.nut', '.npc', '.cre', '.msn']
    for fmt in formats:
        analyze_format(pvf, fmt, 100)

    pvf.close()
    print("\n" + "=" * 60)
    print("Analysis Complete!")
