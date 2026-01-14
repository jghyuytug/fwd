#!/usr/bin/env python3
"""
Fix remaining compilation errors in PacketBuf.cpp
"""

import sys
import re

def fix_remaining_errors(input_file):
    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Fix 1: Comment out unused Encryption/GameWorld/CDataManager variable declarations
    fixes = [
        (r'(\s+)Encryption \*v1;(.+?\n)', r'\1// REMOVED: Encryption *v1; - not used without external calls\n'),
        (r'(\s+)Encryption \*v2;(.+?\n)', r'\1// REMOVED: Encryption *v2; - not used without external calls\n'),
        (r'(\s+)GameWorld \*v2;(.+?\n)', r'\1// REMOVED: GameWorld *v2; - not used without external calls\n'),
        (r'(\s+)CDataManager \*v3;(.+?\n)', r'\1// REMOVED: CDataManager *v3; - not used without external calls\n'),
        (r'(\s+)Cipher \*v2;(.+?\n)', r'\1// REMOVED: Cipher *v2; - not used without external calls\n'),
        (r'(\s+)Cipher \*v6;(.+?\n)', r'\1// REMOVED: Cipher *v6; - not used without external calls\n'),
    ]

    for pattern, replacement in fixes:
        content = re.sub(pattern, replacement, content)

    # Fix 2: Fix compress_packet unsigned __int64 issue
    content = re.sub(
        r'(\s+)unsigned __int64 n;',
        r'\1uint64_t n;',
        content
    )

    # Fix 3: Fix encrypt_packet variable declarations
    # Comment out problematic variable declarations in encrypt_packet
    lines = content.split('\n')
    in_encrypt_packet = False
    fixed_lines = []

    for i, line in enumerate(lines):
        if 'void __cdecl PacketBuf_encrypt_packet(PacketBuf *self)' in line:
            in_encrypt_packet = True
            fixed_lines.append(line)
        elif in_encrypt_packet and line.strip().startswith('int id;'):
            # Comment out duplicate id declarations
            indent = line[:len(line) - len(line.lstrip())]
            fixed_lines.append(f'{indent}// REMOVED: int id; - duplicate')
        elif in_encrypt_packet and (
            'unsigned __int8 *v3;' in line or
            'const unsigned __int8 *v4;' in line or
            'int id;' in line or
            'char flag;' in line
        ):
            # Keep first set, comment out rest based on context
            fixed_lines.append(line)
        elif in_encrypt_packet and '}' in line and 'void __cdecl' in lines[min(i+1, len(lines)-1)]:
            # End of function
            in_encrypt_packet = False
            fixed_lines.append(line)
        else:
            fixed_lines.append(line)

    content = '\n'.join(fixed_lines)

    # Write back
    with open(input_file, 'w', encoding='utf-8') as f:
        f.write(content)

    return True

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python fix_remaining_packetbuf_errors.py <file>")
        sys.exit(1)

    file_path = sys.argv[1]
    fix_remaining_errors(file_path)
    print(f"Fixed remaining errors in {file_path}")
