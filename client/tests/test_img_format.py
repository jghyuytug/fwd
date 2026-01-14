#!/usr/bin/env python3
"""
IMG Format Analysis

Analyze the structure of IMG files extracted from NPK archives.
"""

import os
import sys
import struct
import zlib

# Import from test_npk
from test_npk import parse_npk, read_file_data, DECRYPT_KEY

def analyze_img_data(data: bytes, name: str):
    """Analyze IMG file data."""
    print(f"\n=== Analyzing: {name} ===")
    print(f"Size: {len(data)} bytes")

    # Show first 64 bytes
    print("First 64 bytes (hex):")
    for i in range(0, min(64, len(data)), 16):
        hex_line = ' '.join(f'{b:02X}' for b in data[i:i+16])
        ascii_line = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data[i:i+16])
        print(f"  {i:04X}: {hex_line:<48} {ascii_line}")

    # Try different interpretations
    print("\nPossible interpretations:")

    # Check for zlib compression
    if len(data) >= 2:
        if data[0] == 0x78:  # zlib header
            if data[1] in (0x01, 0x5E, 0x9C, 0xDA):
                print("  - Possible zlib compressed data (header 0x78)")
                try:
                    decompressed = zlib.decompress(data)
                    print(f"  - Decompressed: {len(decompressed)} bytes")
                    print("  - First 32 bytes of decompressed:")
                    hex_preview = ' '.join(f'{b:02X}' for b in decompressed[:32])
                    print(f"    {hex_preview}")

                    # Check if decompressed is IMG
                    if decompressed.startswith(b'Neople'):
                        print("  - FOUND: Neople IMG signature after decompression!")
                    return decompressed
                except Exception as e:
                    print(f"  - Decompress failed: {e}")

    # Try XOR decrypt with key (like filename)
    print("\nTrying XOR decrypt with NPK key...")
    decrypted = bytearray(min(64, len(data)))
    for i in range(len(decrypted)):
        decrypted[i] = data[i] ^ DECRYPT_KEY[i % 256]

    print("XOR decrypted first 64 bytes:")
    for i in range(0, len(decrypted), 16):
        hex_line = ' '.join(f'{b:02X}' for b in decrypted[i:i+16])
        ascii_line = ''.join(chr(b) if 32 <= b < 127 else '.' for b in decrypted[i:i+16])
        print(f"  {i:04X}: {hex_line:<48} {ascii_line}")

    if decrypted.startswith(b'Neople'):
        print("  - FOUND: Neople signature after XOR decrypt!")
        return bytes(decrypted)

    # Try reading as raw IMG structure
    if len(data) >= 20:
        print("\nTrying to parse as raw IMG header...")
        # Try little-endian reads
        try:
            magic = data[:16]
            if len(data) >= 20:
                v1 = struct.unpack('<I', data[16:20])[0]
                print(f"  Offset 16 (uint32): {v1} (0x{v1:08X})")
            if len(data) >= 24:
                v2 = struct.unpack('<I', data[20:24])[0]
                print(f"  Offset 20 (uint32): {v2} (0x{v2:08X})")
            if len(data) >= 28:
                v3 = struct.unpack('<I', data[24:28])[0]
                print(f"  Offset 24 (uint32): {v3} (0x{v3:08X})")
        except Exception as e:
            print(f"  Parse error: {e}")

    return None

def analyze_npk_imgs(npk_path: str, max_files: int = 5):
    """Analyze IMG files from an NPK archive."""
    print("=" * 60)
    print("  IMG Format Analysis")
    print("=" * 60)

    if not os.path.exists(npk_path):
        print(f"ERROR: File not found: {npk_path}")
        return

    result = parse_npk(npk_path)
    if result['errors']:
        print(f"NPK Error: {result['errors']}")
        return

    print(f"\nNPK: {npk_path}")
    print(f"Files: {result['file_count']}")

    for entry in result['files'][:max_files]:
        if entry['name'].endswith('.img'):
            data = read_file_data(npk_path, entry, result['file_count'])
            analyze_img_data(data, entry['name'])

def check_multiple_npks():
    """Check IMG format across multiple NPKs."""
    base_path = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"

    npks = [
        "sprite_character.NPK",
        "sprite.NPK",
        "sprite_interface_exui.NPK"
    ]

    for npk in npks:
        path = os.path.join(base_path, npk)
        if os.path.exists(path):
            analyze_npk_imgs(path, max_files=2)

def main():
    base_path = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    default_npk = os.path.join(base_path, "sprite_character.NPK")

    npk_path = sys.argv[1] if len(sys.argv) > 1 else default_npk

    analyze_npk_imgs(npk_path, max_files=3)

    print("\n" + "=" * 60)
    print("Analysis Complete")
    print("=" * 60)

if __name__ == "__main__":
    main()
