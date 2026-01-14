#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ANI文件结构分析
===============
分析144,647个ANI文件的内部二进制结构
"""

import sys
import os
import struct
from collections import Counter, defaultdict
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader


@dataclass
class ANIHeader:
    """ANI文件头"""
    type_code: int = 0
    version: int = 0
    path_length: int = 0
    image_path: str = ""
    extra_data: bytes = b""


def analyze_ani_header(data: bytes) -> Optional[ANIHeader]:
    """解析ANI文件头"""
    if len(data) < 4:
        return None

    header = ANIHeader()

    # 读取类型码和版本
    header.type_code = struct.unpack_from('<H', data, 0)[0]
    header.version = struct.unpack_from('<H', data, 2)[0]

    # 根据类型码判断结构
    if header.type_code == 0xD0B0:
        # 这是PVF格式，不是ANI
        return None

    # 尝试读取路径长度和路径
    if len(data) >= 8:
        header.path_length = struct.unpack_from('<I', data, 4)[0]

        if header.path_length > 0 and header.path_length < 1000:
            if len(data) >= 8 + header.path_length:
                try:
                    header.image_path = data[8:8+header.path_length].decode('utf-8', errors='replace')
                except:
                    header.image_path = ""

    return header


def main():
    print("=" * 70)
    print("ANI File Structure Analysis")
    print("=" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 统计
    type_codes = Counter()
    versions = Counter()
    path_patterns = Counter()
    size_ranges = Counter()
    header_samples = defaultdict(list)

    count = 0
    pvf_format = 0
    empty_files = 0

    for entry in pvf.indexes:
        if not entry.name.endswith('.ani'):
            continue

        data = pvf.extract_file(entry)
        if not data:
            empty_files += 1
            continue

        if len(data) < 2:
            continue

        # 检查是否为PVF格式
        if data[0] == 0xB0 and data[1] == 0xD0:
            pvf_format += 1
            continue

        # 解析头部
        header = analyze_ani_header(data)
        if header:
            type_codes[header.type_code] += 1
            versions[header.version] += 1

            # 分析路径模式
            if header.image_path:
                # 提取扩展名
                if '.' in header.image_path:
                    ext = header.image_path.split('.')[-1].lower()[:10]
                    path_patterns[f".{ext}"] += 1
                # 提取目录
                if '/' in header.image_path:
                    parts = header.image_path.split('/')
                    if len(parts) > 1:
                        path_patterns[parts[0]] += 1

            # 采样
            if len(header_samples[header.type_code]) < 3:
                header_samples[header.type_code].append({
                    'file': entry.name,
                    'size': len(data),
                    'path': header.image_path[:60] if header.image_path else "",
                    'version': header.version,
                })

        # 大小范围
        size = len(data)
        if size < 100:
            size_ranges['<100'] += 1
        elif size < 500:
            size_ranges['100-500'] += 1
        elif size < 1000:
            size_ranges['500-1K'] += 1
        elif size < 5000:
            size_ranges['1K-5K'] += 1
        else:
            size_ranges['>5K'] += 1

        count += 1
        if count >= 50000:
            break

    pvf.close()

    # 输出结果
    print(f"\nAnalyzed {count} ANI files")
    print(f"PVF format (0xD0B0): {pvf_format}")
    print(f"Empty files: {empty_files}")
    print(f"Custom binary: {count - pvf_format}")

    print("\n" + "-" * 70)
    print("Type Codes Distribution")
    print("-" * 70)
    for code, cnt in type_codes.most_common(20):
        pct = cnt / count * 100
        print(f"  0x{code:04X}: {cnt:>7,} ({pct:5.1f}%)")

    print("\n" + "-" * 70)
    print("Version Distribution")
    print("-" * 70)
    for ver, cnt in versions.most_common(10):
        print(f"  {ver}: {cnt:>7,}")

    print("\n" + "-" * 70)
    print("File Size Ranges")
    print("-" * 70)
    for range_name, cnt in sorted(size_ranges.items()):
        print(f"  {range_name:>10}: {cnt:>7,}")

    print("\n" + "-" * 70)
    print("Path Patterns in ANI")
    print("-" * 70)
    for pattern, cnt in path_patterns.most_common(20):
        print(f"  {pattern:>20}: {cnt:>7,}")

    print("\n" + "-" * 70)
    print("Sample Files by Type Code")
    print("-" * 70)
    for code in sorted(header_samples.keys())[:10]:
        samples = header_samples[code]
        print(f"\nType 0x{code:04X}:")
        for s in samples:
            path_safe = s['path'].encode('ascii', 'replace').decode('ascii')
            print(f"  {s['file'].split('/')[-1][:30]:32} size={s['size']:>5} path={path_safe[:40]}")


def analyze_type_1_structure():
    """详细分析Type 0x0001的结构"""
    print("\n" + "=" * 70)
    print("Detailed Analysis: Type 0x0001 (Most Common)")
    print("=" * 70)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    samples = []
    for entry in pvf.indexes:
        if not entry.name.endswith('.ani'):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 10:
            continue

        type_code = struct.unpack_from('<H', data, 0)[0]
        if type_code == 0x0001:
            samples.append((entry.name, data))
            if len(samples) >= 5:
                break

    pvf.close()

    for name, data in samples:
        print(f"\nFile: {name.split('/')[-1]}")
        print(f"Size: {len(data)} bytes")
        print(f"Header hex: {data[:32].hex()}")

        # 尝试解析
        type_code = struct.unpack_from('<H', data, 0)[0]
        print(f"Type: 0x{type_code:04X}")

        if len(data) >= 4:
            field2 = struct.unpack_from('<H', data, 2)[0]
            print(f"Field2: 0x{field2:04X} ({field2})")

        if len(data) >= 8:
            field3 = struct.unpack_from('<I', data, 4)[0]
            print(f"Field3: 0x{field3:08X} ({field3})")

        # 寻找路径字符串
        for i in range(min(100, len(data) - 10)):
            try:
                # 查找可能的路径开头
                chunk = data[i:i+20]
                if b'/' in chunk or b'sprite' in chunk.lower() or b'img' in chunk.lower():
                    # 向后找字符串结束
                    end = i
                    while end < len(data) and data[end] != 0 and end - i < 200:
                        end += 1
                    if end - i > 5:
                        path = data[i:end].decode('utf-8', errors='replace')
                        if '/' in path or '.' in path:
                            print(f"Possible path at offset {i}: {path[:60]}")
                            break
            except:
                pass


if __name__ == "__main__":
    main()
    analyze_type_1_structure()
