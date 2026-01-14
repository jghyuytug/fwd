#!/usr/bin/env python3
"""
comment_stl_members.py - Comment out STL template member declarations
Eliminates "not a member of" errors for STL internal types
"""

import sys
import re

def main():
    stl_type = sys.argv[sys.argv.index('--type') + 1] if '--type' in sys.argv else 'all'

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

        if stl_type == '_Rb_tree':
            # Comment std::_Rb_tree<...>::member
            if 'std::_Rb_tree<' in line and '::' in line:
                # Make sure it's a member access, not just declaration
                if re.search(r'std::_Rb_tree<[^>]+>::\w+', line):
                    should_comment = True

        elif stl_type == 'pair':
            # Comment std::pair<...>::member (but not _Select1st which was handled earlier)
            if 'std::pair<' in line and '::' in line and '_Select1st' not in line:
                if re.search(r'std::pair<[^>]+>::\w+', line):
                    should_comment = True

        elif stl_type == 'map':
            # Comment std::map<...>::member
            if 'std::map<' in line and '::' in line:
                if re.search(r'std::map<[^>]+>::\w+', line):
                    should_comment = True

        elif stl_type == 'others':
            # Comment other STL template members
            # Pattern: std::<template><...>::member
            if re.search(r'std::\w+<[^>]+>::\w+', line):
                # Exclude already handled types
                if not any(x in line for x in ['_Select1st', '_Rb_tree', 'pair<', 'map<']):
                    should_comment = True

        else:
            # Comment all STL template members
            if re.search(r'std::\w+<[^>]+>::\w+', line):
                should_comment = True

        if should_comment:
            output.append(f'// {line}')
            commented_count += 1
        else:
            output.append(line)

    # Write output
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(output)

    print(f"✓ 已注释 {commented_count} 行 std::{stl_type} 模板成员")

if __name__ == '__main__':
    main()
