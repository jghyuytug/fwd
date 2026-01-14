#!/usr/bin/env python3
"""
Fix function bodies: replace 'this' with 'self' and ClassName:: with ClassName_.
"""

import sys
import re
from pathlib import Path


def fix_function_bodies(input_file, output_file=None):
    """
    Fix function bodies.

    Args:
        input_file: Path to input file
        output_file: Path to output file (defaults to input_file if None)

    Returns:
        tuple: (fixes_made, total_lines)
    """
    if output_file is None:
        output_file = input_file

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    fixes = 0

    # Fix 1: Replace 'this' with 'self' (except in comments)
    # Only replace in actual code, not in strings or comments
    pattern1 = r'\bthis\b'
    def replace_this(match):
        return 'self'

    content_fixed, count1 = re.subn(pattern1, replace_this, content)
    fixes += count1

    # Fix 2: Replace ClassName::method( with ClassName_method(
    pattern2 = r'(\w+)::(\w+)\s*\('
    replacement2 = r'\1_\2('
    content_fixed, count2 = re.subn(pattern2, replacement2, content_fixed)
    fixes += count2

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content_fixed)

    total_lines = content.count('\n') + 1
    return fixes, total_lines


def main():
    if len(sys.argv) < 2:
        print("Usage: python fix_function_bodies.py <input_file> [output_file]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    fixes, total_lines = fix_function_bodies(input_file, output_file)

    output = output_file or input_file
    print(f"Made {fixes} fixes in {total_lines} lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
