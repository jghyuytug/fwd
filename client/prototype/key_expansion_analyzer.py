#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
键映射扩展分析器
================
目标: 从13.8%扩展到20%+ (317 -> 460+键)

策略:
1. 分析高频未映射键
2. 值域聚类推断语义
3. 按格式分析特有键
4. StringTable深度挖掘
"""

import sys
import os
import struct
from collections import Counter, defaultdict
from typing import Dict, List, Set, Tuple
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable

# 当前已映射的键 (从pvf_semantic_keys_v3.py)
KNOWN_KEYS = {
    # COMMON_KEYS
    0x2A12, 0x0B69, 0x0B68, 0x0B67, 0x0B66, 0x0B65, 0x0B64, 0x0B63,
    0x0B62, 0x0B61, 0x0B60, 0x2A11, 0x2A10, 0x2A0F, 0x2A0E, 0x2A0D,
    0x2A0C, 0x2A0B, 0x2A0A, 0x2A09, 0x2A08, 0x2A07, 0x2A06, 0x2A05,
    0x2A04, 0x2A03, 0x2A02, 0x2A01, 0x2A00, 0x29FF, 0x29FE, 0x29FD,
    0x29FC, 0x29FB,
    # CROSS_FORMAT_KEYS
    0x0500, 0x22A9, 0x227F, 0x4338, 0x433A, 0x2280, 0x2281, 0x22AD,
    0x22AE, 0x22AF, 0x22B0, 0x22B1, 0x4183, 0x299E, 0x2BA4, 0x02ED,
    0x1A5C, 0x01E0, 0x82EC,
    # REF_TARGET_KEYS (75个)
    0x5DB0, 0x299A, 0x2999, 0x2998, 0x2997, 0x2996, 0x29B4, 0x29B5,
    0x5DB1, 0x5DB2, 0x5DB3, 0x5DB4, 0x06E7, 0x06EB, 0x06EC, 0x06ED,
    0x06EE, 0x06EF, 0x06F0, 0x06F1, 0x0501, 0x0502, 0x0503, 0x4184,
    0x4185, 0x4186, 0x4187, 0x82ED, 0x82EE, 0x82EF, 0x01E1, 0x01E2,
    # ... 更多已知键
}


def analyze_key_frequency(pvf: PVFReader, sample_size: int = 10000) -> Dict[int, Dict]:
    """分析所有键的频率和特征"""
    key_stats = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'formats': set(),
        'values': [],
        'value_range': [float('inf'), float('-inf')],
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
                if e.key == 0:
                    continue

                stats = key_stats[e.key]
                stats['count'] += 1
                stats['types'][e.type.name] += 1
                stats['formats'].add(ext)

                # 收集值样本
                if e.type == PVFValueType.INTEGER:
                    if len(stats['values']) < 20:
                        stats['values'].append(e.value)
                    stats['value_range'][0] = min(stats['value_range'][0], e.value)
                    stats['value_range'][1] = max(stats['value_range'][1], e.value)
                elif e.type == PVFValueType.FLOAT:
                    if len(stats['values']) < 20:
                        stats['values'].append(e.value)
                    stats['value_range'][0] = min(stats['value_range'][0], e.value)
                    stats['value_range'][1] = max(stats['value_range'][1], e.value)
                elif e.type == PVFValueType.REFERENCE:
                    if len(stats['values']) < 20:
                        stats['values'].append(e.value)
        except:
            pass

        count += 1

    return key_stats


def infer_semantic_from_values(key: int, stats: Dict) -> str:
    """根据值域推断语义"""
    if not stats['values']:
        return ""

    main_type = stats['types'].most_common(1)[0][0] if stats['types'] else ""
    values = stats['values']
    val_range = stats['value_range']

    if main_type == 'INTEGER':
        min_v, max_v = val_range

        # 百分比类 (0-100 或 0-10000)
        if 0 <= min_v and max_v <= 100:
            return "percentage"
        if 0 <= min_v and max_v <= 10000 and all(v % 100 == 0 or v < 100 for v in values[:10]):
            return "percentage_scaled"

        # 等级类 (1-100)
        if 1 <= min_v <= 5 and max_v <= 100:
            return "level"

        # 时间类 (毫秒/秒)
        if all(v % 100 == 0 for v in values[:10]) and 100 <= min_v <= 100000:
            return "time_ms"
        if all(v % 1000 == 0 for v in values[:10]) and 1000 <= min_v <= 300000:
            return "duration_ms"

        # ID类 (大整数)
        if min_v > 10000:
            return "id"

        # 数量类
        if 1 <= min_v <= 10 and max_v <= 999:
            return "count"

        # 坐标类 (可能有负数)
        if min_v < 0 and abs(min_v) < 10000 and abs(max_v) < 10000:
            return "coordinate"

    elif main_type == 'FLOAT':
        min_v, max_v = val_range

        # 倍率类 (0.0-2.0)
        if 0.0 <= min_v and max_v <= 5.0:
            return "multiplier"

        # 概率类 (0.0-1.0)
        if 0.0 <= min_v and max_v <= 1.0:
            return "probability"

    elif main_type == 'REFERENCE':
        return "reference"

    return ""


def analyze_format_specific_keys(pvf: PVFReader, ext: str, sample_size: int = 500) -> Dict[int, Dict]:
    """分析特定格式的专有键"""
    key_stats = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'values': [],
    })

    count = 0
    for entry in pvf.indexes:
        if not entry.name.endswith(ext):
            continue
        if count >= sample_size:
            break

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

                stats = key_stats[e.key]
                stats['count'] += 1
                stats['types'][e.type.name] += 1

                if len(stats['values']) < 10:
                    stats['values'].append(e.value)
        except:
            pass

        count += 1

    return key_stats


def main():
    print("=" * 70)
    print("键映射扩展分析")
    print("目标: 13.8% -> 20%+ (317 -> 460+键)")
    print("=" * 70)

    # 加载StringTable
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=100000)

    print("\n[1/4] 分析键频率分布...")
    key_stats = analyze_key_frequency(pvf, sample_size=20000)

    # 统计
    total_keys = len(key_stats)
    known_count = sum(1 for k in key_stats if k in KNOWN_KEYS)
    unknown_count = total_keys - known_count

    print(f"\n发现 {total_keys} 个唯一键")
    print(f"已映射: {known_count} ({known_count/total_keys*100:.1f}%)")
    print(f"未映射: {unknown_count} ({unknown_count/total_keys*100:.1f}%)")

    # 分析高频未映射键
    print("\n" + "=" * 70)
    print("[2/4] 高频未映射键分析")
    print("=" * 70)

    unknown_keys = [(k, v) for k, v in key_stats.items() if k not in KNOWN_KEYS]
    unknown_keys.sort(key=lambda x: -x[1]['count'])

    print(f"\n{'Key':>8} {'Count':>7} {'Type':>10} {'Formats':>15} {'ValueRange':>20} {'Inferred':>15} {'ST Tag'}")
    print("-" * 95)

    new_mappings = {}

    for key, stats in unknown_keys[:100]:
        main_type = stats['types'].most_common(1)[0][0] if stats['types'] else "?"
        formats = ','.join(sorted(stats['formats']))[:15]

        val_range = ""
        if stats['value_range'][0] != float('inf'):
            min_v, max_v = stats['value_range']
            if isinstance(min_v, float):
                val_range = f"{min_v:.2f}~{max_v:.2f}"
            else:
                val_range = f"{min_v}~{max_v}"

        inferred = infer_semantic_from_values(key, stats)

        # StringTable标签
        tag = st.get_tag(key)
        tag_str = ""
        if tag:
            tag_str = tag.encode('ascii', errors='replace').decode('ascii')[:20]

        print(f"0x{key:04X}  {stats['count']:>7}  {main_type:>10}  {formats:>15}  {val_range:>20}  {inferred:>15}  {tag_str}")

        # 收集可推断的键
        if inferred or tag_str:
            semantic = tag_str.lower().replace(' ', '_') if tag_str else inferred
            semantic = semantic.replace('[', '').replace(']', '').strip('_')
            if semantic:
                new_mappings[key] = semantic

    # 按格式分析
    print("\n" + "=" * 70)
    print("[3/4] 按格式分析特有键")
    print("=" * 70)

    formats_to_analyze = ['.equ', '.mob', '.skl', '.stk', '.qst', '.npc', '.dun', '.map']
    format_specific = {}

    for ext in formats_to_analyze:
        fmt_keys = analyze_format_specific_keys(pvf, ext, sample_size=300)

        # 找出该格式特有的高频键
        specific_keys = []
        for key, stats in fmt_keys.items():
            if key in KNOWN_KEYS:
                continue
            if stats['count'] >= 50:  # 高频
                # 检查是否主要出现在该格式
                global_stats = key_stats.get(key, {})
                if global_stats:
                    formats = global_stats.get('formats', set())
                    if len(formats) <= 2:  # 主要在1-2种格式中
                        specific_keys.append((key, stats))

        if specific_keys:
            specific_keys.sort(key=lambda x: -x[1]['count'])
            format_specific[ext] = specific_keys[:15]

            print(f"\n{ext} 格式特有键 ({len(specific_keys)}个):")
            for key, stats in specific_keys[:10]:
                main_type = stats['types'].most_common(1)[0][0] if stats['types'] else "?"
                tag = st.get_tag(key)
                tag_str = tag.encode('ascii', errors='replace').decode('ascii')[:15] if tag else ""
                vals = str(stats['values'][:3])[:25]
                print(f"  0x{key:04X}  count={stats['count']:>4}  type={main_type:>6}  {tag_str:>15}  {vals}")

                # 添加到新映射
                if tag_str:
                    semantic = f"{ext[1:]}_{tag_str.lower().replace(' ', '_').replace('[', '').replace(']', '')}"
                else:
                    semantic = f"{ext[1:]}_field_{key:04x}"
                new_mappings[key] = semantic

    # 值域聚类分析
    print("\n" + "=" * 70)
    print("[4/4] 值域聚类推断")
    print("=" * 70)

    # 分类统计
    categories = defaultdict(list)
    for key, stats in unknown_keys[:200]:
        inferred = infer_semantic_from_values(key, stats)
        if inferred:
            categories[inferred].append((key, stats))

    for category, keys in sorted(categories.items(), key=lambda x: -len(x[1])):
        print(f"\n{category} ({len(keys)}个键):")
        for key, stats in keys[:8]:
            val_range = stats['value_range']
            range_str = f"{val_range[0]}~{val_range[1]}" if val_range[0] != float('inf') else ""
            print(f"  0x{key:04X}  count={stats['count']:>4}  range={range_str}")

            if key not in new_mappings:
                new_mappings[key] = f"{category}_{key:04x}"

    pvf.close()

    # 输出新映射
    print("\n" + "=" * 70)
    print("新发现的键映射")
    print("=" * 70)

    print(f"\n新增键数: {len(new_mappings)}")
    print(f"总键数将达到: {len(KNOWN_KEYS) + len(new_mappings)}")
    print(f"预计覆盖率: {(len(KNOWN_KEYS) + len(new_mappings)) / 2300 * 100:.1f}%")

    # 输出Python代码
    print("\n# 新增键映射")
    print("EXPANDED_KEYS = {")
    for key in sorted(new_mappings.keys()):
        semantic = new_mappings[key]
        print(f'    0x{key:04X}: "{semantic}",')
    print("}")

    # 保存到文件
    with open('key_expansion_result.txt', 'w', encoding='utf-8') as f:
        f.write(f"# 键映射扩展结果\n")
        f.write(f"# 新增: {len(new_mappings)}键\n")
        f.write(f"# 预计覆盖率: {(len(KNOWN_KEYS) + len(new_mappings)) / 2300 * 100:.1f}%\n\n")
        f.write("EXPANDED_KEYS = {\n")
        for key in sorted(new_mappings.keys()):
            f.write(f'    0x{key:04X}: "{new_mappings[key]}",\n')
        f.write("}\n")

    print("\n结果已保存到 key_expansion_result.txt")


if __name__ == "__main__":
    main()
