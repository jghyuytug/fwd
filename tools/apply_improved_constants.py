#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
常量应用工具 - 将 constants_improved.h 中的常量名称应用到代码中

智能替换策略：
1. 只替换独立的常量值（不在字符串、注释、地址中）
2. 优先替换高频常量
3. 保持代码格式和对齐
"""

import re
import sys
from collections import defaultdict

class ConstantApplier:
    def __init__(self):
        # 常量映射: 值 -> 名称
        self.hex_constants = {}  # "0xFFFF" -> "INVALID_ID_U16"
        self.dec_constants = {}  # "9992" -> "FLEX_YY_DEF_DEFAULT"

        # 替换统计
        self.replacements = defaultdict(int)
        self.total_replacements = 0

    def parse_constants_header(self, header_file):
        """解析 constants_improved.h 提取常量定义"""
        print(f"[*] 解析常量定义: {header_file}")

        with open(header_file, 'r', encoding='utf-8') as f:
            for line in f:
                # 匹配 #define NAME VALUE
                match = re.match(r'^\s*#define\s+(\w+)\s+(0x[0-9A-Fa-f]+|[\d]+)\s*$', line)
                if match:
                    const_name = match.group(1)
                    const_value = match.group(2)

                    # 跳过注释掉的定义
                    if line.strip().startswith('//'):
                        continue

                    if const_value.startswith('0x'):
                        self.hex_constants[const_value.upper()] = const_name
                    else:
                        self.dec_constants[const_value] = const_name

        print(f"[+] 加载了 {len(self.hex_constants)} 个十六进制常量")
        print(f"[+] 加载了 {len(self.dec_constants)} 个十进制常量")

    def is_safe_to_replace(self, line, match_start, match_end):
        """检查是否可以安全替换（不在字符串、注释、地址中）"""

        # 检查是否在字符串中
        before = line[:match_start]
        quote_count = before.count('"') - before.count('\\"')
        if quote_count % 2 == 1:  # 奇数个引号 = 在字符串内
            return False

        # 检查是否在注释中
        if '//' in before:
            comment_pos = before.rfind('//')
            # 确保不是在字符串中的 //
            before_comment = before[:comment_pos]
            quote_count_before_comment = before_comment.count('"') - before_comment.count('\\"')
            if quote_count_before_comment % 2 == 0:
                return False

        # 检查是否是地址的一部分（sub_XXXXX, 0x12345678 in address context）
        if 'sub_' in before[-10:]:
            return False

        # 检查前后字符，确保是独立的数字（不是标识符的一部分）
        char_before = before[-1] if before else ' '
        char_after = line[match_end] if match_end < len(line) else ' '

        # 前后必须是非字母数字字符（除了 x/X 用于十六进制）
        if char_before.isalnum() and char_before not in 'xX':
            return False
        if char_after.isalnum() and char_after not in 'uUlL':  # 允许后缀 u, l
            return False

        return True

    def replace_in_line(self, line, line_no):
        """替换一行中的常量"""
        modified = False
        result = line

        # 先处理十六进制常量（更特殊，优先级高）
        # 匹配 0xHEX[uUlL]*
        hex_pattern = r'\b(0x[0-9A-Fa-f]+)[uUlL]*\b'

        def hex_replacer(match):
            nonlocal modified
            hex_str = match.group(1).upper()

            # 检查是否在我们的映射中
            if hex_str in self.hex_constants:
                # 检查是否安全替换
                if self.is_safe_to_replace(result, match.start(), match.end()):
                    const_name = self.hex_constants[hex_str]
                    self.replacements[const_name] += 1
                    self.total_replacements += 1
                    modified = True
                    return const_name

            return match.group(0)

        result = re.sub(hex_pattern, hex_replacer, result)

        # 处理十进制常量
        # 匹配独立的数字（4位或以上，避免替换小数字）
        dec_pattern = r'\b(\d{4,})\b'

        def dec_replacer(match):
            nonlocal modified
            dec_str = match.group(1)

            # 检查是否在我们的映射中
            if dec_str in self.dec_constants:
                # 检查是否安全替换
                if self.is_safe_to_replace(result, match.start(), match.end()):
                    const_name = self.dec_constants[dec_str]
                    self.replacements[const_name] += 1
                    self.total_replacements += 1
                    modified = True
                    return const_name

            return match.group(0)

        result = re.sub(dec_pattern, dec_replacer, result)

        return result, modified

    def apply_to_file(self, input_file, output_file):
        """应用常量替换到整个文件"""
        print(f"\n[*] 处理文件: {input_file}")
        print(f"[*] 输出到: {output_file}")

        lines_modified = 0
        total_lines = 0

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as fin:
            with open(output_file, 'w', encoding='utf-8') as fout:
                for line_no, line in enumerate(fin, 1):
                    total_lines += 1

                    new_line, modified = self.replace_in_line(line, line_no)

                    if modified:
                        lines_modified += 1

                    fout.write(new_line)

                    # 进度显示
                    if line_no % 100000 == 0:
                        print(f"    处理 {line_no:,} 行... (已修改 {lines_modified:,} 行)")

        print(f"\n[+] 处理完成！")
        print(f"[+] 总行数: {total_lines:,}")
        print(f"[+] 修改行数: {lines_modified:,} ({lines_modified*100.0/total_lines:.2f}%)")
        print(f"[+] 总替换次数: {self.total_replacements:,}")

    def print_statistics(self):
        """打印替换统计"""
        print(f"\n{'='*80}")
        print("替换统计 (Top 30)")
        print(f"{'='*80}\n")

        # 按替换次数排序
        sorted_replacements = sorted(self.replacements.items(),
                                     key=lambda x: x[1],
                                     reverse=True)

        print(f"{'常量名称':<50} {'替换次数':<10}")
        print("-" * 80)

        for const_name, count in sorted_replacements[:30]:
            print(f"{const_name:<50} {count:<10,}")

        if len(sorted_replacements) > 30:
            print(f"\n... 还有 {len(sorted_replacements) - 30} 个常量")

    def generate_report(self, output_file):
        """生成详细的替换报告"""
        print(f"\n[*] 生成替换报告: {output_file}")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("# 常量应用报告\n\n")
            f.write(f"## 总体统计\n\n")
            f.write(f"- 总替换次数: {self.total_replacements:,}\n")
            f.write(f"- 不同常量数: {len(self.replacements)}\n")
            f.write(f"- 十六进制常量: {len(self.hex_constants)}\n")
            f.write(f"- 十进制常量: {len(self.dec_constants)}\n\n")

            f.write(f"## 替换详情\n\n")
            f.write(f"| 排名 | 常量名称 | 替换次数 |\n")
            f.write(f"|------|----------|----------|\n")

            sorted_replacements = sorted(self.replacements.items(),
                                         key=lambda x: x[1],
                                         reverse=True)

            for rank, (const_name, count) in enumerate(sorted_replacements, 1):
                f.write(f"| {rank} | {const_name} | {count:,} |\n")

        print(f"[+] 报告已生成: {output_file}")


def main():
    print("="*80)
    print("常量应用工具 - 将改进的常量名称应用到代码")
    print("="*80)

    # 创建应用器
    applier = ConstantApplier()

    # 解析常量定义
    applier.parse_constants_header('include/constants_improved.h')

    # 应用到代码
    applier.apply_to_file('df_game_r_v2.c', 'df_game_r_v3.c')

    # 打印统计
    applier.print_statistics()

    # 生成报告
    applier.generate_report('PHASE3.6_APPLY_CONSTANTS_REPORT.md')

    print("\n" + "="*80)
    print("完成！df_game_r_v3.c 已生成")
    print("="*80)

    return 0


if __name__ == '__main__':
    sys.exit(main())
