#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF数据提取演示
从PVF中提取并解析技能、怪物、装备等数据
"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from typing import Dict, List, Any


def extract_and_parse(pvf: PVFReader, path: str) -> Dict[int, Any]:
    """提取并解析PVF文件"""
    entry = pvf.get_file(path)
    if not entry:
        return None

    data = pvf.extract_file(entry)
    if not data:
        return None

    container = parse_pvf_data(data)

    # 转换为字典
    result = {
        '_path': path,
        '_size': len(data),
        '_entries': len(container.entries),
    }

    # 按键分组整数值
    for e in container.entries:
        if e.key > 0:
            key_hex = f"0x{e.key:04X}"
            if e.type == PVFValueType.INTEGER:
                result[key_hex] = e.value
            elif e.type == PVFValueType.FLOAT:
                result[key_hex] = round(e.value, 4)
            elif e.type == PVFValueType.REFERENCE:
                result[f"{key_hex}_ref"] = f"0x{e.value:04X}"
            elif e.type == PVFValueType.STRING:
                result[f"{key_hex}_str"] = e.value

    return result


def list_skills(pvf: PVFReader, job: str = "fighter", limit: int = 5) -> List[Dict]:
    """列出技能"""
    skills = []
    prefix = f"skill/{job}/"

    for entry in pvf.indexes:
        if entry.name.startswith(prefix) and entry.name.endswith('.skl'):
            data = extract_and_parse(pvf, entry.name)
            if data:
                skills.append(data)
                if len(skills) >= limit:
                    break

    return skills


def list_monsters(pvf: PVFReader, limit: int = 5) -> List[Dict]:
    """列出怪物"""
    monsters = []

    for entry in pvf.indexes:
        if entry.name.endswith('.mob'):
            data = extract_and_parse(pvf, entry.name)
            if data:
                monsters.append(data)
                if len(monsters) >= limit:
                    break

    return monsters


def main():
    pvf_path = "C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf"

    print("=" * 70)
    print("PVF Data Extraction Demo")
    print("=" * 70)

    pvf = PVFReader(pvf_path)
    if not pvf.open():
        print("Failed to open PVF")
        return 1

    print(f"PVF: {pvf.header.index_count} files")

    if not pvf.parse_index(max_entries=300000):
        print("Failed to parse index")
        return 1

    print(f"Parsed: {len(pvf.indexes)} entries")
    print()

    # 提取技能数据
    print("-" * 70)
    print("Fighter Skills:")
    print("-" * 70)
    skills = list_skills(pvf, "fighter", 5)
    for skill in skills:
        print(f"\n{skill['_path']}:")
        for k, v in skill.items():
            if not k.startswith('_'):
                print(f"  {k}: {v}")

    # 提取怪物数据
    print()
    print("-" * 70)
    print("Monsters:")
    print("-" * 70)
    monsters = list_monsters(pvf, 5)
    for monster in monsters:
        print(f"\n{monster['_path']}:")
        # 只显示前10个属性
        count = 0
        for k, v in monster.items():
            if not k.startswith('_'):
                print(f"  {k}: {v}")
                count += 1
                if count >= 10:
                    print(f"  ... ({len(monster) - 10 - 3} more)")
                    break

    # 统计文件类型
    print()
    print("-" * 70)
    print("File Type Statistics:")
    print("-" * 70)
    types = {}
    for entry in pvf.indexes:
        ext = entry.name.split('.')[-1].lower() if '.' in entry.name else 'no_ext'
        types[ext] = types.get(ext, 0) + 1

    top = sorted(types.items(), key=lambda x: -x[1])[:15]
    for ext, count in top:
        print(f"  .{ext}: {count:,}")

    pvf.close()
    print()
    print("Done!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
