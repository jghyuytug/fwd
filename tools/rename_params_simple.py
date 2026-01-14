#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简化的参数重命名工具
只处理最明显的情况：基于类型的直接映射
"""

import re
import sys

# 简单的类型到名称映射（只处理最明显的情况）
TYPE_TO_NAME = {
    'CUser *': 'user',
    'CUserCharacInfo *': 'charac_info',
    'CInventory *': 'inventory',
    'CParty *': 'party',
    'CDungeon *': 'dungeon',
    'CBattle_Field *': 'battlefield',
    'MySQL *': 'mysql',
    'Inven_Item *': 'item',
}

def rename_in_line(line):
    """在一行中进行重命名"""
    modified = False

    for type_pattern, suggested_name in TYPE_TO_NAME.items():
        # 匹配模式：Type a1, Type a2, etc.
        # 例如：CUser *a2 → CUser *user
        pattern = re.escape(type_pattern) + r'\s*([av]\d+)\b'

        def replace_func(match):
            nonlocal modified
            old_name = match.group(1)
            # 检查是否是第一个出现（如果有多个相同类型的参数，添加后缀）
            count = line.count(type_pattern)
            if count > 1:
                # 有多个相同类型，使用 user1, user2 等
                return f"{type_pattern}{suggested_name}"  # 简化：不处理多个
            else:
                modified = True
                return f"{type_pattern}{suggested_name}"

        line = re.sub(pattern, replace_func, line)

    return line, modified

def main():
    input_file = 'df_game_r.c'
    output_file = 'df_game_r_renamed.c'

    print(f"Reading {input_file}...")
    total_lines = 0
    renamed_lines = 0

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as fin:
        with open(output_file, 'w', encoding='utf-8') as fout:
            for line_no, line in enumerate(fin, 1):
                new_line, modified = rename_in_line(line)
                fout.write(new_line)

                total_lines += 1
                if modified:
                    renamed_lines += 1

                if line_no % 100000 == 0:
                    print(f"  Processed {line_no:,} lines...")

    print(f"\nDone!")
    print(f"Total lines: {total_lines:,}")
    print(f"Modified lines: {renamed_lines:,}")
    print(f"Output: {output_file}")

if __name__ == '__main__':
    main()
