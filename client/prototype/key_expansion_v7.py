#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
键映射扩展 V7
=============
目标: 从45.3%扩展到50%+ (1041 -> 1150+键)
需要新增约109键
"""

import sys
import os
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable
from pvf_semantic_keys_v4 import get_all_keys

KNOWN_KEYS = set(get_all_keys().keys())


def analyze_remaining(pvf: PVFReader, st: PVFStringTable, sample_size: int = 140000):
    """分析剩余未映射的键"""
    key_stats = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'formats': set(),
    })

    count = 0
    for entry in pvf.indexes:
        if count >= sample_size:
            break

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        ext = Path(entry.name).suffix.lower()

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.key == 0 or e.key in KNOWN_KEYS:
                    continue

                stats = key_stats[e.key]
                stats['count'] += 1
                stats['types'][e.type.name] += 1
                stats['formats'].add(ext)
        except:
            pass

        count += 1
        if count % 30000 == 0:
            print(f"  Processed {count} files...")

    return key_stats


def clean_tag(tag):
    if not tag:
        return ""
    tag = tag.strip('[]/ ').lower()
    tag = tag.replace(' ', '_').replace('-', '_')
    result = ''.join(c if (c.isascii() and (c.isalnum() or c == '_')) else '' for c in tag)
    return result if len(result) >= 3 else ""


def main():
    print("=" * 60)
    print("Key Expansion V7")
    print("Target: 45.3% -> 50%+ (1041 -> 1150+ keys)")
    print("=" * 60)

    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=150000)

    print(f"\nCurrent keys: {len(KNOWN_KEYS)}")
    print("Analyzing remaining unmapped keys...")
    key_stats = analyze_remaining(pvf, st, sample_size=140000)

    pvf.close()

    sorted_keys = sorted(key_stats.items(), key=lambda x: -x[1]['count'])
    print(f"\nFound {len(key_stats)} unmapped keys")

    # Generate 115 new mappings
    new_mappings = {}

    for key, stats in sorted_keys[:115]:
        tag = st.get_tag(key)
        semantic = clean_tag(tag)

        if not semantic:
            main_type = stats['types'].most_common(1)[0][0] if stats['types'] else ""
            formats = stats['formats']

            prefix = ""
            if len(formats) == 1:
                prefix = list(formats)[0][1:] + "_"

            type_map = {
                'REFERENCE': 'ref', 'INTEGER': 'int', 'KEY_VALUE': 'data',
                'STRING': 'str', 'FLOAT': 'float'
            }
            suffix = type_map.get(main_type, 'field')
            semantic = f"{prefix}{suffix}_{key:04x}"

        if semantic:
            new_mappings[key] = semantic

    total_keys = len(KNOWN_KEYS) + len(new_mappings)
    print(f"\nNew mappings: {len(new_mappings)}")
    print(f"Total will reach: {total_keys}")
    print(f"Expected coverage: {total_keys / 2300 * 100:.1f}%")

    # Save to file
    with open('key_expansion_v10.py', 'w', encoding='utf-8') as f:
        f.write("#!/usr/bin/env python3\n")
        f.write('"""键映射扩展V10 - 目标50%+"""\n\n')
        f.write("V10_KEYS = {\n")
        for key in sorted(new_mappings.keys()):
            f.write(f'    0x{key:04X}: "{new_mappings[key]}",\n')
        f.write("}\n")

    print("\nSaved to key_expansion_v10.py")
    print("\n# First 40 V10 keys:")
    for i, key in enumerate(sorted(new_mappings.keys())[:40]):
        print(f'    0x{key:04X}: "{new_mappings[key]}",')


if __name__ == "__main__":
    main()
