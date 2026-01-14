#!/usr/bin/env python3
"""dump一个装备文件的完整内容"""

import sys
import os

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

# 分析几个不同类型的装备
targets = [
    'equipment/character/swordman/weapon/club/pvp_club2222002.equ',  # 武器
    'equipment/character/fighter/weapon/tonfa/tonfa_2213101.equ',    # 武器
    'equipment/character/common/pants/leather/pant_lt_2choro92.equ', # 防具
]

for target in targets:
    for entry in pvf.indexes:
        if entry.name == target:
            data = pvf.extract_file(entry)
            if not data:
                continue

            print("=" * 70)
            print(f"File: {entry.name}")
            print(f"Size: {len(data)} bytes")
            print("=" * 70)

            try:
                container = parse_pvf_data(data)
                print(f"Total entries: {len(container.entries)}")
                print()

                for e in container.entries:
                    tag = st.get_tag(e.key)
                    if tag:
                        tag = tag.encode('ascii', errors='replace').decode('ascii')
                    else:
                        tag = f"KEY_0x{e.key:04X}"

                    type_name = {
                        PVFValueType.INTEGER: "INT",
                        PVFValueType.FLOAT: "FLOAT",
                        PVFValueType.STRING: "STR",
                        PVFValueType.REFERENCE: "REF",
                        PVFValueType.ARRAY: "ARR",
                        PVFValueType.NESTED: "NEST",
                    }.get(e.type, f"T{e.type}")

                    val = e.value
                    if isinstance(val, str):
                        val = val.encode('ascii', errors='replace').decode('ascii')
                    elif isinstance(val, bytes):
                        val = val.hex()
                    elif isinstance(val, list):
                        val = str(val)[:60]

                    print(f"  {tag:40} ({type_name:5}) = {val}")

            except Exception as ex:
                import traceback
                traceback.print_exc()

            print()
            break

pvf.close()
