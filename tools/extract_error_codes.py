#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
错误码提取工具
扫描代码中的负数错误码，推断其含义并生成 error_codes.h
"""

import re
import sys
from collections import defaultdict

class ErrorCodeExtractor:
    def __init__(self):
        self.error_codes = defaultdict(lambda: {
            'count': 0,
            'contexts': [],
            'functions': set()
        })
        self.current_function = None

    def extract_from_line(self, line, line_no):
        """从一行代码提取错误码"""
        # 匹配模式：变量 = -数字;
        patterns = [
            r'=\s*(-\d+)\s*;',           # v5 = -417;
            r'return\s+(-\d+)\s*;',      # return -102;
            r'==\s*(-\d+)',              # if (ret == -8)
            r'!=\s*(-\d+)',              # if (ret != -1)
        ]

        for pattern in patterns:
            for match in re.finditer(pattern, line):
                error_code = int(match.group(1))

                # 只记录负数错误码（通常 -1 到 -999）
                if -1000 < error_code < 0:
                    self.error_codes[error_code]['count'] += 1

                    # 保存上下文（去除多余空白）
                    context = line.strip()
                    if len(context) < 200:  # 不要太长的上下文
                        self.error_codes[error_code]['contexts'].append((line_no, context))

                    if self.current_function:
                        self.error_codes[error_code]['functions'].add(self.current_function)

    def detect_function(self, line):
        """检测函数定义"""
        # 简单检测：如果是函数定义行
        if re.match(r'^[a-zA-Z_].*::', line) and '(' in line and '{' not in line.strip():
            # 提取函数名
            match = re.search(r'(\w+(?:::\w+)*)\s*\(', line)
            if match:
                self.current_function = match.group(1)
        elif line.strip() == '}':
            self.current_function = None

    def infer_error_name(self, error_code, data):
        """根据上下文推断错误名称"""
        contexts_str = ' '.join([ctx for _, ctx in data['contexts'][:5]]).lower()

        # 常见错误模式
        patterns = {
            'hack': 'HACK_USER',
            'trade': 'TRADE_ERROR',
            'not.*tradable': 'NOT_TRADABLE',
            'item.*count': 'INSUFFICIENT_ITEM_COUNT',
            'money': 'MONEY_ERROR',
            'level': 'LEVEL_ERROR',
            'inventory.*full': 'INVENTORY_FULL',
            'delete.*item': 'DELETE_ITEM_ERROR',
            'invalid': 'INVALID_PARAMETER',
            'permission': 'PERMISSION_DENIED',
            'timeout': 'TIMEOUT',
            'not.*found': 'NOT_FOUND',
        }

        for pattern, name in patterns.items():
            if re.search(pattern, contexts_str):
                return f'ERR_{name}'

        # 默认名称
        return f'ERR_CODE_{abs(error_code)}'

    def scan_file(self, filename, max_lines=None):
        """扫描文件"""
        print(f"[*] 扫描文件: {filename}")

        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            for line_no, line in enumerate(f, 1):
                self.detect_function(line)
                self.extract_from_line(line, line_no)

                if line_no % 100000 == 0:
                    print(f"    处理 {line_no:,} 行...")

                if max_lines and line_no >= max_lines:
                    break

        print(f"[+] 扫描完成")
        print(f"[+] 发现 {len(self.error_codes)} 个不同的错误码")

    def generate_header(self, output_file):
        """生成 error_codes.h"""
        print(f"\n[*] 生成头文件: {output_file}")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("""/*
 * error_codes.h - 游戏错误码定义
 *
 * 自动从 df_game_r_v2.c 提取
 * 基于代码中的负数返回值和赋值语句
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

//=============================================================================
// 错误码定义
//
// 约定：
// - 成功: 0 或正数
// - 错误: 负数
// - 通用错误: -1 到 -99
// - 系统错误: -100 到 -199
// - 业务错误: -200 到 -999
//=============================================================================

""")

            # 按错误码排序
            sorted_codes = sorted(self.error_codes.items())

            # 分组
            groups = {
                '通用错误 (-1 到 -99)': [],
                '系统错误 (-100 到 -199)': [],
                '业务错误 (-200 到 -499)': [],
                '其他错误 (-500+)': []
            }

            for code, data in sorted_codes:
                if -100 < code < 0:
                    groups['通用错误 (-1 到 -99)'].append((code, data))
                elif -200 < code <= -100:
                    groups['系统错误 (-100 到 -199)'].append((code, data))
                elif -500 < code <= -200:
                    groups['业务错误 (-200 到 -499)'].append((code, data))
                else:
                    groups['其他错误 (-500+)'].append((code, data))

            # 输出分组
            for group_name, codes in groups.items():
                if not codes:
                    continue

                f.write(f"//-----------------------------------------------------------------------------\n")
                f.write(f"// {group_name}\n")
                f.write(f"//-----------------------------------------------------------------------------\n\n")

                for code, data in codes:
                    error_name = self.infer_error_name(code, data)

                    # 添加注释
                    f.write(f"// 使用次数: {data['count']}\n")

                    # 添加示例上下文
                    if data['contexts']:
                        _, example = data['contexts'][0]
                        if len(example) < 80:
                            f.write(f"// 示例: {example}\n")

                    # 添加使用函数
                    if data['functions']:
                        funcs = list(data['functions'])[:3]
                        f.write(f"// 函数: {', '.join(funcs)}\n")

                    f.write(f"#define {error_name:40s} {code}\n\n")

            f.write("""
#endif /* ERROR_CODES_H */
""")

        print(f"[+] 头文件已生成: {output_file}")

    def print_statistics(self):
        """打印统计信息"""
        print(f"\n{'='*60}")
        print("错误码统计")
        print(f"{'='*60}\n")

        print(f"总错误码数: {len(self.error_codes)}\n")

        # Top 20 最常用的错误码
        sorted_by_count = sorted(self.error_codes.items(),
                                key=lambda x: x[1]['count'],
                                reverse=True)

        print("最常用的错误码 (Top 20):")
        print(f"{'错误码':<10} {'使用次数':<10} {'推断名称':<30} {'示例上下文'}")
        print("-" * 100)

        for code, data in sorted_by_count[:20]:
            error_name = self.infer_error_name(code, data)
            example = ""
            if data['contexts']:
                _, ctx = data['contexts'][0]
                example = ctx[:50] + "..." if len(ctx) > 50 else ctx

            print(f"{code:<10} {data['count']:<10} {error_name:<30} {example}")

def main():
    print("="*60)
    print("错误码提取工具")
    print("="*60)

    extractor = ErrorCodeExtractor()

    # 扫描文件
    extractor.scan_file('df_game_r_v2.c')

    # 打印统计
    extractor.print_statistics()

    # 生成头文件
    extractor.generate_header('include/error_codes.h')

    return 0

if __name__ == '__main__':
    sys.exit(main())
