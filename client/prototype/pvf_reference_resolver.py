#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF引用解析器
=============
解析文件间的引用关系，构建资源依赖图
"""

import sys
import os
from dataclasses import dataclass, field
from typing import Dict, List, Set, Optional, Tuple
from collections import defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType
from pvf_semantic_keys import get_semantic_name


@dataclass
class FileReference:
    """文件引用"""
    source_key: int           # 引用所在的键
    source_key_name: str      # 键的语义名称
    target_index: int         # 目标文件索引
    target_path: str          # 目标文件路径


@dataclass
class FileNode:
    """文件节点（资源依赖图中的节点）"""
    path: str
    index: int
    extension: str
    outgoing_refs: List[FileReference] = field(default_factory=list)  # 我引用的文件
    incoming_refs: List[Tuple[str, int]] = field(default_factory=list)  # 引用我的文件


class ReferenceResolver:
    """引用解析器"""

    def __init__(self, pvf_path: str):
        self.pvf = PVFReader(pvf_path)
        self.pvf.open()
        self.pvf.parse_index(max_entries=300000)

        # 构建索引映射
        self._index_to_path = {i: e.name for i, e in enumerate(self.pvf.indexes)}
        self._path_to_index = {e.name.lower(): i for i, e in enumerate(self.pvf.indexes)}

        # 缓存已解析的文件节点
        self._nodes: Dict[str, FileNode] = {}

    def close(self):
        self.pvf.close()

    def get_file_count(self) -> int:
        return len(self.pvf.indexes)

    def resolve_index(self, index: int) -> Optional[str]:
        """将索引解析为文件路径"""
        return self._index_to_path.get(index)

    def get_file_index(self, path: str) -> Optional[int]:
        """获取文件的索引"""
        return self._path_to_index.get(path.lower())

    def parse_file_references(self, path: str) -> FileNode:
        """解析文件的所有引用"""
        if path in self._nodes:
            return self._nodes[path]

        # 查找文件
        index = self.get_file_index(path)
        if index is None:
            return FileNode(path=path, index=-1, extension="")

        entry = self.pvf.indexes[index]
        ext = os.path.splitext(path)[1].lower()
        node = FileNode(path=path, index=index, extension=ext)

        # 提取文件数据
        data = self.pvf.extract_file(entry)
        if not data or len(data) < 2:
            self._nodes[path] = node
            return node

        if data[0] != 0xB0 or data[1] != 0xD0:
            self._nodes[path] = node
            return node

        # 解析引用
        try:
            container = parse_pvf_data(data)
            for e in container.entries:
                if e.type == PVFValueType.REFERENCE and e.key > 0:
                    ref_index = e.value
                    ref_path = self.resolve_index(ref_index)
                    if ref_path:
                        key_name = get_semantic_name(ext, e.key)
                        ref = FileReference(
                            source_key=e.key,
                            source_key_name=key_name,
                            target_index=ref_index,
                            target_path=ref_path
                        )
                        node.outgoing_refs.append(ref)
        except:
            pass

        self._nodes[path] = node
        return node

    def get_dependencies(self, path: str, depth: int = 1) -> Dict[str, Set[str]]:
        """获取文件的依赖树（指定深度）"""
        result = defaultdict(set)
        visited = set()

        def visit(p: str, current_depth: int):
            if p in visited or current_depth > depth:
                return
            visited.add(p)

            node = self.parse_file_references(p)
            for ref in node.outgoing_refs:
                result[p].add(ref.target_path)
                if current_depth < depth:
                    visit(ref.target_path, current_depth + 1)

        visit(path, 1)
        return dict(result)

    def get_dependents(self, path: str, sample_size: int = 5000) -> List[str]:
        """获取引用此文件的其他文件"""
        target_index = self.get_file_index(path)
        if target_index is None:
            return []

        dependents = []
        for i, entry in enumerate(self.pvf.indexes[:sample_size]):
            data = self.pvf.extract_file(entry)
            if not data or len(data) < 2:
                continue
            if data[0] != 0xB0 or data[1] != 0xD0:
                continue

            try:
                container = parse_pvf_data(data)
                for e in container.entries:
                    if e.type == PVFValueType.REFERENCE and e.value == target_index:
                        dependents.append(entry.name)
                        break
            except:
                pass

        return dependents


def demo():
    """演示引用解析"""
    print("=" * 70)
    print("PVF Reference Resolver Demo")
    print("=" * 70)

    resolver = ReferenceResolver('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    print(f"Total files: {resolver.get_file_count()}")

    # 示例1：解析一个怪物文件的引用
    print("\n--- Monster File References ---")
    mob_path = "monster/goblin/goliath.mob"
    node = resolver.parse_file_references(mob_path)
    print(f"File: {mob_path}")
    print(f"Outgoing references: {len(node.outgoing_refs)}")
    for ref in node.outgoing_refs[:10]:
        print(f"  [{ref.source_key_name}] -> {ref.target_path[:50]}")

    # 示例2：解析一个装备文件的引用
    print("\n--- Equipment File References ---")
    equ_path = "equipment/character/swordman/weapon/katana/lkatana.equ"
    # 找一个实际存在的装备文件
    for entry in resolver.pvf.indexes[:1000]:
        if entry.name.endswith('.equ') and 'weapon' in entry.name:
            equ_path = entry.name
            break

    node = resolver.parse_file_references(equ_path)
    print(f"File: {equ_path}")
    print(f"Outgoing references: {len(node.outgoing_refs)}")
    for ref in node.outgoing_refs[:10]:
        ref_ext = os.path.splitext(ref.target_path)[1]
        print(f"  [{ref.source_key_name}] -> ({ref_ext}) {ref.target_path.split('/')[-1][:40]}")

    # 示例3：依赖树
    print("\n--- Dependency Tree (depth=2) ---")
    deps = resolver.get_dependencies(equ_path, depth=2)
    for src, targets in list(deps.items())[:5]:
        print(f"  {src.split('/')[-1][:30]}:")
        for t in list(targets)[:3]:
            print(f"    -> {t.split('/')[-1][:40]}")

    resolver.close()
    print("\nDone!")


if __name__ == "__main__":
    demo()
