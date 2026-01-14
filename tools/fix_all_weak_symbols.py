#!/usr/bin/env python3
"""
Fix ALL weak symbol declarations in decompiled code.

Comment out any global variable declaration ending with:
- // weak
- // idb

These are defined in other modules and should not be redeclared here.
"""

import sys
import re
from pathlib import Path


def fix_all_weak_symbols(input_file, output_file=None):
    """
    Comment out all lines ending with // weak or // idb.

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
        stripped = line.strip()

        # Skip if already commented
        if stripped.startswith('//'):
            fixed_lines.append(line)
            continue

        # Check if line ends with // weak or // idb
        if stripped.endswith('// weak') or stripped.endswith('// idb'):
            fixed_lines.append(f"// REMOVED (weak/idb symbol): {line}")
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
        print("Usage: python fix_all_weak_symbols.py <input_file> [output_file]")
        print("Example: python fix_all_weak_symbols.py src/network/PacketBuf.cpp")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    lines_fixed, total_lines = fix_all_weak_symbols(input_file, output_file)

    output = output_file or input_file
    print(f"Commented out {lines_fixed} weak/idb symbol declarations in {total_lines} lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
