#!/usr/bin/env python3
"""
Analyze IMG structure by examining raw bytes
"""

import os
import struct

def read_img_absolute(npk_path: str, offset: int, size: int) -> bytes:
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)

def hexdump(data: bytes, start: int = 0, length: int = 256, cols: int = 16):
    """Print hexdump of data."""
    for i in range(0, min(length, len(data)), cols):
        hex_part = ' '.join(f'{data[i+j]:02X}' for j in range(min(cols, len(data)-i)))
        ascii_part = ''.join(chr(data[i+j]) if 32 <= data[i+j] < 127 else '.'
                            for j in range(min(cols, len(data)-i)))
        print(f"  {start+i:04X}: {hex_part:<48} {ascii_part}")

def analyze_img(data: bytes, name: str):
    """Analyze IMG structure."""
    print(f"\n{'='*60}")
    print(f"Analyzing: {name}")
    print(f"Size: {len(data)} bytes")
    print(f"{'='*60}")

    # Header
    print("\n--- Header (first 32 bytes) ---")
    hexdump(data, 0, 32)

    magic = data[:18].rstrip(b'\x00')
    print(f"\nMagic: {magic}")

    # After magic, parse version info
    print("\n--- After magic (offset 18-32) ---")
    for off in range(18, 32, 4):
        val = struct.unpack('<I', data[off:off+4])[0]
        print(f"  Offset {off}: {val} (0x{val:08X})")

    # Image count seems to be at offset 28
    image_count = struct.unpack('<I', data[28:32])[0]
    print(f"\nImage count (at offset 28): {image_count}")

    # Analyze frame entries starting at offset 32
    print("\n--- Frame entries (starting at offset 32) ---")
    print("Offset 32 raw bytes:")
    hexdump(data, 32, 144)

    # Try different entry sizes
    for entry_size in [20, 24, 28, 32, 36, 40]:
        print(f"\n--- Trying entry size {entry_size} ---")
        valid_count = 0
        for i in range(min(5, image_count)):
            offset = 32 + i * entry_size
            if offset + entry_size > len(data):
                break

            fmt = struct.unpack('<I', data[offset:offset+4])[0]
            cmp = struct.unpack('<I', data[offset+4:offset+8])[0]
            w = struct.unpack('<I', data[offset+8:offset+12])[0]
            h = struct.unpack('<I', data[offset+12:offset+16])[0]
            sz = struct.unpack('<I', data[offset+16:offset+20])[0]

            # Check if values look reasonable
            if fmt in [14, 15, 16, 17, 0] and cmp in [0, 5, 6] and w < 4096 and h < 4096:
                valid_count += 1
                print(f"  [{i}] fmt={fmt}, cmp={cmp}, w={w}, h={h}, sz={sz} [VALID]")
            else:
                print(f"  [{i}] fmt={fmt}, cmp={cmp}, w={w}, h={h}, sz={sz} [INVALID]")

        if valid_count >= 3:
            print(f"  -> Entry size {entry_size} looks correct!")
            return entry_size

    return 36

def main():
    base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"

    # First, get the NPK file list
    from test_npk import parse_npk

    npk_path = os.path.join(base, "sprite_character.NPK")
    npk = parse_npk(npk_path)

    for entry in npk['files'][:2]:
        if entry['name'].endswith('.img'):
            data = read_img_absolute(npk_path, entry['offset'], entry['size'])
            analyze_img(data, entry['name'])

if __name__ == "__main__":
    main()
