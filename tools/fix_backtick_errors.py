#!/usr/bin/env python3
"""
Fix backtick syntax errors in decompiled C++ code.

IDA decompiler creates malformed guard variable declarations with backticks:
    __guard `guard variable for'FunctionName(...)::instance; // idb

Also affects anonymous namespace symbols:
    int `anonymous namespace'::variable_name; // weak

This script comments out these problematic lines.
"""

import sys
import re
from pathlib import Path


def fix_backtick_errors(input_file, output_file=None):
    """
    Fix backtick syntax errors by commenting out problematic lines.

    Args:
        input_file: Path to input file
        output_file: Path to output file (defaults to input_file if None)

    Returns:
        tuple: (lines_fixed, total_lines)
    """
    if output_file is None:
        output_file = input_file

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    fixed_lines = []
    lines_fixed = 0

    for line in lines:
        # Check for backtick characters
        if '`' in line:
            # Pattern 1: __guard lines with backticks
            if line.strip().startswith('__guard ') and '`' in line:
                fixed_lines.append(f"// REMOVED (backtick syntax error): {line}")
                lines_fixed += 1
                continue

            # Pattern 2: anonymous namespace with backticks
            if '`anonymous namespace' in line:
                fixed_lines.append(f"// REMOVED (backtick syntax error): {line}")
                lines_fixed += 1
                continue

            # Pattern 3: Any other line with backticks (likely errors)
            if line.strip() and not line.strip().startswith('//'):
                # Only comment out non-comment lines with backticks
                fixed_lines.append(f"// REMOVED (backtick syntax error): {line}")
                lines_fixed += 1
                continue

        # Keep line as-is
        fixed_lines.append(line)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.writelines(fixed_lines)

    return lines_fixed, len(lines)


def main():
    if len(sys.argv) < 2:
        print("Usage: python fix_backtick_errors.py <input_file> [output_file]")
        print("Example: python fix_backtick_errors.py src/network/PacketBuf.cpp")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    lines_fixed, total_lines = fix_backtick_errors(input_file, output_file)

    output = output_file or input_file
    print(f"Fixed {lines_fixed} lines out of {total_lines} total lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
