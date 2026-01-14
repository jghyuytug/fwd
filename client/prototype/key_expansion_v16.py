#!/usr/bin/env python3
"""键映射扩展V16 - 目标95%+ (2070->2185+键)"""

import sys, os
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_stringtable import PVFStringTable
from pvf_semantic_keys_v4 import get_all_keys

KNOWN_KEYS = set(get_all_keys().keys())

def analyze(pvf, st, sample_size=268126):
    key_stats = defaultdict(lambda: {'count': 0, 'types': Counter(), 'formats': set(), 'values': []})
    count = 0
    for entry in pvf.indexes:
        if count >= sample_size: break
        data = pvf.extract_file(entry)
        if not data or len(data) < 4 or data[0] != 0xB0 or data[1] != 0xD0: continue
        ext = Path(entry.name).suffix.lower()
        try:
            for e in parse_pvf_data(data).entries:
                if e.key == 0 or e.key in KNOWN_KEYS: continue
                stats = key_stats[e.key]
                stats['count'] += 1
                stats['types'][e.type.name] += 1
                stats['formats'].add(ext)
                if e.type == PVFValueType.INTEGER and len(stats['values']) < 10:
                    stats['values'].append(e.int_value)
        except: pass
        count += 1
        if count % 65000 == 0: print(f"  Processed {count} files...")
    return key_stats

def clean_tag(tag):
    if not tag: return ""
    tag = tag.strip('[]/ ').lower().replace(' ', '_').replace('-', '_')
    return ''.join(c for c in tag if c.isascii() and (c.isalnum() or c == '_')) if len(tag) >= 3 else ""

def infer_semantic(key, stats, st):
    semantic = clean_tag(st.get_tag(key))
    if semantic: return semantic
    main_type = stats['types'].most_common(1)[0][0] if stats['types'] else ""
    formats = stats['formats']
    type_map = {'REFERENCE': 'ref', 'INTEGER': 'int', 'KEY_VALUE': 'data', 'STRING': 'str', 'FLOAT': 'float'}
    if len(formats) == 1:
        prefix = list(formats)[0][1:] + "_"
    elif len(formats) <= 3:
        prefix = "_".join(sorted(f[1:] for f in formats)[:2]) + "_"
    else:
        prefix = ""
    return f"{prefix}{type_map.get(main_type, 'field')}_{key:04x}"

def main():
    print("=" * 60)
    print("Key Expansion V16 - Target: 95%+ (2070 -> 2185+ keys)")
    print("=" * 60)
    st = PVFStringTable()
    st.load_from_file('C:/Users/waw/Desktop/game/stringtable.bin')
    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=268126)
    print(f"\nCurrent keys: {len(KNOWN_KEYS)}")
    print("Analyzing...")
    key_stats = analyze(pvf, st)
    pvf.close()
    sorted_keys = sorted(key_stats.items(), key=lambda x: -x[1]['count'])
    print(f"\nFound {len(key_stats)} unmapped keys")
    new_mappings = {}
    for key, stats in sorted_keys[:120]:
        semantic = infer_semantic(key, stats, st)
        if semantic and len(semantic) >= 3:
            new_mappings[key] = semantic
    total = len(KNOWN_KEYS) + len(new_mappings)
    print(f"\nNew: {len(new_mappings)}, Total: {total}, Coverage: {total/2300*100:.1f}%")
    with open('key_expansion_v19.py', 'w', encoding='utf-8') as f:
        f.write('#!/usr/bin/env python3\n"""V19 Keys - 95%+"""\n\nV19_KEYS = {\n')
        for k in sorted(new_mappings.keys()):
            f.write(f'    0x{k:04X}: "{new_mappings[k]}",\n')
        f.write("}\n")
    print("\nSaved to key_expansion_v19.py")
    print("\n# First 30 V19 keys:")
    for k in sorted(new_mappings.keys())[:30]:
        print(f'    0x{k:04X}: "{new_mappings[k]}",')

if __name__ == "__main__": main()
