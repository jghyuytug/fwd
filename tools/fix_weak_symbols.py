#!/usr/bin/env python3
"""
Fix weak symbol declarations with nested scope resolution.

IDA decompiler creates global variable declarations like:
    pthread_mutex_t ClassName::method(void)::variable_name; // weak
    _UNKNOWN namespace::function()::static_var; // weak

These should be commented out as they're defined elsewhere.
"""

import sys
import re
from pathlib import Path


def fix_weak_symbols(input_file, output_file=None):
    """
    Comment out weak symbol declarations with nested ::.

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
        # Check if line contains :: twice or more (nested scope)
        # and ends with // weak or // idb
        if '::' in line and line.count('::') >= 2:
            if '// weak' in line or '// idb' in line:
                # Check it's not already commented
                if not line.strip().startswith('//'):
                    fixed_lines.append(f"// REMOVED (weak symbol): {line}")
                    lines_fixed += 1
                    continue

        # Also check for lines with single :: but are globals with function calls
        # Example: pthread_mutex_t PacketBuf::compress_packet(void)::var;
        if '::' in line and '(' in line and ')::' in line:
            if '// weak' in line or '// idb' in line:
                if not line.strip().startswith('//'):
                    fixed_lines.append(f"// REMOVED (weak symbol): {line}")
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
        print("Usage: python fix_weak_symbols.py <input_file> [output_file]")
        print("Example: python fix_weak_symbols.py src/network/PacketBuf.cpp")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    lines_fixed, total_lines = fix_weak_symbols(input_file, output_file)

    output = output_file or input_file
    print(f"Fixed {lines_fixed} weak symbol declarations in {total_lines} lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
