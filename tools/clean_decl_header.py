#!/usr/bin/env python3
"""
清理 df_game_r_decl.h 文件中的 MSVC 特有语法和 Hex-Rays 伪代码

修复内容：
1. unsigned __int8/16/32/64 → uint8_t/uint16_t/uint32_t/uint64_t
2. 移除全局构造函数伪代码
3. 移除虚函数 thunk 伪代码
4. 移除匿名命名空间伪代码
5. 修复其他 Hex-Rays 特有语法
"""

import re
import sys
from pathlib import Path

def clean_msvc_types(content):
    """替换 MSVC 风格的整数类型为标准 C 类型"""
    replacements = [
        (r'\bunsigned __int64\b', 'uint64_t'),
        (r'\bunsigned __int32\b', 'uint32_t'),
        (r'\bunsigned __int16\b', 'uint16_t'),
        (r'\bunsigned __int8\b', 'uint8_t'),
        (r'\b__int64\b', 'int64_t'),
        (r'\b__int32\b', 'int32_t'),
        (r'\b__int16\b', 'int16_t'),
        (r'\b__int8\b', 'int8_t'),
    ]

    for pattern, replacement in replacements:
        content = re.sub(pattern, replacement, content)

    return content

def remove_all_pseudo_code(content):
    """移除所有 Hex-Rays 伪代码（使用单一通用模式）"""

    # 通用模式：匹配所有包含反引号的行
    # 这包括:
    # - `global constructor keyed to'...
    # - `non-virtual thunk to'...
    # - `virtual thunk to'...
    # - `anonymous namespace'::...
    # 等所有变种

    lines = content.split('\n')
    cleaned_lines = []

    for line in lines:
        # 如果行中包含反引号，很可能是 Hex-Rays 伪代码
        if '`' in line and not line.strip().startswith('//'):
            # 注释掉这一行
            cleaned_lines.append(f"// {line}  // [Hex-Rays pseudo-code removed]")
        else:
            cleaned_lines.append(line)

    return '\n'.join(cleaned_lines)

def clean_decl_header(input_file, output_file=None):
    """清理声明文件"""
    if output_file is None:
        output_file = input_file

    print(f"Reading: {input_file}")
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    original_lines = content.count('\n')

    # 应用清理操作
    print("Cleaning MSVC-style types...")
    content = clean_msvc_types(content)

    print("Removing Hex-Rays pseudo-code...")
    content = remove_all_pseudo_code(content)

    # 写回文件
    print(f"Writing: {output_file}")
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content)

    final_lines = content.count('\n')

    print(f"\nCompleted:")
    print(f"  Original lines: {original_lines}")
    print(f"  Final lines:    {final_lines}")
    print(f"  Difference:     {final_lines - original_lines}")

    return True

def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("Usage: python clean_decl_header.py <input_file> [output_file]")
        print("\nExample:")
        print("  python clean_decl_header.py src_split/df_game_r_decl.h")
        print("  python clean_decl_header.py src_split/df_game_r_decl.h src_split/df_game_r_decl_clean.h")
        return 1

    input_file = Path(sys.argv[1])
    output_file = Path(sys.argv[2]) if len(sys.argv) > 2 else input_file

    if not input_file.exists():
        print(f"Error: File not found: {input_file}")
        return 1

    try:
        clean_decl_header(input_file, output_file)
        print("\nSuccess!")
        return 0
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())
