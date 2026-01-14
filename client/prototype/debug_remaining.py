#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Debug remaining formats"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

# 检查失败的格式
formats_to_check = ['.str', '.nut', '.txt', '.rtf']

for fmt in formats_to_check:
    print(f"\n{'='*60}")
    print(f"Checking {fmt} files")
    print('='*60)
    count = 0
    for entry in pvf.indexes:
        if entry.name.endswith(fmt):
            data = pvf.extract_file(entry)
            if data:
                print(f"File: {entry.name.split('/')[-1]}")
                print(f"Size: {len(data)} bytes")
                if len(data) >= 2:
                    print(f"First 2 bytes: 0x{data[0]:02X} 0x{data[1]:02X}")
                    is_pvf = data[0] == 0xB0 and data[1] == 0xD0
                    print(f"Is PVF format: {is_pvf}")
                # 尝试显示文本内容
                try:
                    text = data[:100].decode('utf-8', errors='replace')
                    print(f"Content preview: {repr(text[:80])}")
                except:
                    pass
                print()
                count += 1
                if count >= 3:
                    break

pvf.close()
