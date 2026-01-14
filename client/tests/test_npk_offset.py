#!/usr/bin/env python3
"""
NPK Offset Analysis - Compare relative vs absolute offset
"""

import os
import struct
from test_npk import DECRYPT_KEY

NPK_MAGIC_LENGTH = 16
NPK_NAME_LENGTH = 256
NPK_ENTRY_SIZE = 264

def analyze_npk_offsets(npk_path: str):
    """Analyze NPK file offsets."""
    print(f"=== Analyzing: {os.path.basename(npk_path)} ===")

    with open(npk_path, 'rb') as f:
        # Read magic
        magic = f.read(16)
        print(f"Magic: {magic}")

        # Read file count
        file_count = struct.unpack('<I', f.read(4))[0]
        print(f"File count: {file_count}")

        # Calculate expected data start
        header_size = 16 + 4
        entries_size = file_count * 264
        data_start = header_size + entries_size
        print(f"Header size: {header_size}")
        print(f"Entries size: {entries_size}")
        print(f"Expected data start: {data_start} (0x{data_start:X})")

        # Read first few entries
        print(f"\nFirst 3 entries:")
        for i in range(min(3, file_count)):
            offset = struct.unpack('<I', f.read(4))[0]
            size = struct.unpack('<I', f.read(4))[0]
            encrypted_name = f.read(256)

            # Decrypt name
            decrypted = bytearray(256)
            for j in range(256):
                decrypted[j] = encrypted_name[j] ^ DECRYPT_KEY[j]
            name = decrypted.rstrip(b'\x00').decode('utf-8', errors='replace')

            print(f"  [{i}] offset={offset} (0x{offset:X}), size={size}, name={name[:50]}")

            # Check what's at that offset (absolute)
            current_pos = f.tell()

            # Try absolute offset
            f.seek(offset)
            abs_data = f.read(min(32, size))
            print(f"      At absolute {offset}: {' '.join(f'{b:02X}' for b in abs_data[:16])}")

            # Try relative offset (data_start + offset)
            rel_offset = data_start + offset
            if rel_offset < os.path.getsize(npk_path):
                f.seek(rel_offset)
                rel_data = f.read(min(32, size))
                print(f"      At relative {rel_offset}: {' '.join(f'{b:02X}' for b in rel_data[:16])}")

            # Check if either starts with "Neople"
            if abs_data.startswith(b'Neople'):
                print(f"      -> Absolute offset is correct (Neople magic found)!")
            elif len(rel_data) >= 6 and rel_data.startswith(b'Neople'):
                print(f"      -> Relative offset is correct (Neople magic found)!")
            else:
                print(f"      -> Neither has Neople magic - data may be encrypted")

            f.seek(current_pos)

        # Check file size
        file_size = os.path.getsize(npk_path)
        print(f"\nFile size: {file_size} bytes")

def main():
    base_path = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    npk_files = [
        "sprite_character.NPK",
        "sprite.NPK"
    ]

    for npk_name in npk_files:
        npk_path = os.path.join(base_path, npk_name)
        if os.path.exists(npk_path):
            analyze_npk_offsets(npk_path)
            print()

if __name__ == "__main__":
    main()
