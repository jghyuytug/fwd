#!/usr/bin/env python3
"""
清理 df_game_r_decl.h 中无效的 STL 成员函数声明

Hex-Rays 反编译器会生成类似这样的声明：
  int __cdecl std::vector<unsigned char>::push_back(int a1, _BYTE *a2);

这些是无效的 C++ 语法（成员函数不能这样声明），需要注释掉。
"""

import re
import sys
from pathlib import Path

def clean_stl_member_declarations(content):
    """注释掉无效的 STL 成员函数声明"""
    lines = content.split('\n')
    cleaned_lines = []
    stl_member_count = 0

    # 匹配 STL 成员函数声明的模式
    patterns = [
        r'std::\w+<[^>]+>::',      # std::vector<T>::method
        r'std::\w+::',              # std::string::method
        r'__gnu_cxx::\w+<[^>]+>::', # __gnu_cxx::allocator<T>::method
        r'__gnu_cxx::\w+::',        # __gnu_cxx::method
    ]

    for line in lines:
        should_comment = False

        # 跳过已经注释的行
        if line.strip().startswith('//'):
            cleaned_lines.append(line)
            continue

        # 检查是否匹配任何 STL 成员函数模式
        for pattern in patterns:
            if re.search(pattern, line):
                should_comment = True
                break

        if should_comment:
            cleaned_lines.append(f"// {line}  // [Invalid STL member declaration removed]")
            stl_member_count += 1
        else:
            cleaned_lines.append(line)

    print(f"Commented out {stl_member_count} invalid STL member declarations")
    return '\n'.join(cleaned_lines)

def main():
    input_file = Path("src_split/df_game_r_decl.h")

    if not input_file.exists():
        print(f"Error: {input_file} not found")
        return 1

    print(f"Reading: {input_file}")
    content = input_file.read_text(encoding='utf-8')

    print("Cleaning invalid STL member declarations...")
    cleaned_content = clean_stl_member_declarations(content)

    # 备份原文件
    backup_file = input_file.with_suffix('.h.before_stl_clean')
    if not backup_file.exists():
        print(f"Creating backup: {backup_file}")
        backup_file.write_text(content, encoding='utf-8')

    print(f"Writing cleaned file: {input_file}")
    input_file.write_text(cleaned_content, encoding='utf-8')

    print("\nCompleted successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
