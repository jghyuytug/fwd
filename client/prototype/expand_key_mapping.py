#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
扩展键语义映射
==============
通过数据模式分析推断更多键的语义
"""

import sys
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Any, Set

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


@dataclass
class KeyAnalysis:
    """键分析结果"""
    key: int
    count: int
    value_type: str
    min_val: Any = None
    max_val: Any = None
    unique_count: int = 0
    sample_values: List[Any] = None
    inferred_meaning: str = ""


def analyze_key_patterns(ext: str, sample_size: int = 2000) -> Dict[int, KeyAnalysis]:
    """分析特定格式的键模式"""
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    key_data = defaultdict(lambda: {
        'count': 0,
        'types': Counter(),
        'int_values': [],
        'float_values': [],
        'str_values': [],
        'ref_values': [],
    })

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
                if e.key == 0:
                    continue

                kd = key_data[e.key]
                kd['count'] += 1
                kd['types'][e.type] += 1

                if e.type == PVFValueType.INTEGER:
                    if len(kd['int_values']) < 100:
                        kd['int_values'].append(e.value)
                elif e.type == PVFValueType.FLOAT:
                    if len(kd['float_values']) < 100:
                        kd['float_values'].append(e.value)
                elif e.type == PVFValueType.STRING:
                    if len(kd['str_values']) < 10:
                        kd['str_values'].append(e.value)
                elif e.type == PVFValueType.REFERENCE:
                    if len(kd['ref_values']) < 100:
                        kd['ref_values'].append(e.value)
        except:
            pass

        count += 1
        if count >= sample_size:
            break

    pvf.close()

    # 分析每个键
    results = {}
    for key, kd in key_data.items():
        analysis = KeyAnalysis(
            key=key,
            count=kd['count'],
            value_type="",
            sample_values=[]
        )

        # 确定主要类型
        if kd['types']:
            main_type = max(kd['types'], key=kd['types'].get)
            type_names = {
                PVFValueType.INTEGER: "INT",
                PVFValueType.FLOAT: "FLOAT",
                PVFValueType.STRING: "STR",
                PVFValueType.REFERENCE: "REF",
            }
            analysis.value_type = type_names.get(main_type, str(main_type))

        # 分析数值范围
        if kd['int_values']:
            vals = kd['int_values']
            analysis.min_val = min(vals)
            analysis.max_val = max(vals)
            analysis.unique_count = len(set(vals))
            analysis.sample_values = vals[:5]

            # 推断语义
            analysis.inferred_meaning = infer_int_meaning(
                key, vals, analysis.min_val, analysis.max_val, analysis.unique_count
            )

        elif kd['float_values']:
            vals = kd['float_values']
            analysis.min_val = min(vals)
            analysis.max_val = max(vals)
            analysis.unique_count = len(set(vals))
            analysis.sample_values = [round(v, 2) for v in vals[:5]]
            analysis.inferred_meaning = infer_float_meaning(key, vals)

        elif kd['str_values']:
            analysis.sample_values = kd['str_values'][:3]
            analysis.unique_count = len(set(kd['str_values']))
            analysis.inferred_meaning = "text_field"

        elif kd['ref_values']:
            vals = kd['ref_values']
            analysis.min_val = min(vals)
            analysis.max_val = max(vals)
            analysis.unique_count = len(set(vals))
            analysis.sample_values = vals[:5]
            analysis.inferred_meaning = "file_reference"

        results[key] = analysis

    return results


def infer_int_meaning(key: int, values: List[int], min_v: int, max_v: int, unique: int) -> str:
    """推断整数键的语义"""
    avg = sum(values) / len(values) if values else 0

    # 布尔值
    if set(values) <= {0, 1}:
        return "boolean_flag"

    # 小范围枚举
    if unique <= 10 and max_v < 100:
        return "enum_type"

    # 等级相关 (1-100范围)
    if 1 <= min_v and max_v <= 100 and avg < 60:
        return "level_or_grade"

    # 百分比 (0-100或0-10000)
    if min_v >= 0 and max_v <= 100:
        return "percentage"
    if min_v >= 0 and max_v <= 10000 and avg < 5000:
        return "percentage_scaled"

    # 大数值 - 可能是HP/经验
    if avg > 10000:
        return "large_value_hp_exp"

    # 中等数值 - 可能是攻防
    if 100 < avg < 5000:
        return "medium_value_stat"

    # 小数值 - 可能是倍率/数量
    if avg < 100:
        return "small_value_count"

    # 负数存在
    if min_v < 0:
        return "signed_value"

    return "unknown_int"


def infer_float_meaning(key: int, values: List[float]) -> str:
    """推断浮点键的语义"""
    avg = sum(values) / len(values) if values else 0

    if 0 <= avg <= 1:
        return "ratio_0_1"
    if 0 <= avg <= 100:
        return "percentage_float"
    if avg > 100:
        return "large_float"

    return "unknown_float"


def main():
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    formats = ['.equ', '.mob', '.skl', '.stk', '.qst', '.act', '.atk', '.ai']

    all_keys = {}

    for ext in formats:
        print(f"\n{'='*70}")
        print(f"Analyzing {ext}")
        print('='*70)

        results = analyze_key_patterns(ext, sample_size=1000)

        # 按出现次数排序
        sorted_keys = sorted(results.values(), key=lambda x: -x.count)

        print(f"Found {len(sorted_keys)} unique keys")
        print(f"\n{'Key':>8} {'Type':>6} {'Count':>7} {'Min':>10} {'Max':>10} {'Uniq':>5} {'Inferred Meaning'}")
        print('-'*70)

        for analysis in sorted_keys[:30]:
            tag = st.get_tag(analysis.key)
            if tag:
                tag = tag.encode('ascii', 'replace').decode('ascii')[:20]
            else:
                tag = ""

            min_str = str(analysis.min_val)[:10] if analysis.min_val is not None else ""
            max_str = str(analysis.max_val)[:10] if analysis.max_val is not None else ""

            print(f"0x{analysis.key:04X}  {analysis.value_type:>6} {analysis.count:>7} "
                  f"{min_str:>10} {max_str:>10} {analysis.unique_count:>5}  {analysis.inferred_meaning}")

            # 收集到总表
            if analysis.key not in all_keys:
                all_keys[analysis.key] = {
                    'formats': [],
                    'inferred': analysis.inferred_meaning,
                    'type': analysis.value_type,
                }
            all_keys[analysis.key]['formats'].append(ext)

    # 输出综合映射建议
    print("\n" + "="*70)
    print("Suggested Key Mappings (高频跨格式键)")
    print("="*70)

    multi_format_keys = {k: v for k, v in all_keys.items()
                         if len(v['formats']) >= 2}

    for key in sorted(multi_format_keys.keys()):
        info = multi_format_keys[key]
        tag = st.get_tag(key)
        tag_str = tag.encode('ascii', 'replace').decode('ascii')[:25] if tag else ""
        formats_str = ','.join(info['formats'])
        print(f"  0x{key:04X}  {tag_str:27} {info['type']:>5}  {info['inferred']:25} [{formats_str}]")


if __name__ == "__main__":
    main()
