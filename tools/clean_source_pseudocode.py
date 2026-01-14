#!/usr/bin/env python3
"""
清理 24 个源文件中的 Hex-Rays 伪代码

处理两类问题：
1. 反引号伪代码：`guard variable for'..., `non-virtual thunk to'...
2. 寄存器标记：@<eax>, @<esi>, @<ebx> 等
"""

import re
import sys
from pathlib import Path

def clean_pseudocode(content):
    """清理 Hex-Rays 伪代码"""
    lines = content.split('\n')
    cleaned_lines = []
    modified_count = 0

    for line in lines:
        original_line = line
        should_comment = False

        # 如果已经是注释，跳过
        if line.strip().startswith('//'):
            cleaned_lines.append(line)
            continue

        # 检查是否包含反引号伪代码
        if '`' in line:
            should_comment = True

        # 检查是否包含寄存器标记 @<reg>
        if re.search(r'@<e[abcd]x>', line) or re.search(r'@<[ers][is][lp]>', line):
            should_comment = True

        if should_comment:
            cleaned_lines.append(f"// {line}  // [Hex-Rays pseudo-code]")
            modified_count += 1
        else:
            cleaned_lines.append(line)

    return '\n'.join(cleaned_lines), modified_count

def main():
    src_dir = Path("src_split")

    if not src_dir.exists():
        print(f"Error: {src_dir} not found")
        return 1

    total_modified = 0

    # 处理所有 24 个源文件
    for i in range(1, 25):
        part_file = src_dir / f"df_game_r_part{i:02d}.c"

        if not part_file.exists():
            print(f"Warning: {part_file} not found, skipping")
            continue

        print(f"Processing {part_file.name}...", end=' ')

        # 读取文件
        content = part_file.read_text(encoding='utf-8', errors='ignore')

        # 清理伪代码
        cleaned_content, modified = clean_pseudocode(content)

        # 备份原文件
        backup_file = part_file.with_suffix('.c.before_pseudocode_clean')
        if not backup_file.exists():
            backup_file.write_text(content, encoding='utf-8')

        # 写入清理后的内容
        part_file.write_text(cleaned_content, encoding='utf-8')

        print(f"{modified} lines cleaned")
        total_modified += modified

    print(f"\nTotal: {total_modified} lines of pseudo-code commented out across 24 files")
    return 0

if __name__ == "__main__":
    sys.exit(main())
