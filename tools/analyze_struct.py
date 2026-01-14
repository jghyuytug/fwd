#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
结构体偏移量分析工具
分析代码中的偏移量访问模式，推断结构体字段
"""

import re
import sys
from collections import defaultdict

class StructAnalyzer:
    def __init__(self):
        self.offsets = defaultdict(lambda: {
            'reads': [],
            'writes': [],
            'types': set(),
            'contexts': []
        })

    def analyze_line(self, line, line_no):
        """分析一行代码中的偏移量访问"""
        # 匹配模式：*(_TYPE *)(var + offset)
        pattern = r'\*\((_[A-Z]+) \*\)\(([a-z_]\w*) \+ (\d+)\)'

        for match in re.finditer(pattern, line):
            type_name = match.group(1)  # _DWORD, _BYTE, etc.
            var_name = match.group(2)   # a1, inventory, this, etc.
            offset = int(match.group(3))

            # 判断是读还是写
            full_match = match.group(0)
            # 检查是否是赋值的左侧
            if re.search(rf'{re.escape(full_match)}\s*=', line):
                self.offsets[offset]['writes'].append((line_no, var_name, type_name))
            else:
                self.offsets[offset]['reads'].append((line_no, var_name, type_name))

            self.offsets[offset]['types'].add(type_name)
            self.offsets[offset]['contexts'].append(line.strip())

    def analyze_file(self, filename, class_filter=None, max_lines=500000):
        """分析文件"""
        print(f"[*] 分析文件: {filename}")
        if class_filter:
            print(f"[*] 筛选类: {class_filter}")

        in_target_class = False
        current_function = None

        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            for line_no, line in enumerate(f, 1):
                # 如果指定了类过滤
                if class_filter:
                    # 检测函数定义
                    if f'{class_filter}::' in line and '(' in line:
                        in_target_class = True
                        current_function = line.strip()
                    elif line.strip().startswith('}') and in_target_class:
                        in_target_class = False

                    if not in_target_class:
                        continue

                self.analyze_line(line, line_no)

                if line_no % 100000 == 0:
                    print(f"    处理 {line_no:,} 行...")

                if max_lines and line_no >= max_lines:
                    break

        print(f"[+] 分析完成")

    def infer_field_type(self, offset_data):
        """推断字段类型"""
        types = offset_data['types']

        # 如果所有访问都是同一类型，直接返回
        if len(types) == 1:
            type_name = list(types)[0]
            return self.convert_type(type_name)

        # 如果有多种类型，选择最常用的
        type_counts = defaultdict(int)
        for _, _, type_name in offset_data['reads'] + offset_data['writes']:
            type_counts[type_name] += 1

        most_common = max(type_counts.items(), key=lambda x: x[1])[0]
        return self.convert_type(most_common)

    def convert_type(self, hex_type):
        """将Hex-Rays类型转换为C类型"""
        mapping = {
            '_BYTE': 'uint8_t',
            '_WORD': 'uint16_t',
            '_DWORD': 'uint32_t',
            '_QWORD': 'uint64_t',
            '_BOOL4': 'int32_t'
        }
        return mapping.get(hex_type, 'void*')

    def infer_field_name(self, offset, offset_data):
        """推断字段名称"""
        # 基于上下文推断
        contexts = offset_data['contexts']

        # 检查常见模式
        context_str = ' '.join(contexts[:10])  # 看前10个上下文

        # 金钱相关
        if 'money' in context_str.lower() or 'gold' in context_str.lower():
            return 'money'

        # 等级相关
        if 'level' in context_str.lower() or 'lev' in context_str.lower():
            return 'level'

        # HP/MP相关
        if 'hp' in context_str.lower():
            return 'hp'
        if 'mp' in context_str.lower():
            return 'mp'

        # 用户/角色相关
        if 'user' in context_str.lower() or 'CUser' in context_str:
            return 'owner' if offset == 0 else 'user_ptr'

        # 数量相关
        if 'count' in context_str.lower():
            return 'count'

        # 默认命名
        return f'field_{offset:04x}'

    def generate_struct_definition(self, struct_name='CInventory'):
        """生成结构体定义"""
        print(f"\n{'='*60}")
        print(f"结构体定义: {struct_name}")
        print(f"{'='*60}\n")

        # 按偏移量排序
        sorted_offsets = sorted(self.offsets.items())

        # 生成C++类定义
        print(f"class {struct_name} {{")
        print("public:")

        last_offset = 0
        for offset, data in sorted_offsets:
            if not data['reads'] and not data['writes']:
                continue

            # 如果偏移量之间有gap，添加padding
            if offset > last_offset:
                gap = offset - last_offset
                if gap > 0:
                    print(f"    uint8_t _padding_{last_offset:04x}[{gap}];  // padding")

            field_type = self.infer_field_type(data)
            field_name = self.infer_field_name(offset, data)

            reads = len(data['reads'])
            writes = len(data['writes'])

            print(f"    {field_type:12s} {field_name:20s};  // offset {offset:4d} (0x{offset:04x}) - R:{reads} W:{writes}")

            last_offset = offset + self.get_type_size(field_type)

        print("};")
        print()

    def get_type_size(self, type_name):
        """获取类型大小"""
        sizes = {
            'uint8_t': 1,
            'uint16_t': 2,
            'uint32_t': 4,
            'int32_t': 4,
            'uint64_t': 8,
            'void*': 4,  # 32-bit
        }
        return sizes.get(type_name, 4)

    def print_statistics(self):
        """打印统计信息"""
        print(f"\n[统计信息]")
        print(f"  发现偏移量: {len(self.offsets)} 个")

        total_reads = sum(len(d['reads']) for d in self.offsets.values())
        total_writes = sum(len(d['writes']) for d in self.offsets.values())

        print(f"  总读取次数: {total_reads:,}")
        print(f"  总写入次数: {total_writes:,}")

        # 最常访问的偏移量
        access_counts = [(off, len(d['reads']) + len(d['writes']))
                        for off, d in self.offsets.items()]
        access_counts.sort(key=lambda x: x[1], reverse=True)

        print(f"\n[最常访问的偏移量 Top 10]")
        for offset, count in access_counts[:10]:
            data = self.offsets[offset]
            types = ', '.join(data['types'])
            print(f"  偏移 {offset:4d} (0x{offset:04x}): {count:4d} 次 - 类型: {types}")

def main():
    print("="*60)
    print("结构体偏移量分析工具")
    print("="*60)

    analyzer = StructAnalyzer()

    # 分析 CInventory
    analyzer.analyze_file('df_game_r_v2.c', class_filter='CInventory', max_lines=2000000)

    # 打印统计
    analyzer.print_statistics()

    # 生成结构体定义
    analyzer.generate_struct_definition('CInventory')

    return 0

if __name__ == '__main__':
    sys.exit(main())
