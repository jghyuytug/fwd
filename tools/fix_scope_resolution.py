#!/usr/bin/env python3
"""
Fix C++ scope resolution in function declarations with forward-declared types.

IDA decompiler creates function declarations like:
    int __cdecl PacketBuf::put_byte(PacketBuf *self, char flag);

But PacketBuf is only forward-declared, so the scope resolution is invalid.
This script converts them to free functions:
    int __cdecl PacketBuf_put_byte(PacketBuf *self, char flag);
"""

import sys
import re
from pathlib import Path


def fix_scope_resolution(input_file, output_file=None):
    """
    Fix scope resolution by converting to free functions.

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

    # Pattern: ReturnType __cdecl ClassName::method_name(params);
    # Convert to: ReturnType __cdecl ClassName_method_name(params);
    pattern = r'(\w+(?:\s*\*)*\s+)__cdecl\s+(\w+)::(\w+)\s*\('
    replacement = r'\1__cdecl \2_\3('

    content_fixed, count = re.subn(pattern, replacement, content)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content_fixed)

    total_lines = content.count('\n') + 1
    return count, total_lines


def main():
    if len(sys.argv) < 2:
        print("Usage: python fix_scope_resolution.py <input_file> [output_file]")
        print("Example: python fix_scope_resolution.py include/network/PacketBuf.h")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    fixes, total_lines = fix_scope_resolution(input_file, output_file)

    output = output_file or input_file
    print(f"Fixed {fixes} scope resolution occurrences in {total_lines} lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
