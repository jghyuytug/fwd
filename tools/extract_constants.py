#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
常量提取工具
扫描代码中的魔术数字（十六进制和大的十进制常量），推断其含义并生成 constants.h
"""

import re
import sys
from collections import defaultdict

class ConstantExtractor:
    def __init__(self):
        self.hex_constants = defaultdict(lambda: {
            'count': 0,
            'contexts': []
        })
        self.decimal_constants = defaultdict(lambda: {
            'count': 0,
            'contexts': []
        })
        self.current_function = None

    def extract_from_line(self, line, line_no):
        """从一行代码提取常量"""
        # 十六进制常量 (0x...)
        hex_pattern = r'\b(0x[0-9A-Fa-f]+)[uUlL]*\b'
        for match in re.finditer(hex_pattern, line):
            hex_str = match.group(1)
            try:
                value = int(hex_str, 16)

                # 过滤掉一些常见的小值（0x0-0xF 太常见）
                if value < 0x10:
                    continue

                # 过滤掉指针模式 like 0x12345678 in address
                if '(*)' in line or 'sub_' in line:
                    continue

                self.hex_constants[hex_str]['count'] += 1

                context = line.strip()
                if len(context) < 200:
                    self.hex_constants[hex_str]['contexts'].append((line_no, context))
            except:
                pass

        # 大的十进制常量（至少 4 位数字，可能是偏移量、大小等）
        # 排除函数参数中的行号引用
        decimal_pattern = r'\b(\d{4,})\b'
        for match in re.finditer(decimal_pattern, line):
            try:
                value_str = match.group(1)
                value = int(value_str)

                # 过滤条件
                # 1. 太小的值（< 1000）
                if value < 1000:
                    continue

                # 2. 过滤掉年份（1900-2100）
                if 1900 <= value <= 2100:
                    continue

                # 3. 过滤掉看起来像地址的（太大）
                if value > 10000000:
                    continue

                # 4. 过滤掉行号引用 (Line XXXXX:)
                if 'Line' in line or 'line' in line:
                    continue

                self.decimal_constants[value_str]['count'] += 1

                context = line.strip()
                if len(context) < 200:
                    self.decimal_constants[value_str]['contexts'].append((line_no, context))
            except:
                pass

    def detect_function(self, line):
        """检测函数定义"""
        if re.match(r'^[a-zA-Z_].*::', line) and '(' in line:
            match = re.search(r'(\w+(?:::\w+)*)\s*\(', line)
            if match:
                self.current_function = match.group(1)

    def scan_file(self, filename, max_lines=None):
        """扫描文件"""
        print(f"[*] 扫描文件: {filename}")

        line_count = 0
        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            for line_no, line in enumerate(f, 1):
                self.detect_function(line)
                self.extract_from_line(line, line_no)

                line_count += 1
                if line_count % 100000 == 0:
                    print(f"    处理 {line_count:,} 行...")

                if max_lines and line_no >= max_lines:
                    break

        print(f"[+] 扫描完成")
        print(f"[+] 发现 {len(self.hex_constants)} 个不同的十六进制常量")
        print(f"[+] 发现 {len(self.decimal_constants)} 个不同的十进制常量")

    def infer_constant_name(self, const_str, contexts):
        """根据上下文推断常量名称"""
        context_str = ' '.join([ctx for _, ctx in contexts[:5]]).lower()

        # 十六进制常量命名规则
        if const_str.startswith('0x'):
            value = int(const_str, 16)

            # 检查是否是位标志
            if value & (value - 1) == 0 and value > 0:  # 2的幂次
                return f'FLAG_{const_str.upper()}'

            # 检查上下文关键词
            if 'size' in context_str or 'len' in context_str:
                return f'SIZE_{const_str.upper()}'
            if 'mask' in context_str:
                return f'MASK_{const_str.upper()}'
            if 'offset' in context_str:
                return f'OFFSET_{const_str.upper()}'
            if 'max' in context_str or 'limit' in context_str:
                return f'MAX_{const_str.upper()}'

            return f'CONST_{const_str.upper()}'

        # 十进制常量命名规则
        else:
            value = int(const_str)

            if 'size' in context_str or 'len' in context_str:
                return f'SIZE_{value}'
            if 'max' in context_str or 'limit' in context_str:
                return f'MAX_{value}'
            if 'offset' in context_str:
                return f'OFFSET_{value}'
            if 'count' in context_str:
                return f'COUNT_{value}'

            return f'CONST_{value}'

    def generate_header(self, output_file, min_usage=10):
        """生成 constants.h"""
        print(f"\n[*] 生成头文件: {output_file}")
        print(f"[*] 最小使用次数阈值: {min_usage}")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("""/*
 * constants.h - 游戏常量定义
 *
 * 自动从 df_game_r_v2.c 提取
 * 只包含使用次数 >= %d 的常量
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

//=============================================================================
// 常量定义
//=============================================================================

""" % min_usage)

            # 十六进制常量
            hex_filtered = [(k, v) for k, v in self.hex_constants.items() if v['count'] >= min_usage]
            hex_sorted = sorted(hex_filtered, key=lambda x: x[1]['count'], reverse=True)

            if hex_sorted:
                f.write("//-----------------------------------------------------------------------------\n")
                f.write("// 十六进制常量\n")
                f.write("//-----------------------------------------------------------------------------\n\n")

                for const_str, data in hex_sorted:
                    const_name = self.infer_constant_name(const_str, data['contexts'])

                    f.write(f"// 使用次数: {data['count']}\n")

                    if data['contexts']:
                        _, example = data['contexts'][0]
                        if len(example) < 120:
                            f.write(f"// 示例: {example}\n")

                    f.write(f"#define {const_name:40s} {const_str}\n\n")

            # 十进制常量
            dec_filtered = [(k, v) for k, v in self.decimal_constants.items() if v['count'] >= min_usage]
            dec_sorted = sorted(dec_filtered, key=lambda x: x[1]['count'], reverse=True)

            if dec_sorted:
                f.write("//-----------------------------------------------------------------------------\n")
                f.write("// 十进制常量\n")
                f.write("//-----------------------------------------------------------------------------\n\n")

                for const_str, data in dec_sorted:
                    const_name = self.infer_constant_name(const_str, data['contexts'])

                    f.write(f"// 使用次数: {data['count']}\n")

                    if data['contexts']:
                        _, example = data['contexts'][0]
                        if len(example) < 120:
                            f.write(f"// 示例: {example}\n")

                    f.write(f"#define {const_name:40s} {const_str}\n\n")

            f.write("""
#endif /* CONSTANTS_H */
""")

        print(f"[+] 头文件已生成: {output_file}")

    def print_statistics(self):
        """打印统计信息"""
        print(f"\n{'='*60}")
        print("常量统计")
        print(f"{'='*60}\n")

        # 十六进制常量 Top 20
        hex_sorted = sorted(self.hex_constants.items(),
                           key=lambda x: x[1]['count'],
                           reverse=True)

        print("最常用的十六进制常量 (Top 20):")
        print(f"{'常量':<15} {'使用次数':<10} {'推断名称':<40} {'示例上下文'}")
        print("-" * 120)

        for const_str, data in hex_sorted[:20]:
            const_name = self.infer_constant_name(const_str, data['contexts'])
            example = ""
            if data['contexts']:
                _, ctx = data['contexts'][0]
                example = ctx[:50] + "..." if len(ctx) > 50 else ctx

            print(f"{const_str:<15} {data['count']:<10} {const_name:<40} {example}")

        # 十进制常量 Top 20
        dec_sorted = sorted(self.decimal_constants.items(),
                           key=lambda x: x[1]['count'],
                           reverse=True)

        print(f"\n最常用的十进制常量 (Top 20):")
        print(f"{'常量':<15} {'使用次数':<10} {'推断名称':<40} {'示例上下文'}")
        print("-" * 120)

        for const_str, data in dec_sorted[:20]:
            const_name = self.infer_constant_name(const_str, data['contexts'])
            example = ""
            if data['contexts']:
                _, ctx = data['contexts'][0]
                example = ctx[:50] + "..." if len(ctx) > 50 else ctx

            print(f"{const_str:<15} {data['count']:<10} {const_name:<40} {example}")

def main():
    print("="*60)
    print("常量提取工具")
    print("="*60)

    extractor = ConstantExtractor()

    # 扫描文件
    extractor.scan_file('df_game_r_v2.c')

    # 打印统计
    extractor.print_statistics()

    # 生成头文件（只包含使用次数 >= 10 的常量）
    extractor.generate_header('include/constants.h', min_usage=10)

    return 0

if __name__ == '__main__':
    sys.exit(main())
