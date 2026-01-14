#!/usr/bin/env python3
"""
comment_boost.py - Comment out boost namespace declarations
Eliminates namespace and declaration errors related to boost
"""

import sys
import re

def main():
    namespace = sys.argv[sys.argv.index('--namespace') + 1] if '--namespace' in sys.argv else 'all'

    file_path = 'src_split/df_game_r_decl.h'

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: File not found: {file_path}")
        sys.exit(1)

    output = []
    commented_count = 0

    for line in lines:
        should_comment = False

        if namespace == 'gregorian':
            # Comment boost::gregorian declarations
            if 'boost::gregorian' in line and ('class ' in line or 'struct ' in line):
                should_comment = True
        elif namespace == 'others':
            # Comment other boost declarations (not gregorian)
            if 'boost::' in line and 'gregorian' not in line and ('class ' in line or 'struct ' in line):
                should_comment = True
        else:
            # Comment all boost declarations
            if 'boost::' in line and ('class ' in line or 'struct ' in line):
                should_comment = True

        if should_comment:
            output.append(f'// {line}')
            commented_count += 1
        else:
            output.append(line)

    # Write output
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(output)

    print(f"✓ 已注释 {commented_count} 行 boost::{namespace} 声明")

if __name__ == '__main__':
    main()
