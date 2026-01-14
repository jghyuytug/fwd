#!/usr/bin/env python3
"""快速键值语义分析"""

import sys
import os
from collections import Counter

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data
from pvf_stringtable import PVFStringTable

# 加载StringTable
st = PVFStringTable()
st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')
print(f"StringTable: {st.count} entries")

# 加载PVF
pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

# 快速统计前5000个PVF文件的键使用
keys = Counter()
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
            keys[e.key] += 1
    except:
        pass
    sample += 1
    if sample >= 5000:
        break

pvf.close()

print(f"\nSampled {sample} files, found {len(keys)} unique keys")
print("\n" + "=" * 70)
print("Top 50 Keys with Names")
print("=" * 70)

for key, count in keys.most_common(50):
    tag = st.get_tag(key)
    if tag:
        tag = tag.encode('ascii', errors='replace').decode('ascii')[:40]
    else:
        tag = f"(no name)"
    print(f"  0x{key:04X}  {tag:42} {count:>8,}")

# 输出常用游戏属性键
print("\n" + "=" * 70)
print("Known Game Attribute Keys")
print("=" * 70)

known_keys = [
    (0x2A12, "name"),
    (0x4817, "grade"),
    (0x066C, "rarity"),
    (0x481C, "minimum level"),
    (0x481D, "price"),
    (0x1265, "physical attack"),
    (0x1D0B, "physical defense"),
    (0x1D0C, "magical attack"),
    (0x1D0D, "magical defense"),
    (0x0B69, "HP"),
    (0x0B6A, "MP"),
    (0x0B2C, "attack speed"),
    (0x4822, "icon"),
    (0x4826, "equipment type"),
    (0x62D3, "durability"),
    (0x2280, "skill id"),
    (0x0500, "required sp"),
    (0x1A6D, "monster id"),
    (0x0B68, "monster attack"),
    (0x1A7C, "move speed"),
]

for key, desc in known_keys:
    count = keys.get(key, 0)
    tag = st.get_tag(key)
    if tag:
        tag = tag.encode('ascii', errors='replace').decode('ascii')[:30]
    else:
        tag = "(not in stringtable)"
    print(f"  0x{key:04X}  {desc:20}  {tag:32} {count:>6}")

print("\nDone!")
