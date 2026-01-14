#!/usr/bin/env python3
"""
comment_select1st.py - Comment out std::_Select1st template member declarations
Eliminates template argument errors related to std::_Select1st
"""

import sys
import re

def main():
    if '--type' in sys.argv:
        type_filter = sys.argv[sys.argv.index('--type') + 1]
    else:
        type_filter = 'all'

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
        # Check if line contains std::_Select1st
        if 'std::_Select1st<' in line:
            if type_filter == 'StringUnorderedMap':
                # Only comment lines with StringUnorderedMap
                if 'StringUnorderedMap' in line:
                    output.append(f'// {line}')
                    commented_count += 1
                else:
                    output.append(line)
            elif type_filter == 'others':
                # Comment lines without StringUnorderedMap
                if 'StringUnorderedMap' not in line:
                    output.append(f'// {line}')
                    commented_count += 1
                else:
                    output.append(line)
            else:
                # Comment all _Select1st
                output.append(f'// {line}')
                commented_count += 1
        else:
            output.append(line)

    # Write output
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(output)

    print(f"✓ 已注释 {commented_count} 行 std::_Select1st ({type_filter})")

if __name__ == '__main__':
    main()
