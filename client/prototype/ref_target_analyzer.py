#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
引用目标类型追踪
================
分析每个REF类型的键主要指向什么类型的文件
"""

import sys
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Tuple
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


def analyze_ref_targets(pvf: PVFReader, ext: str, sample_size: int = 500):
    """分析特定格式中REF键指向的目标文件类型"""
    # key -> target_ext -> count
    ref_targets = defaultdict(lambda: Counter())

    # 建立索引->文件名映射
    index_to_file = {}
    for i, entry in enumerate(pvf.indexes):
        index_to_file[i] = entry.name

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
                if e.type == PVFValueType.REFERENCE and e.key != 0:
                    ref_value = e.value

                    # 查找目标文件
                    if ref_value in index_to_file:
                        target_file = index_to_file[ref_value]
                        target_ext = Path(target_file).suffix.lower()
                        ref_targets[e.key][target_ext] += 1
                    else:
                        # 可能是StringTable索引
                        ref_targets[e.key]['_stringtable'] += 1
        except:
            pass

        count += 1
        if count >= sample_size:
            break

    return ref_targets


def main():
    print("=" * 60)
    print("引用目标类型追踪")
    print("=" * 60)

    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    formats = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk', '.ai']

    all_ref_semantics = {}

    for ext in formats:
        print(f"\n{'='*60}")
        print(f"分析 {ext} 格式的引用目标")
        print('='*60)

        ref_targets = analyze_ref_targets(pvf, ext, sample_size=500)

        if not ref_targets:
            print("  未发现REF类型键")
            continue

        print(f"\n发现 {len(ref_targets)} 个REF键:")
        print(f"\n{'Key':>8} {'Total':>6} {'主要目标':30} {'推断语义'}")
        print('-'*70)

        for key in sorted(ref_targets.keys(), key=lambda k: -sum(ref_targets[k].values())):
            targets = ref_targets[key]
            total = sum(targets.values())
            if total < 3:
                continue

            # 找出主要目标
            main_target, main_count = targets.most_common(1)[0]
            pct = main_count / total * 100

            # 推断语义
            semantic = infer_ref_semantic(key, main_target, targets)

            target_str = f"{main_target} ({main_count}/{total}, {pct:.0f}%)"

            # StringTable标签
            tag = st.get_tag(key)
            tag_str = ""
            if tag:
                tag_str = tag.encode('ascii', errors='replace').decode('ascii')[:15]

            print(f"0x{key:04X}  {total:>6}  {target_str:30} {semantic:20} {tag_str}")

            all_ref_semantics[key] = {
                'main_target': main_target,
                'semantic': semantic,
                'total': total,
                'formats': [ext],
            }

    pvf.close()

    # 输出推断的REF键语义
    print("\n" + "=" * 60)
    print("REF键语义推断结果")
    print("=" * 60)

    print("\nREF_SEMANTIC_KEYS = {")
    for key in sorted(all_ref_semantics.keys()):
        info = all_ref_semantics[key]
        if info['total'] >= 10:
            print(f"    0x{key:04X}: \"{info['semantic']}\",  # -> {info['main_target']}")
    print("}")


def infer_ref_semantic(key: int, main_target: str, targets: Counter) -> str:
    """根据引用目标推断语义"""
    if main_target == '.ani':
        return "animation_ref"
    elif main_target == '.skl':
        return "skill_ref"
    elif main_target == '.mob':
        return "monster_ref"
    elif main_target == '.equ':
        return "equipment_ref"
    elif main_target == '.stk':
        return "item_ref"
    elif main_target == '.act':
        return "action_ref"
    elif main_target == '.atk':
        return "attack_ref"
    elif main_target == '.ai':
        return "ai_ref"
    elif main_target == '.qst':
        return "quest_ref"
    elif main_target == '.ptl':
        return "particle_ref"
    elif main_target == '.obj':
        return "object_ref"
    elif main_target == '.map':
        return "map_ref"
    elif main_target == '.npc':
        return "npc_ref"
    elif main_target == '.img':
        return "image_ref"
    elif main_target == '.wav' or main_target == '.ogg':
        return "sound_ref"
    elif main_target == '_stringtable':
        return "text_ref"
    else:
        return f"ref_to_{main_target[1:] if main_target.startswith('.') else main_target}"


if __name__ == "__main__":
    main()
