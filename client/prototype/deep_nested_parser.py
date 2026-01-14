#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
深度嵌套结构解析器
==================
正确解析嵌套KEY_VALUE的内部结构
"""

import sys
import os
import struct
from collections import Counter, defaultdict
from typing import Dict, List, Optional, Any

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_stringtable import PVFStringTable


class NestedParser:
    """嵌套结构解析器"""

    def __init__(self, data: bytes):
        self.data = data
        self.pos = 0
        self.size = len(data)

    def has_more(self) -> bool:
        return self.pos < self.size

    def read_u8(self) -> int:
        if self.pos >= self.size:
            return 0
        val = self.data[self.pos]
        self.pos += 1
        return val

    def read_u16(self) -> int:
        if self.pos + 2 > self.size:
            return 0
        val = struct.unpack_from('<H', self.data, self.pos)[0]
        self.pos += 2
        return val

    def read_u32(self) -> int:
        if self.pos + 4 > self.size:
            return 0
        val = struct.unpack_from('<I', self.data, self.pos)[0]
        self.pos += 4
        return val

    def read_i32(self) -> int:
        if self.pos + 4 > self.size:
            return 0
        val = struct.unpack_from('<i', self.data, self.pos)[0]
        self.pos += 4
        return val

    def read_f32(self) -> float:
        if self.pos + 4 > self.size:
            return 0.0
        val = struct.unpack_from('<f', self.data, self.pos)[0]
        self.pos += 4
        return val

    def parse_entry(self, depth: int = 0) -> Optional[Dict]:
        """解析单个条目"""
        if not self.has_more():
            return None

        start_pos = self.pos
        type_byte = self.read_u8()

        entry = {
            'offset': start_pos,
            'type': type_byte,
            'key': 0,
            'value': None,
            'children': [],
        }

        if type_byte == 0x05:  # KEY_VALUE
            key = self.read_u16()
            extra = self.read_u16()
            inner_type = self.read_u8()

            entry['key'] = key
            entry['extra'] = extra
            entry['inner_type'] = inner_type

            if inner_type == 0x02:  # INTEGER
                entry['value'] = self.read_i32()
                entry['value_type'] = 'INT'
            elif inner_type == 0x04:  # FLOAT
                entry['value'] = self.read_f32()
                entry['value_type'] = 'FLOAT'
            elif inner_type == 0x07:  # REFERENCE
                entry['value'] = self.read_u16()
                entry['value_type'] = 'REF'
            elif inner_type == 0x05:  # NESTED
                sub_key = self.read_u16()
                entry['value'] = sub_key
                entry['value_type'] = 'NESTED'
            elif inner_type == 0x09:  # STRING
                str_len = self.read_u32()
                marker = self.read_u8()
                if marker == 0x0a and str_len > 0 and self.pos + str_len <= self.size:
                    entry['value'] = self.data[self.pos:self.pos + str_len].decode('utf-8', errors='replace')
                    self.pos += str_len
                entry['value_type'] = 'STRING'
            else:
                entry['value'] = self.read_u32()
                entry['value_type'] = f'UNK_{inner_type:02X}'

        elif type_byte == 0x02:  # INTEGER without key
            entry['value'] = self.read_i32()
            entry['value_type'] = 'INT'
        elif type_byte == 0x04:  # FLOAT without key
            entry['value'] = self.read_f32()
            entry['value_type'] = 'FLOAT'
        elif type_byte == 0x07:  # REF without key
            entry['value'] = self.read_u16()
            entry['value_type'] = 'REF'
        else:
            # 未知类型，回退
            self.pos = start_pos + 1
            entry['value_type'] = f'UNK_{type_byte:02X}'

        return entry

    def parse_all(self, max_entries: int = 100) -> List[Dict]:
        """解析所有条目"""
        entries = []
        while self.has_more() and len(entries) < max_entries:
            entry = self.parse_entry()
            if entry:
                entries.append(entry)
            else:
                break
        return entries


def analyze_nested_block(data: bytes, nested_key: int, sample_files: int = 10):
    """分析嵌套块的完整结构"""
    print(f"\n分析嵌套键 0x{nested_key:04X}")
    print("-" * 60)

    # 找到嵌套位置
    positions = []
    i = 2 if len(data) >= 2 and data[0] == 0xB0 and data[1] == 0xD0 else 0

    while i < len(data) - 6:
        if data[i] == 0x05:
            key = struct.unpack_from('<H', data, i + 1)[0]
            extra = struct.unpack_from('<H', data, i + 3)[0]
            inner_type = data[i + 5]

            if key == nested_key and inner_type == 0x05:
                positions.append(i)
        i += 1

    if not positions:
        print("  未找到嵌套结构")
        return []

    print(f"  找到 {len(positions)} 个嵌套结构位置")

    all_inner_keys = defaultdict(lambda: {'count': 0, 'types': Counter(), 'values': []})

    for pos in positions[:5]:
        print(f"\n  位置 {pos}:")
        print(f"    原始hex: {data[pos:pos+40].hex()}")

        # 跳过外层05 + key(2) + extra(2) + inner_type(1) + sub_key(2) = 8字节
        inner_start = pos + 8
        parser = NestedParser(data[inner_start:])
        entries = parser.parse_all(max_entries=30)

        print(f"    解析到 {len(entries)} 个内部条目:")
        for j, e in enumerate(entries[:10]):
            val_str = str(e['value'])[:30] if e['value'] is not None else "None"
            print(f"      [{j}] key=0x{e['key']:04X} type={e.get('value_type', '?'):8} val={val_str}")

            # 收集统计
            if e['key'] != 0:
                info = all_inner_keys[e['key']]
                info['count'] += 1
                info['types'][e.get('value_type', '?')] += 1
                if len(info['values']) < 5 and e['value'] is not None:
                    info['values'].append(e['value'])

    return all_inner_keys


def main():
    print("=" * 60)
    print("深度嵌套结构解析")
    print("=" * 60)

    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=50000)

    # 分析几个关键格式
    test_cases = [
        ('.skl', 0x0500, 'SP数据'),
        ('.mob', 0x299E, '精灵数据'),
        ('.equ', 0x4183, '模型数据'),
        ('.ai', 0x01E0, 'AI类型'),
        ('.qst', 0x82EC, '奖励数据'),
    ]

    all_results = {}

    for ext, nested_key, desc in test_cases:
        print(f"\n{'='*60}")
        print(f"格式: {ext} - {desc}")
        print('='*60)

        # 找几个样本文件
        sample_count = 0
        format_keys = defaultdict(lambda: {'count': 0, 'types': Counter(), 'values': []})

        for entry in pvf.indexes:
            if sample_count >= 20:
                break

            if not entry.name.endswith(ext):
                continue

            data = pvf.extract_file(entry)
            if not data or len(data) < 4:
                continue
            if data[0] != 0xB0 or data[1] != 0xD0:
                continue

            # 检查是否有这个嵌套键
            has_nested = False
            i = 2
            while i < len(data) - 6:
                if data[i] == 0x05:
                    key = struct.unpack_from('<H', data, i + 1)[0]
                    if key == nested_key:
                        has_nested = True
                        break
                i += 1

            if not has_nested:
                continue

            sample_count += 1

            if sample_count <= 3:
                print(f"\n文件: {entry.name.split('/')[-1]}")
                inner_keys = analyze_nested_block(data, nested_key)

                for k, v in inner_keys.items():
                    fk = format_keys[k]
                    fk['count'] += v['count']
                    for t, c in v['types'].items():
                        fk['types'][t] += c
                    fk['values'].extend(v['values'][:3])

        # 汇总该格式的内部键
        if format_keys:
            print(f"\n{ext} 格式内部键汇总 ({sample_count} 个文件):")
            print(f"{'Key':>8} {'Count':>6} {'Type':>8} {'Sample'}")
            print('-'*50)

            for key in sorted(format_keys.keys(), key=lambda k: -format_keys[k]['count'])[:20]:
                info = format_keys[key]
                main_type = info['types'].most_common(1)[0][0] if info['types'] else "?"
                vals = str(info['values'][:2])[:25]
                tag = st.get_tag(key)
                tag_str = f"  {tag.encode('ascii', errors='replace').decode('ascii')[:15]}" if tag else ""
                print(f"0x{key:04X}  {info['count']:>6}  {main_type:>8}  {vals}{tag_str}")

            all_results[ext] = format_keys

    pvf.close()

    # 输出建议的新键映射
    print("\n" + "=" * 60)
    print("建议添加到语义映射的内部键")
    print("=" * 60)

    print("\nNESTED_INNER_KEYS = {")
    for ext, keys in all_results.items():
        if keys:
            print(f"    # {ext} 嵌套内部键")
            for key in sorted(keys.keys(), key=lambda k: -keys[k]['count'])[:10]:
                info = keys[key]
                main_type = info['types'].most_common(1)[0][0] if info['types'] else "?"
                print(f"    0x{key:04X}: \"inner_{key:04x}\",  # {main_type}, count={info['count']}")
    print("}")


if __name__ == "__main__":
    main()
