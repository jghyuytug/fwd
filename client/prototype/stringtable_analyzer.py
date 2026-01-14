#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
StringTable用途分析器
=====================
验证StringTable的三层引用架构:
PVF数据键 -> 值(StringTable索引) -> StringTable -> 实际文本

分析内容:
1. 标签格式[xxx]的分布
2. 非标签内容的类型
3. 验证三层引用链
"""

import sys
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Tuple, Optional

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable


@dataclass
class StringTableAnalysis:
    """StringTable分析结果"""
    total_entries: int = 0
    tag_entries: int = 0           # [xxx]格式标签数量
    text_entries: int = 0          # 纯文本数量
    empty_entries: int = 0         # 空条目
    tag_patterns: Dict[str, int] = None
    sample_tags: List[Tuple[int, str]] = None
    sample_texts: List[Tuple[int, str]] = None


def analyze_stringtable(st: PVFStringTable, sample_range: int = 100000) -> StringTableAnalysis:
    """分析StringTable的内容分布"""
    result = StringTableAnalysis()
    result.total_entries = st.count
    result.tag_patterns = Counter()
    result.sample_tags = []
    result.sample_texts = []

    for i in range(min(st.count, sample_range)):
        raw = st.get_string(i)
        if not raw:
            result.empty_entries += 1
            continue

        # 检查是否为[xxx]格式标签
        if '[' in raw and ']' in raw:
            result.tag_entries += 1

            # 提取标签
            start = raw.find('[')
            end = raw.find(']', start)
            if end > start:
                tag = raw[start:end+1]
                result.tag_patterns[tag] += 1

                # 采样
                if len(result.sample_tags) < 50:
                    result.sample_tags.append((i, tag))
        else:
            result.text_entries += 1

            # 采样非标签文本
            if len(result.sample_texts) < 20:
                safe_text = raw[:50].encode('ascii', errors='replace').decode('ascii')
                result.sample_texts.append((i, safe_text))

    return result


def verify_reference_chain(st: PVFStringTable, pvf: PVFReader, sample_count: int = 10):
    """
    验证三层引用链:
    PVF数据键(如name) -> 值(StringTable索引) -> StringTable -> 实际文本
    """
    print("\n" + "=" * 70)
    print("验证三层引用链")
    print("=" * 70)
    print("结构: PVF数据键 -> 值(ST索引) -> StringTable -> 文本")
    print("-" * 70)

    # 已知的名称键
    NAME_KEY = 0x2A12

    verified = 0
    samples = []

    for entry in pvf.indexes:
        if verified >= sample_count:
            break

        # 只检查有意义的格式
        if not any(entry.name.endswith(ext) for ext in ['.equ', '.mob', '.skl', '.stk']):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue

        # 必须是PVF格式
        if data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)

            # 查找NAME键的引用值
            name_entry = container.find(NAME_KEY)
            if not name_entry:
                continue

            # 引用类型才有意义
            if name_entry.type not in [PVFValueType.REFERENCE, PVFValueType.INTEGER]:
                continue

            st_index = name_entry.value

            # 检查是否在StringTable范围内
            if not st.is_valid_index(st_index):
                continue

            # 获取StringTable中的文本
            text = st.get_string(st_index)
            if not text:
                continue

            # 成功验证一个链
            safe_text = text[:40].encode('ascii', errors='replace').decode('ascii')
            samples.append({
                'file': entry.name.split('/')[-1],
                'key': f'0x{NAME_KEY:04X}',
                'st_index': st_index,
                'text': safe_text,
            })
            verified += 1

        except Exception:
            continue

    # 输出验证结果
    if samples:
        print(f"\n成功验证 {len(samples)} 个三层引用链:\n")
        for i, s in enumerate(samples, 1):
            print(f"[{i}] {s['file']}")
            print(f"    PVF键: {s['key']} (name)")
            print(f"    -> ST索引: {s['st_index']}")
            print(f"    -> 文本: \"{s['text']}\"")
            print()
    else:
        print("未能验证任何引用链")

    return samples


def analyze_ref_values_distribution(st: PVFStringTable, pvf: PVFReader, sample_size: int = 2000):
    """分析PVF中引用值的分布，看是否指向StringTable"""
    print("\n" + "=" * 70)
    print("引用值分布分析")
    print("=" * 70)

    ref_values = []
    int_values_as_ref = []

    count = 0
    for entry in pvf.indexes:
        if count >= sample_size:
            break

        if not entry.name.endswith(('.equ', '.mob', '.skl')):
            continue

        data = pvf.extract_file(entry)
        if not data or data[0] != 0xB0 or data[1] != 0xD0:
            continue

        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.type == PVFValueType.REFERENCE:
                    ref_values.append(e.value)
                elif e.type == PVFValueType.INTEGER:
                    # 整数值如果在StringTable范围内
                    if 0 < e.value < st.count:
                        int_values_as_ref.append(e.value)
        except:
            pass

        count += 1

    print(f"\n分析 {count} 个PVF文件:")
    print(f"  REF类型值: {len(ref_values)}")
    print(f"  INT值在ST范围内: {len(int_values_as_ref)}")

    # REF值分布
    if ref_values:
        ref_min = min(ref_values)
        ref_max = max(ref_values)
        ref_avg = sum(ref_values) / len(ref_values)

        print(f"\nREF值范围:")
        print(f"  最小: {ref_min}")
        print(f"  最大: {ref_max}")
        print(f"  平均: {ref_avg:.0f}")
        print(f"  StringTable范围: 0-{st.count}")

        # 检查有多少REF值在StringTable有效范围内
        valid_st_refs = sum(1 for v in ref_values if v < st.count)
        print(f"  在ST范围内的REF: {valid_st_refs}/{len(ref_values)} ({valid_st_refs/len(ref_values)*100:.1f}%)")

    return ref_values, int_values_as_ref


def export_tag_index_mapping(st: PVFStringTable, output_file: str, max_entries: int = 50000):
    """导出[xxx]格式标签的索引映射"""
    print(f"\n导出标签映射到: {output_file}")

    mappings = []
    for i in range(min(st.count, max_entries)):
        raw = st.get_string(i)
        if not raw or '[' not in raw:
            continue

        # 提取标签
        start = raw.find('[')
        end = raw.find(']', start)
        if end > start:
            tag = raw[start:end+1]
            mappings.append((i, tag))

    # 写入文件
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("# StringTable标签索引映射\n")
        f.write(f"# 共 {len(mappings)} 个标签\n\n")

        for idx, tag in sorted(mappings, key=lambda x: x[1]):
            f.write(f"0x{idx:04X}\t{tag}\n")

    print(f"已导出 {len(mappings)} 个标签映射")
    return mappings


def main():
    print("=" * 70)
    print("StringTable用途分析")
    print("=" * 70)

    # 加载StringTable
    st = PVFStringTable()
    st_path = 'C:/Users/waw/Desktop/game/stringtable.bin'
    if not st.load_from_file(st_path):
        print(f"无法加载StringTable: {st_path}")
        return

    # 加载PVF
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 1. 分析StringTable内容分布
    print("\n" + "-" * 70)
    print("1. StringTable内容分布分析")
    print("-" * 70)

    analysis = analyze_stringtable(st, sample_range=100000)

    print(f"\n总条目数: {analysis.total_entries:,}")
    print(f"采样范围: 前100,000条")
    print(f"  [xxx]标签: {analysis.tag_entries:,} ({analysis.tag_entries/100000*100:.1f}%)")
    print(f"  纯文本: {analysis.text_entries:,} ({analysis.text_entries/100000*100:.1f}%)")
    print(f"  空条目: {analysis.empty_entries:,}")

    print(f"\n最常见的标签 (前20):")
    for tag, count in analysis.tag_patterns.most_common(20):
        tag_safe = tag.encode('ascii', errors='replace').decode('ascii')
        print(f"  {tag_safe:30} {count:>6}")

    print(f"\n标签样本:")
    for idx, tag in analysis.sample_tags[:10]:
        tag_safe = tag.encode('ascii', errors='replace').decode('ascii')
        print(f"  索引 {idx:>6} (0x{idx:04X}): {tag_safe}")

    print(f"\n非标签文本样本:")
    for idx, text in analysis.sample_texts[:10]:
        print(f"  索引 {idx:>6} (0x{idx:04X}): \"{text}\"")

    # 2. 验证三层引用链
    verify_reference_chain(st, pvf, sample_count=10)

    # 3. 分析引用值分布
    analyze_ref_values_distribution(st, pvf, sample_size=2000)

    # 4. 导出标签映射
    export_tag_index_mapping(
        st,
        'C:/Users/waw/Desktop/game/client/prototype/stringtable_tags.txt',
        max_entries=100000
    )

    pvf.close()

    # 总结
    print("\n" + "=" * 70)
    print("总结: StringTable的真正用途")
    print("=" * 70)
    print("""
1. StringTable是独立的文本内容管理系统，不是PVF键的直接映射

2. 三层架构:
   PVF数据键(0x2a12) -> 值(45678) -> StringTable[45678] -> "技能名称"

3. 内容类型:
   - [xxx]格式标签: 用于数据结构定义和调试
   - 纯文本: 游戏中显示的实际名称、说明等

4. 主要用途:
   - UI文本显示
   - 多语言本地化
   - 集中文本管理
""")


if __name__ == "__main__":
    main()
