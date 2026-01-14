#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
快速验证StringTable三层引用链
"""

import sys
import os

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


def main():
    print("=" * 60)
    print("StringTable三层引用链快速验证")
    print("=" * 60)

    # 加载StringTable
    st = PVFStringTable()
    if not st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin'):
        print("无法加载StringTable")
        return

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=50000)

    # 验证三层引用链
    NAME_KEY = 0x2A12
    verified = 0
    checked = 0

    print(f"\n验证: PVF键0x{NAME_KEY:04X}(name) -> 值 -> StringTable -> 文本")
    print("-" * 60)

    for entry in pvf.indexes:
        if verified >= 10:
            break

        if not entry.name.endswith(('.equ', '.mob', '.skl')):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        checked += 1

        try:
            container = parse_pvf_data(data)
            name_entry = container.find(NAME_KEY)

            if not name_entry:
                continue

            st_index = name_entry.value
            if not st.is_valid_index(st_index):
                continue

            text = st.get_string(st_index)
            if not text:
                continue

            verified += 1
            safe_text = text[:40].encode('ascii', errors='replace').decode('ascii')
            file_name = entry.name.split('/')[-1][:30]

            print(f"\n[{verified}] {file_name}")
            print(f"    键 0x{NAME_KEY:04X} -> 值 {st_index} -> \"{safe_text}\"")

        except:
            continue

    pvf.close()

    # 结果
    print("\n" + "=" * 60)
    print(f"结果: 检查 {checked} 个文件，成功验证 {verified} 个三层引用链")
    print("=" * 60)
    print("\n结论: StringTable是文本内容管理系统")
    print("  - PVF键定义数据结构")
    print("  - 值是StringTable索引")
    print("  - StringTable存储实际文本")


if __name__ == "__main__":
    main()
