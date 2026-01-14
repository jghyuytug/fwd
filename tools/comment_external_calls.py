#!/usr/bin/env python3
"""
Comment out external function calls that are not available in modular compilation.

This includes:
- LogManager_logFormat
- Encryption/Cipher functions
- Game-specific functions (GameWorld, CDataManager, etc.)
"""

import sys
import re
from pathlib import Path


def comment_external_calls(input_file, output_file=None):
    """
    Comment out lines containing external function calls.

    Args:
        input_file: Path to input file
        output_file: Path to output file (defaults to input_file if None)

    Returns:
        tuple: (lines_commented, total_lines)
    """
    if output_file is None:
        output_file = input_file

    external_patterns = [
        r'LogManager_logFormat',
        r'Encryption_',
        r'Cipher_',
        r'G_Cipher\(',
        r'G_GameWorld\(',
        r'G_CDataManager\(',
        r'GameWorld_',
        r'CDataManager_',
        r'UpgradeSeparateInfo_',
        r'RandomOption::',
        r'Mutex_',
        r'_NS_PI_MakeHash',
        r'__cxa_atexit',
        r'_dso_handle',
    ]

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    fixed_lines = []
    lines_commented = 0
    in_multiline_call = False
    multiline_indent = ""

    for i, line in enumerate(lines):
        stripped = line.strip()

        # Skip if already commented
        if stripped.startswith('//'):
            fixed_lines.append(line)
            continue

        # Check if line contains external function call
        contains_external = any(re.search(pattern, line) for pattern in external_patterns)

        if contains_external:
            # Start of external call
            if not in_multiline_call:
                multiline_indent = line[:len(line) - len(line.lstrip())]
                in_multiline_call = True

            # Comment out the line
            fixed_lines.append(f"{multiline_indent}// EXTERNAL: {line.lstrip()}")
            lines_commented += 1

            # Check if this line ends the call
            if ';' in line or ('{' in line and '}' in line):
                in_multiline_call = False
        elif in_multiline_call:
            # Continue commenting multiline call
            fixed_lines.append(f"{multiline_indent}// EXTERNAL: {line.lstrip()}")
            lines_commented += 1

            # Check if call ends
            if ';' in line or ('{' in line and '}' in line):
                in_multiline_call = False
        else:
            # Keep line as-is
            fixed_lines.append(line)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.writelines(fixed_lines)

    return lines_commented, len(lines)


def main():
    if len(sys.argv) < 2:
        print("Usage: python comment_external_calls.py <input_file> [output_file]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    if not Path(input_file).exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    print(f"Processing: {input_file}")
    lines_commented, total_lines = comment_external_calls(input_file, output_file)

    output = output_file or input_file
    print(f"Commented out {lines_commented} external call lines in {total_lines} total lines")
    print(f"Output written to: {output}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
