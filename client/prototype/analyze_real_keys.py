#!/usr/bin/env python3
"""分析真正有语义的键（排除key=0）"""

import sys
import os
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable, KNOWN_TAGS

# 加载StringTable
st = PVFStringTable()
st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

def analyze_format(ext, limit=1000):
    """分析特定格式的真实键分布"""
    keys = Counter()
    key_values = defaultdict(list)

    count = 0
    for entry in pvf.indexes:
        if not entry.name.endswith(ext):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 2:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                # 只统计有实际键的条目（key > 0）
                if e.key > 0:
                    keys[e.key] += 1
                    if len(key_values[e.key]) < 3:
                        key_values[e.key].append(e.value)
        except:
            pass

        count += 1
        if count >= limit:
            break

    return keys, key_values, count

# 分析各格式
formats = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk']

for ext in formats:
    keys, values, count = analyze_format(ext, limit=500)

    print("=" * 70)
    print(f"{ext} - Analyzed {count} files, {len(keys)} unique keys")
    print("=" * 70)

    for key, cnt in keys.most_common(25):
        tag = st.get_tag(key)
        if tag:
            tag = tag.encode('ascii', errors='replace').decode('ascii')[:35]
        else:
            tag = "(no tag)"

        # 采样值
        samples = values[key][:2]
        sample_str = str(samples)[:40]
        sample_str = sample_str.encode('ascii', errors='replace').decode('ascii')

        print(f"  0x{key:04X}  {tag:37} {cnt:>5}  {sample_str}")

    print()

pvf.close()

# 输出已知的游戏属性键
print("=" * 70)
print("Known Game Attribute Keys (from pvf_stringtable.py)")
print("=" * 70)

for key, name in sorted(KNOWN_TAGS.items()):
    print(f"  0x{key:04X} = {name}")
