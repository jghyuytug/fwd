#!/usr/bin/env python3
"""
注释掉 df_game_r_decl.h 中的游戏类成员函数声明

保留：
- 全局函数（不带::）
- std:: 命名空间（已被注释）
- __gnu_cxx:: 命名空间（已被注释）
- 已经注释的行

注释：
- 游戏类成员函数（ClassName::methodName）
"""

import re
import sys
from pathlib import Path

def should_comment_line(line):
    """判断是否应该注释这一行"""
    # 跳过空行和已注释行
    if not line.strip() or line.strip().startswith('//'):
        return False

    # 检查是否包含 ::
    if '::' not in line:
        return False

    # 保留 std:: 和 __gnu_cxx::
    if 'std::' in line or '__gnu_cxx::' in line:
        return False

    # 检查是否是成员函数声明（简化：开头是类型名或返回类型）
    if re.match(r'^[a-zA-Z_].*::', line):
        return True

    return False

def main():
    decl_file = Path("src_split/df_game_r_decl.h")

    if not decl_file.exists():
        print(f"Error: {decl_file} not found")
        return 1

    print("Reading df_game_r_decl.h...")
    content = decl_file.read_text(encoding='utf-8', errors='ignore')
    lines = content.split('\n')

    print(f"Total lines: {len(lines)}")

    # 备份
    backup_file = decl_file.with_suffix('.h.before_member_clean')
    if not backup_file.exists():
        print(f"Creating backup: {backup_file}")
        backup_file.write_text(content, encoding='utf-8')

    # 处理
    cleaned_lines = []
    commented_count = 0

    for line in lines:
        if should_comment_line(line):
            cleaned_lines.append(f"// {line}  // [Game class member]")
            commented_count += 1
        else:
            cleaned_lines.append(line)

    # 写回
    print(f"Writing cleaned file...")
    cleaned_content = '\n'.join(cleaned_lines)
    decl_file.write_text(cleaned_content, encoding='utf-8')

    print(f"\nCompleted!")
    print(f"Commented out {commented_count} game class member function declarations")

    return 0

if __name__ == "__main__":
    sys.exit(main())
