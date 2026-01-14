#!/usr/bin/env python3
"""深入分析装备文件内容"""

import sys
import os
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable

# 加载StringTable
st = PVFStringTable()
st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

# 加载PVF
pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

print("=" * 70)
print("Equipment (EQU) Content Analysis")
print("=" * 70)

# 分析装备文件
equ_keys = Counter()
equ_types = defaultdict(Counter)  # 按值类型分组
sample_values = defaultdict(list)  # 采样值

count = 0
for entry in pvf.indexes:
    if not entry.name.endswith('.equ'):
        continue

    data = pvf.extract_file(entry)
    if not data or len(data) < 2:
        continue
    if data[0] != 0xB0 or data[1] != 0xD0:
        continue

    try:
        container = parse_pvf_data(data)
        for e in container.entries:
            equ_keys[e.key] += 1
            equ_types[e.key][e.type] += 1
            if len(sample_values[e.key]) < 3:
                sample_values[e.key].append((entry.name.split('/')[-1], e.value))
    except:
        pass

    count += 1
    if count >= 2000:  # 分析前2000个装备
        break

pvf.close()

print(f"\nAnalyzed {count} equipment files")
print(f"Found {len(equ_keys)} unique keys")

# 输出Top键
print("\n" + "-" * 70)
print("Top 40 Equipment Keys")
print("-" * 70)
print(f"{'Key':>8}  {'StringTable Tag':42} {'Count':>8} {'Type':>8}")
print("-" * 70)

for key, cnt in equ_keys.most_common(40):
    tag = st.get_tag(key)
    if tag:
        tag = tag.encode('ascii', errors='replace').decode('ascii')[:40]
    else:
        tag = "(unknown)"

    # 主要值类型
    types = equ_types[key]
    main_type = max(types, key=types.get) if types else "?"
    type_name = {
        PVFValueType.INTEGER: "INT",
        PVFValueType.FLOAT: "FLOAT",
        PVFValueType.STRING: "STR",
        PVFValueType.REFERENCE: "REF",
        PVFValueType.ARRAY: "ARR",
        PVFValueType.NESTED: "NEST",
    }.get(main_type, str(main_type))

    print(f"0x{key:04X}    {tag:42} {cnt:>8} {type_name:>8}")

# 采样一个完整装备文件内容
print("\n" + "=" * 70)
print("Sample Equipment File Content")
print("=" * 70)

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

# 找一个典型装备
for entry in pvf.indexes:
    if '/character/swordman/equipment/' in entry.name and entry.name.endswith('.equ'):
        data = pvf.extract_file(entry)
        if not data:
            continue

        print(f"\nFile: {entry.name}")
        print(f"Size: {len(data)} bytes")

        try:
            container = parse_pvf_data(data)
            print(f"Entries: {len(container.entries)}")
            print("\nAll key-value pairs:")

            for e in container.entries:
                tag = st.get_tag(e.key)
                if tag:
                    tag = tag.encode('ascii', errors='replace').decode('ascii')[:30]
                else:
                    tag = f"0x{e.key:04X}"

                val = e.value
                if isinstance(val, str):
                    val = val.encode('ascii', errors='replace').decode('ascii')[:40]
                elif isinstance(val, bytes):
                    val = val.hex()[:40]

                type_name = {
                    PVFValueType.INTEGER: "INT",
                    PVFValueType.FLOAT: "FLOAT",
                    PVFValueType.STRING: "STR",
                    PVFValueType.REFERENCE: "REF",
                }.get(e.type, str(e.type))

                print(f"  {tag:32} = {val} ({type_name})")

        except Exception as ex:
            print(f"Parse error: {ex}")

        break

pvf.close()
print("\nDone!")
