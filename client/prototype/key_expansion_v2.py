#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
键映射扩展 V2
=============
目标: 从20.2%扩展到25%+ (464 -> 575+键)
需要新增约111键
"""

import sys
import os
from collections import Counter, defaultdict
from typing import Dict, Set
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable
from pvf_semantic_keys_v4 import get_all_keys

KNOWN_KEYS = set(get_all_keys().keys())


def analyze_remaining_keys(pvf: PVFReader, st: PVFStringTable, sample_size: int = 80000):
    """分析剩余未映射的键"""
    key_stats = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'formats': set(),
        'values': [],
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

                if len(stats['values']) < 5:
                    stats['values'].append(e.value)
        except:
            pass

        count += 1

    return key_stats


def infer_from_stringtable(key: int, st: PVFStringTable) -> str:
    """从StringTable推断语义"""
    tag = st.get_tag(key)
    if tag:
        tag = tag.strip('[]/ ').lower()
        tag = tag.replace(' ', '_').replace('-', '_')
        tag = ''.join(c if c.isalnum() or c == '_' else '' for c in tag)
        if len(tag) >= 3:
            return tag
    return ""


def main():
    print("=" * 60)
    print("键映射扩展 V2")
    print("目标: 20.2% -> 25%+ (464 -> 575+键)")
    print("=" * 60)

    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=150000)

    print("\n分析剩余未映射键...")
    key_stats = analyze_remaining_keys(pvf, st, sample_size=80000)

    pvf.close()

    # 按频率排序
    sorted_keys = sorted(key_stats.items(), key=lambda x: -x[1]['count'])

    print(f"\n发现 {len(key_stats)} 个未映射键")

    # 生成新映射
    new_mappings = {}

    for key, stats in sorted_keys[:200]:
        # 优先使用StringTable标签
        semantic = infer_from_stringtable(key, st)

        if not semantic:
            # 根据类型和格式推断
            main_type = stats['types'].most_common(1)[0][0] if stats['types'] else ""
            formats = stats['formats']

            if len(formats) == 1:
                fmt = list(formats)[0][1:]
                if main_type == 'REFERENCE':
                    semantic = f"{fmt}_ref_{key:04x}"
                elif main_type == 'INTEGER':
                    semantic = f"{fmt}_val_{key:04x}"
                elif main_type == 'KEY_VALUE':
                    semantic = f"{fmt}_data_{key:04x}"
                elif main_type == 'STRING':
                    semantic = f"{fmt}_str_{key:04x}"
                else:
                    semantic = f"{fmt}_field_{key:04x}"
            else:
                if main_type == 'REFERENCE':
                    semantic = f"ref_{key:04x}"
                elif main_type == 'INTEGER':
                    semantic = f"val_{key:04x}"
                elif main_type == 'KEY_VALUE':
                    semantic = f"data_{key:04x}"
                else:
                    semantic = f"field_{key:04x}"

        if semantic:
            new_mappings[key] = semantic

    # 输出
    print(f"\n新增映射: {len(new_mappings)}")
    print(f"总计将达到: {len(KNOWN_KEYS) + len(new_mappings)}")
    print(f"预计覆盖率: {(len(KNOWN_KEYS) + len(new_mappings)) / 2300 * 100:.1f}%")

    # 按分类整理
    categorized = defaultdict(dict)
    for key, semantic in new_mappings.items():
        parts = semantic.split('_')
        if parts[0] in ['equ', 'mob', 'skl', 'stk', 'qst', 'act', 'atk', 'ai',
                       'npc', 'map', 'dgn', 'chr', 'obj', 'ptl', 'cre', 'til',
                       'etc', 'ui', 'als', 'aic', 'ani', 'key', 'wrd', 'rep',
                       'lst', 'mm', 'msn', 'ora', 'apd', 'vm', 'tbl', 'twn',
                       'gdata', 'wdm', 'snd']:
            categorized[parts[0]][key] = semantic
        else:
            categorized['general'][key] = semantic

    # 输出Python代码
    print("\n# === V5新增键 ===")
    for category in sorted(categorized.keys()):
        keys = categorized[category]
        if len(keys) >= 3:
            print(f"\n{category.upper()}_V5_KEYS = {{")
            for k in sorted(keys.keys()):
                print(f'    0x{k:04X}: "{keys[k]}",')
            print("}")

    # 保存到文件
    with open('key_expansion_v5.py', 'w', encoding='utf-8') as f:
        f.write("#!/usr/bin/env python3\n")
        f.write('"""键映射扩展V5 - 目标25%+"""\n\n')

        for category in sorted(categorized.keys()):
            keys = categorized[category]
            if len(keys) >= 2:
                f.write(f"# {category.upper()} Keys\n")
                f.write(f"{category.upper()}_V5_KEYS = {{\n")
                for k in sorted(keys.keys()):
                    f.write(f'    0x{k:04X}: "{keys[k]}",\n')
                f.write("}\n\n")

        # ALL_V5_KEYS
        f.write("# All V5 Keys\n")
        f.write("ALL_V5_KEYS = {\n")
        for k in sorted(new_mappings.keys()):
            f.write(f'    0x{k:04X}: "{new_mappings[k]}",\n')
        f.write("}\n")

    print("\n结果已保存到 key_expansion_v5.py")


if __name__ == "__main__":
    main()
