#!/usr/bin/env python3
"""找出最后68个未解析的文件"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader

pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
pvf.open()
pvf.parse_index(max_entries=300000)

# 已支持的格式
supported = {'.skl', '.mob', '.equ', '.ani', '.act', '.atk', '.stk', '.qst', '.ai',
             '.key', '.obj', '.til', '.ptl', '.map', '.dgn', '.als', '.aic', '.rep',
             '.ui', '.npc', '.cre', '.msn', '.etc', '.wrd', '.lst', '.apd', '.gdata',
             '.shp', '.tbl', '.chr', '.wdm', '.sd', '.twn', '.lay', '.cbt', '.ora',
             '.hsp', '.mm', '.dat', '.exj', '.rgn', '.vm', '.pet', '.glist', '.tlk',
             '.evn', '.stm', '.co', '.evt', '.gdata_', '.ani__', '.ani_new', '.str',
             '.nut', '.txt', '.rtf', '.kor', '.jap', '.chi', '.jpn', '.bak', '.img',
             '.exe', '.bin', '.log', '.dl'}

missing = []
for entry in pvf.indexes:
    ext = os.path.splitext(entry.name)[1].lower()
    if ext and ext not in supported:
        missing.append((ext, entry.name))

pvf.close()

print(f"Found {len(missing)} unsupported files:")
from collections import Counter
ext_counts = Counter(ext for ext, _ in missing)
for ext, count in ext_counts.most_common():
    print(f"  {ext}: {count}")
    samples = [name for e, name in missing if e == ext][:2]
    for s in samples:
        print(f"    - {s.split('/')[-1]}")
