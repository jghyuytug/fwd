#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Debug TIL format"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

count = 0
for entry in pvf.indexes:
    if entry.name.endswith('.til'):
        data = pvf.extract_file(entry)
        if data:
            print(f"File: {entry.name}")
            print(f"Size: {len(data)} bytes")
            print(f"First 32 bytes: {data[:32].hex()}")
            print(f"Magic check: 0x{data[0]:02X} 0x{data[1]:02X}")
            print()
            count += 1
            if count >= 5:
                break

pvf.close()
