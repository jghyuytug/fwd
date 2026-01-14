#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
批量重命名反编译代码中的函数名
sub_XXXXX → 真实函数名（通过注释方式）

策略：
1. 简单函数名（纯C风格）→ 直接替换
2. C++符号（类、命名空间）→ 保留sub_但添加注释
3. 未找到的函数 → 保持原样

输出示例：
  int sub_807D720()  →  int _init()
  int sub_80C8C96()  →  int sub_80C8C96()  /* CUser::GetUID() const */
"""

import re
import sys
import os

def load_mapping(mapping_file):
    """加载函数名映射表（地址 → 真实名称）"""
    print(f"[*] 读取映射文件: {mapping_file}")
    mapping = {}

    with open(mapping_file, 'r', encoding='utf-8', errors='ignore') as f:
        for line_no, line in enumerate(f, 1):
            # 格式：0x807D708	_init
            parts = line.strip().split('\t')
            if len(parts) >= 2:
                addr_str = parts[0]
                func_name = '\t'.join(parts[1:])  # 处理函数名中可能包含\t的情况

                # 标准化地址：移除0x前缀，转大写
                addr = addr_str.replace('0x', '').replace('0X', '').upper()

                # 同时存储两种格式：带前导0的8位和不带前导0的原始格式
                # 这样可以匹配代码中的 sub_0807D720 和 sub_807D720
                mapping[addr.zfill(8)] = func_name  # 8位格式
                mapping[addr.lstrip('0')] = func_name  # 去除前导0

            if line_no % 50000 == 0:
                print(f"    已读取 {line_no} 行...", file=sys.stderr)

    return mapping


def is_cpp_symbol(func_name):
    """判断是否为C++符号（不能直接在C代码中使用）"""
    cpp_indicators = [
        '::',      # 命名空间/类作用域
        '<',       # 模板
        '>',       # 模板
        '&',       # 引用
        ' const',  # const修饰符
        'operator',# 运算符重载
        '~',       # 析构函数
    ]
    return any(indicator in func_name for indicator in cpp_indicators)


def categorize_function(func_name):
    """
    分类函数名
    返回: ('direct', func_name) - 可直接替换
         ('comment', func_name) - 仅添加注释
         ('skip', func_name) - 跳过
    """
    # C++ mangled name (_Z开头)
    if func_name.startswith('_Z'):
        return ('skip', func_name)

    # C++符号（包含类、命名空间等）
    if is_cpp_symbol(func_name):
        return ('comment', func_name)

    # 简单的C风格函数名
    # 检查是否为有效的C标识符
    if re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*$', func_name):
        return ('direct', func_name)

    # 其他情况（如包含括号的函数签名）
    return ('comment', func_name)


def process_line(line, mapping, stats):
    """处理一行代码，替换函数名"""
    # 查找所有 sub_XXXXXXXX 模式（7-8位十六进制）
    pattern = r'\bsub_([0-9A-Fa-f]{7,8})\b'

    def replace_func(match):
        addr_raw = match.group(1).upper()
        original = match.group(0)  # sub_XXXXXXXX

        # 尝试查找映射（先原始格式，再去除前导0，再8位填充）
        real_name = None
        for addr_variant in [addr_raw, addr_raw.lstrip('0'), addr_raw.zfill(8)]:
            if addr_variant in mapping:
                real_name = mapping[addr_variant]
                break

        if not real_name:
            stats['not_found'] += 1
            return original

        category, _ = categorize_function(real_name)

        if category == 'direct':
            # 直接替换
            stats['direct_replaced'] += 1
            return real_name
        elif category == 'comment':
            # 保留sub_但添加注释
            stats['comment_added'] += 1
            return f"{original}  /* {real_name} */"
        else:
            # 跳过（mangled names）
            stats['mangled_skipped'] += 1
            return original

    return re.sub(pattern, replace_func, line)


def rename_functions(input_file, output_file, mapping):
    """批量重命名函数"""
    print(f"[*] 输入文件: {input_file}")
    print(f"[*] 输出文件: {output_file}")

    stats = {
        'total_lines': 0,
        'direct_replaced': 0,      # 直接替换的函数
        'comment_added': 0,        # 添加注释的C++函数
        'mangled_skipped': 0,      # 跳过的mangled names
        'not_found': 0,            # 未找到映射的函数
    }

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as fin:
        with open(output_file, 'w', encoding='utf-8') as fout:
            for line_no, line in enumerate(fin, 1):
                new_line = process_line(line, mapping, stats)
                fout.write(new_line)

                stats['total_lines'] = line_no

                # 每10万行显示进度
                if line_no % 100000 == 0:
                    replaced = stats['direct_replaced'] + stats['comment_added']
                    print(f"    第 {line_no:,} 行 | 已替换: {replaced:,}", file=sys.stderr)

    return stats


def print_stats(stats):
    """打印统计信息"""
    print("\n[统计结果]")
    print(f"  总行数: {stats['total_lines']:,}")
    print(f"  直接替换: {stats['direct_replaced']:,}")
    print(f"  添加注释: {stats['comment_added']:,}")
    print(f"  Mangled符号(跳过): {stats['mangled_skipped']:,}")
    print(f"  未找到映射: {stats['not_found']:,}")

    total_replaced = stats['direct_replaced'] + stats['comment_added']
    total_functions = total_replaced + stats['mangled_skipped'] + stats['not_found']

    if total_functions > 0:
        coverage = (total_replaced / total_functions) * 100
        print(f"\n[覆盖率]")
        print(f"  成功还原: {total_replaced:,} / {total_functions:,} ({coverage:.1f}%)")


def main():
    print("="*60)
    print("批量函数重命名工具")
    print("="*60)

    # 文件路径
    mapping_file = 'symbols/function_name_mapping.txt'
    input_file = 'df_game_r.c'
    output_file = 'df_game_r_restored.c'

    # 检查文件存在
    for f in [mapping_file, input_file]:
        if not os.path.exists(f):
            print(f"[!] 错误: 文件不存在: {f}")
            return 1

    print("\n[1/3] 加载函数名映射表")
    mapping = load_mapping(mapping_file)
    print(f"[+] 加载完成: {len(mapping):,} 个函数映射")

    # 分类统计
    categories = {'direct': 0, 'comment': 0, 'skip': 0}
    for func_name in mapping.values():
        cat, _ = categorize_function(func_name)
        categories[cat] += 1

    print(f"    - 可直接替换: {categories['direct']:,}")
    print(f"    - 需添加注释: {categories['comment']:,}")
    print(f"    - Mangled符号: {categories['skip']:,}")

    print("\n[2/3] 批量重命名函数（这将需要一段时间...）")
    stats = rename_functions(input_file, output_file, mapping)

    print("\n[3/3] 完成")
    print_stats(stats)

    # 计算文件大小
    if os.path.exists(output_file):
        size_mb = os.path.getsize(output_file) / (1024 * 1024)
        print(f"\n[+] 输出文件已生成: {output_file}")
        print(f"    文件大小: {size_mb:.1f} MB")

    return 0


if __name__ == '__main__':
    sys.exit(main())
