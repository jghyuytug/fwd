#!/usr/bin/env python3
"""
Fix 'this' keyword used as parameter name in function declarations.

IDA decompiler creates C-style member function declarations with explicit 'this':
    int __cdecl ClassName::method(ClassName *this, int param);

In C++, 'this' is a reserved keyword and cannot be used as parameter name.
This script renames 'this' to 'self' in function declarations.
"""

import sys
import re
from pathlib import Path


def fix_this_keyword(input_file, output_file=None):
    """
    Fix 'this' keyword by renaming to 'self' in function declarations.

    Args:
        input_file: Path to input file
        output_file: Path to output file (defaults to input_file if None)

    Returns:
        tuple: (lines_fixed, total_lines)
    """
    if output_file is None:
        output_file = input_file

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Pattern 1: Type *this, ... → Type *self, ...
    # Examples:
    #   PacketBuf *this, char flag → PacketBuf *self, char flag
    #   void **this, const char *src → void **self, const char *src
    pattern1 = r'(\w+\s*\*+\s*)this\s*([,;)])'
    content_fixed1, count1 = re.subn(pattern1, r'\1self\2', content)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content_fixed1)

    total_lines = content.count('\n') + 1
    return count1, total_lines


def main():
    if len(sys.argv) < 2:
        print("Usage: python fix_this_keyword.py <input_file> [output_file]")
        print("Example: python fix_this_keyword.py include/network/PacketBuf.h")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    fixes, total_lines = fix_this_keyword(input_file, output_file)

    output = output_file or input_file
    print(f"Fixed {fixes} occurrences of 'this' keyword in {total_lines} lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
