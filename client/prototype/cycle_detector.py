#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF循环依赖检测器
=================
检测PVF文件之间的循环引用关系
"""

import sys
import os
from collections import defaultdict
from typing import Dict, List, Set, Tuple
from pathlib import Path

sys.path.insert(0, os.path.dirname(__file__))
from pvf_reader import PVFReader
from pvf_data_parser import parse_pvf_data, PVFValueType


class CycleDetector:
    """循环依赖检测器"""

    def __init__(self, pvf: PVFReader):
        self.pvf = pvf
        # 文件索引 -> 文件名
        self.index_to_name = {}
        # 文件名 -> 文件索引
        self.name_to_index = {}
        # 依赖图: 文件索引 -> [被引用的文件索引列表]
        self.graph = defaultdict(set)
        # 反向图: 文件索引 -> [引用它的文件索引列表]
        self.reverse_graph = defaultdict(set)

    def build_index_map(self):
        """构建索引映射"""
        for i, entry in enumerate(self.pvf.indexes):
            self.index_to_name[i] = entry.name
            self.name_to_index[entry.name] = i

    def build_dependency_graph(self, max_files: int = 50000):
        """构建依赖图"""
        print(f"构建依赖图 (最多 {max_files} 文件)...")

        count = 0
        ref_count = 0

        for i, entry in enumerate(self.pvf.indexes):
            if count >= max_files:
                break

            data = self.pvf.extract_file(entry)
            if not data or len(data) < 4:
                continue
            if data[0] != 0xB0 or data[1] != 0xD0:
                continue

            try:
                container = parse_pvf_data(data)
                for e in container.entries:
                    if e.type == PVFValueType.REFERENCE:
                        target_idx = e.value
                        if target_idx in self.index_to_name:
                            self.graph[i].add(target_idx)
                            self.reverse_graph[target_idx].add(i)
                            ref_count += 1
            except:
                pass

            count += 1
            if count % 10000 == 0:
                print(f"  已处理 {count} 文件, {ref_count} 引用...")

        print(f"完成: {count} 文件, {ref_count} 引用, {len(self.graph)} 源节点")

    def find_cycles_dfs(self, max_cycles: int = 100) -> List[List[int]]:
        """使用DFS查找循环"""
        print(f"\n检测循环引用...")

        cycles = []
        visited = set()
        rec_stack = set()
        path = []

        def dfs(node: int) -> bool:
            visited.add(node)
            rec_stack.add(node)
            path.append(node)

            for neighbor in self.graph.get(node, []):
                if neighbor not in visited:
                    if dfs(neighbor):
                        return True
                elif neighbor in rec_stack:
                    # 找到循环
                    cycle_start = path.index(neighbor)
                    cycle = path[cycle_start:] + [neighbor]
                    cycles.append(cycle)
                    if len(cycles) >= max_cycles:
                        return True

            path.pop()
            rec_stack.remove(node)
            return False

        for node in list(self.graph.keys()):
            if node not in visited:
                if dfs(node):
                    break

        return cycles

    def find_self_references(self) -> List[int]:
        """查找自引用"""
        self_refs = []
        for node, targets in self.graph.items():
            if node in targets:
                self_refs.append(node)
        return self_refs

    def find_mutual_references(self) -> List[Tuple[int, int]]:
        """查找互相引用 (A->B 且 B->A)"""
        mutual = []
        checked = set()

        for a, targets in self.graph.items():
            for b in targets:
                if (b, a) in checked:
                    continue
                if a in self.graph.get(b, set()):
                    mutual.append((a, b))
                    checked.add((a, b))

        return mutual

    def get_strongly_connected_components(self) -> List[List[int]]:
        """使用Kosaraju算法查找强连通分量"""
        # 第一次DFS，按完成时间排序
        visited = set()
        finish_order = []

        def dfs1(node):
            visited.add(node)
            for neighbor in self.graph.get(node, []):
                if neighbor not in visited:
                    dfs1(neighbor)
            finish_order.append(node)

        for node in self.graph.keys():
            if node not in visited:
                dfs1(node)

        # 第二次DFS，在反向图上按完成时间逆序遍历
        visited.clear()
        sccs = []

        def dfs2(node, component):
            visited.add(node)
            component.append(node)
            for neighbor in self.reverse_graph.get(node, []):
                if neighbor not in visited:
                    dfs2(neighbor, component)

        for node in reversed(finish_order):
            if node not in visited:
                component = []
                dfs2(node, component)
                if len(component) > 1:  # 只保留大于1的强连通分量
                    sccs.append(component)

        return sccs

    def analyze(self):
        """完整分析"""
        self.build_index_map()
        self.build_dependency_graph()

        print("\n" + "=" * 60)
        print("循环依赖分析结果")
        print("=" * 60)

        # 1. 自引用
        self_refs = self.find_self_references()
        print(f"\n[1] 自引用: {len(self_refs)} 个")
        for idx in self_refs[:10]:
            name = self.index_to_name.get(idx, f"idx_{idx}")
            print(f"  {name}")
        if len(self_refs) > 10:
            print(f"  ... 还有 {len(self_refs) - 10} 个")

        # 2. 互相引用
        mutual = self.find_mutual_references()
        print(f"\n[2] 互相引用: {len(mutual)} 对")
        for a, b in mutual[:10]:
            name_a = self.index_to_name.get(a, f"idx_{a}")
            name_b = self.index_to_name.get(b, f"idx_{b}")
            print(f"  {Path(name_a).name} <-> {Path(name_b).name}")
        if len(mutual) > 10:
            print(f"  ... 还有 {len(mutual) - 10} 对")

        # 3. 强连通分量 (循环)
        sccs = self.get_strongly_connected_components()
        print(f"\n[3] 强连通分量 (循环组): {len(sccs)} 个")

        # 按大小排序
        sccs.sort(key=len, reverse=True)

        for i, scc in enumerate(sccs[:5]):
            print(f"\n  SCC #{i+1} ({len(scc)} 节点):")
            # 显示格式分布
            ext_count = defaultdict(int)
            for idx in scc:
                name = self.index_to_name.get(idx, "")
                ext = Path(name).suffix.lower()
                ext_count[ext] += 1

            for ext, cnt in sorted(ext_count.items(), key=lambda x: -x[1])[:5]:
                print(f"    {ext}: {cnt}")

            # 显示部分文件名
            for idx in scc[:5]:
                name = self.index_to_name.get(idx, f"idx_{idx}")
                print(f"    - {Path(name).name}")
            if len(scc) > 5:
                print(f"    ... 还有 {len(scc) - 5} 个")

        if len(sccs) > 5:
            print(f"\n  ... 还有 {len(sccs) - 5} 个强连通分量")

        # 4. 统计
        print("\n" + "=" * 60)
        print("统计摘要")
        print("=" * 60)
        print(f"总节点数: {len(self.graph)}")
        print(f"总边数: {sum(len(v) for v in self.graph.values())}")
        print(f"自引用: {len(self_refs)}")
        print(f"互相引用: {len(mutual)}")
        print(f"强连通分量: {len(sccs)}")
        if sccs:
            print(f"最大SCC大小: {len(sccs[0])}")

        return {
            'self_refs': self_refs,
            'mutual_refs': mutual,
            'sccs': sccs,
        }


def main():
    print("=" * 60)
    print("PVF循环依赖检测")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=100000)

    detector = CycleDetector(pvf)
    results = detector.analyze()

    pvf.close()

    # 保存结果
    with open('cycle_detection_result.txt', 'w', encoding='utf-8') as f:
        f.write("PVF循环依赖检测结果\n")
        f.write("=" * 50 + "\n\n")

        f.write(f"自引用: {len(results['self_refs'])} 个\n")
        for idx in results['self_refs'][:20]:
            name = detector.index_to_name.get(idx, f"idx_{idx}")
            f.write(f"  {name}\n")

        f.write(f"\n互相引用: {len(results['mutual_refs'])} 对\n")
        for a, b in results['mutual_refs'][:20]:
            name_a = detector.index_to_name.get(a, f"idx_{a}")
            name_b = detector.index_to_name.get(b, f"idx_{b}")
            f.write(f"  {name_a} <-> {name_b}\n")

        f.write(f"\n强连通分量: {len(results['sccs'])} 个\n")
        for i, scc in enumerate(results['sccs'][:10]):
            f.write(f"\nSCC #{i+1} ({len(scc)} 节点):\n")
            for idx in scc[:10]:
                name = detector.index_to_name.get(idx, f"idx_{idx}")
                f.write(f"  {name}\n")
            if len(scc) > 10:
                f.write(f"  ... 还有 {len(scc) - 10} 个\n")

    print("\n结果已保存到 cycle_detection_result.txt")


if __name__ == "__main__":
    main()
