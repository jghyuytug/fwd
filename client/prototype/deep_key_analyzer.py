#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
深度键分析器
============
目标: 从未映射的1700+键中挖掘更多语义
"""

import sys
import os
import struct
from collections import Counter, defaultdict
from typing import Dict, List, Set
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable

# 从V4导入已知键
from pvf_semantic_keys_v4 import get_all_keys

KNOWN_KEYS = set(get_all_keys().keys())


def analyze_all_formats(pvf: PVFReader, st: PVFStringTable, sample_per_format: int = 500):
    """分析所有格式中的键"""
    # 按格式收集键
    format_keys = defaultdict(lambda: defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'values': [],
        'value_range': [float('inf'), float('-inf')],
    }))

    # 全局键统计
    global_keys = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'formats': set(),
        'values': [],
    })

    format_counts = Counter()

    for entry in pvf.indexes:
        ext = Path(entry.name).suffix.lower()
        if not ext:
            continue

        # 限制每种格式的采样数
        if format_counts[ext] >= sample_per_format:
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

                # 格式级统计
                fk = format_keys[ext][e.key]
                fk['count'] += 1
                fk['types'][e.type.name] += 1

                # 全局统计
                gk = global_keys[e.key]
                gk['count'] += 1
                gk['types'][e.type.name] += 1
                gk['formats'].add(ext)

                # 收集值样本
                if e.type in (PVFValueType.INTEGER, PVFValueType.FLOAT):
                    if len(fk['values']) < 10:
                        fk['values'].append(e.value)
                    if len(gk['values']) < 10:
                        gk['values'].append(e.value)
                    try:
                        fk['value_range'][0] = min(fk['value_range'][0], float(e.value))
                        fk['value_range'][1] = max(fk['value_range'][1], float(e.value))
                    except:
                        pass

            format_counts[ext] += 1
        except:
            pass

    return format_keys, global_keys


def infer_semantic(key: int, stats: Dict, st: PVFStringTable, format_hint: str = "") -> str:
    """推断键的语义"""
    # 首先检查StringTable标签
    tag = st.get_tag(key)
    if tag:
        # 清理标签
        tag = tag.strip('[]').lower().replace(' ', '_').replace('/', '_')
        if len(tag) > 3 and tag not in ('return', 'true', 'type'):
            if format_hint:
                return f"{format_hint}_{tag}"
            return tag

    # 根据值域推断
    main_type = stats['types'].most_common(1)[0][0] if stats['types'] else ""

    if main_type == 'INTEGER':
        val_range = stats.get('value_range', [float('inf'), float('-inf')])
        if val_range[0] != float('inf'):
            min_v, max_v = val_range
            if 0 <= min_v and max_v <= 100:
                return f"{format_hint}_percent" if format_hint else "percent"
            if 0 <= min_v and max_v <= 10:
                return f"{format_hint}_level" if format_hint else "level"
            if min_v > 10000:
                return f"{format_hint}_id" if format_hint else "id"
            if min_v < 0:
                return f"{format_hint}_offset" if format_hint else "offset"
    elif main_type == 'FLOAT':
        return f"{format_hint}_rate" if format_hint else "rate"
    elif main_type == 'REFERENCE':
        return f"{format_hint}_ref" if format_hint else "ref"
    elif main_type == 'KEY_VALUE':
        return f"{format_hint}_data" if format_hint else "data"
    elif main_type == 'STRING':
        return f"{format_hint}_str" if format_hint else "str"

    return ""


def main():
    print("=" * 70)
    print("深度键分析")
    print("目标: 挖掘更多键语义达到20%覆盖率")
    print("=" * 70)

    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=150000)

    print("\n分析所有格式中的键...")
    format_keys, global_keys = analyze_all_formats(pvf, st, sample_per_format=500)

    pvf.close()

    # 统计
    total_keys = len(global_keys)
    known_count = sum(1 for k in global_keys if k in KNOWN_KEYS)
    unknown_keys = {k: v for k, v in global_keys.items() if k not in KNOWN_KEYS}

    print(f"\n发现 {total_keys} 个唯一键")
    print(f"已映射: {known_count}")
    print(f"未映射: {len(unknown_keys)}")

    # 按频率排序未映射键
    sorted_unknown = sorted(unknown_keys.items(), key=lambda x: -x[1]['count'])

    # 生成新映射
    new_mappings = {}

    print("\n" + "=" * 70)
    print("高频未映射键 (Top 250)")
    print("=" * 70)

    print(f"\n{'Key':>8} {'Count':>6} {'Type':>10} {'Formats':>20} {'Inferred':>25}")
    print("-" * 80)

    for key, stats in sorted_unknown[:250]:
        main_type = stats['types'].most_common(1)[0][0] if stats['types'] else "?"
        formats = ','.join(sorted(stats['formats']))[:20]

        # 确定主要格式
        format_hint = ""
        if len(stats['formats']) == 1:
            format_hint = list(stats['formats'])[0][1:]  # 去掉点号

        inferred = infer_semantic(key, stats, st, format_hint)

        if inferred:
            # 清理语义名称
            inferred = inferred.replace('/', '_').replace('-', '_')
            inferred = ''.join(c if c.isalnum() or c == '_' else '_' for c in inferred)
            inferred = inferred.strip('_')[:30]

            if inferred and len(inferred) > 2:
                new_mappings[key] = inferred

        print(f"0x{key:04X}  {stats['count']:>6}  {main_type:>10}  {formats:>20}  {inferred:>25}")

    # 分析每种格式的特有键
    print("\n" + "=" * 70)
    print("格式特有键分析")
    print("=" * 70)

    formats_to_analyze = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk', '.ai',
                         '.npc', '.map', '.dun', '.chr', '.obj', '.ptl', '.cre', '.til']

    for ext in formats_to_analyze:
        if ext not in format_keys:
            continue

        fkeys = format_keys[ext]
        # 找出该格式特有的高频未映射键
        specific_unknown = []
        for key, stats in fkeys.items():
            if key in KNOWN_KEYS or key in new_mappings:
                continue
            if stats['count'] >= 30:  # 高频
                # 检查是否主要出现在该格式
                global_stats = global_keys.get(key)
                if global_stats and len(global_stats['formats']) <= 2:
                    specific_unknown.append((key, stats))

        if specific_unknown:
            specific_unknown.sort(key=lambda x: -x[1]['count'])

            print(f"\n{ext} 格式特有未映射键 ({len(specific_unknown)}个):")
            for key, stats in specific_unknown[:15]:
                main_type = stats['types'].most_common(1)[0][0] if stats['types'] else "?"
                inferred = infer_semantic(key, stats, st, ext[1:])
                if inferred and len(inferred) > 2:
                    new_mappings[key] = inferred
                vals = str(stats['values'][:3])[:20]
                print(f"  0x{key:04X}  count={stats['count']:>4}  type={main_type:>8}  {inferred:>20}  {vals}")

    # 输出新映射
    print("\n" + "=" * 70)
    print("新发现的键映射")
    print("=" * 70)

    print(f"\n新增键数: {len(new_mappings)}")
    print(f"已有键数: {len(KNOWN_KEYS)}")
    print(f"总键数将达到: {len(KNOWN_KEYS) + len(new_mappings)}")
    print(f"预计覆盖率: {(len(KNOWN_KEYS) + len(new_mappings)) / 2300 * 100:.1f}%")

    # 按分类整理输出
    categorized = defaultdict(dict)
    for key, semantic in new_mappings.items():
        # 按前缀分类
        parts = semantic.split('_')
        if len(parts) > 1 and parts[0] in ['equ', 'mob', 'skl', 'stk', 'qst', 'act', 'atk', 'ai', 'npc', 'map', 'dun', 'chr', 'obj', 'ptl', 'cre', 'til']:
            categorized[parts[0]][key] = semantic
        else:
            categorized['general'][key] = semantic

    # 输出Python代码
    print("\n# === 深度分析新增键 ===")
    for category in sorted(categorized.keys()):
        keys = categorized[category]
        print(f"\n# {category.upper()} Keys ({len(keys)})")
        print(f"{category.upper()}_DEEP_KEYS = {{")
        for key in sorted(keys.keys()):
            print(f'    0x{key:04X}: "{keys[key]}",')
        print("}")

    # 保存到文件
    with open('deep_key_result.py', 'w', encoding='utf-8') as f:
        f.write("#!/usr/bin/env python3\n")
        f.write("# -*- coding: utf-8 -*-\n")
        f.write('"""深度键分析结果"""\n\n')

        for category in sorted(categorized.keys()):
            keys = categorized[category]
            f.write(f"# {category.upper()} Keys ({len(keys)})\n")
            f.write(f"{category.upper()}_DEEP_KEYS = {{\n")
            for key in sorted(keys.keys()):
                f.write(f'    0x{key:04X}: "{keys[key]}",\n')
            f.write("}\n\n")

        # 合并所有
        f.write("# All deep keys\n")
        f.write("ALL_DEEP_KEYS = {\n")
        for key in sorted(new_mappings.keys()):
            f.write(f'    0x{key:04X}: "{new_mappings[key]}",\n')
        f.write("}\n")

    print("\n结果已保存到 deep_key_result.py")


if __name__ == "__main__":
    main()
