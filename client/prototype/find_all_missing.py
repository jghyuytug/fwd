#!/usr/bin/env python3
"""找出所有无法解析的文件"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

# 统计扩展名
from collections import Counter
extensions = Counter()
no_ext = []

for entry in pvf.indexes:
    ext = os.path.splitext(entry.name)[1].lower()
    if ext:
        extensions[ext] += 1
    else:
        no_ext.append(entry.name)

pvf.close()

print(f"Files without extension: {len(no_ext)}")
for f in no_ext[:20]:
    print(f"  {f}")

print(f"\nTotal extensions: {len(extensions)}")
print(f"Total files: {sum(extensions.values()) + len(no_ext)}")
