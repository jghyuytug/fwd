#!/usr/bin/env python3
"""
IMG analysis - try 32-byte entries
"""

import os
import struct
import zlib

def read_img(npk_path: str, offset: int, size: int) -> bytes:
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)

def try_entry_size(data: bytes, entry_size: int, image_count: int):
    """Try parsing with given entry size."""
    entries_start = 32
    valid = 0

    for i in range(image_count):
        off = entries_start + i * entry_size
        if off + 20 > len(data):
            break

        fmt = struct.unpack('<I', data[off:off+4])[0]
        cmp = struct.unpack('<I', data[off+4:off+8])[0]
        w = struct.unpack('<I', data[off+8:off+12])[0]
        h = struct.unpack('<I', data[off+12:off+16])[0]
        sz = struct.unpack('<I', data[off+16:off+20])[0]

        if fmt in [0, 14, 15, 16, 17] and cmp in [0, 5, 6] and w < 4096 and h < 4096 and sz < 10000000:
            valid += 1

    return valid

def analyze_img(data: bytes, name: str):
    """Analyze IMG with multiple entry sizes."""
    print(f"\n{'='*70}")
    print(f"{name} ({len(data)} bytes)")
    print(f"{'='*70}")

    # Parse header
    magic = data[:18].rstrip(b'\x00')
    image_count = struct.unpack('<I', data[28:32])[0]
    print(f"Magic: {magic}")
    print(f"Image Count: {image_count}")

    # Show raw hex at key offsets
    print("\nRaw bytes around entries:")
    for start in [0x20, 0x40, 0x60, 0x80]:
        if start + 32 <= len(data):
            hex_line = ' '.join(f'{data[start+i]:02X}' for i in range(32))
            print(f"  0x{start:02X}: {hex_line}")

    # Try different entry sizes
    print("\nTesting entry sizes:")
    for entry_size in [20, 24, 28, 32, 36, 40]:
        valid = try_entry_size(data, entry_size, image_count)
        data_start = 32 + image_count * entry_size

        # Check if data_start has zlib signature
        has_zlib = False
        if data_start + 2 <= len(data):
            if data[data_start:data_start+2] in [b'\x78\x9C', b'\x78\x01', b'\x78\xDA']:
                has_zlib = True

        marker = ""
        if valid == image_count:
            marker = " <-- ALL VALID!"
        elif has_zlib:
            marker = " <-- ZLIB at data start!"

        print(f"  Size {entry_size}: {valid}/{image_count} valid, data@0x{data_start:X}{marker}")

    # Try entry_size=32 in detail
    entry_size = 32
    entries_start = 32
    data_start = 32 + image_count * entry_size

    print(f"\nUsing entry_size={entry_size}, data_start=0x{data_start:X}:")

    total_size = 0
    for i in range(min(5, image_count)):
        off = entries_start + i * entry_size
        if off + 20 > len(data):
            break

        fmt = struct.unpack('<I', data[off:off+4])[0]
        cmp = struct.unpack('<I', data[off+4:off+8])[0]
        w = struct.unpack('<I', data[off+8:off+12])[0]
        h = struct.unpack('<I', data[off+12:off+16])[0]
        sz = struct.unpack('<I', data[off+16:off+20])[0]
        kx = struct.unpack('<i', data[off+20:off+24])[0]
        ky = struct.unpack('<i', data[off+24:off+28])[0]
        mw = struct.unpack('<I', data[off+28:off+32])[0] if off+32 <= len(data) else 0

        print(f"  [{i}] fmt={fmt}, cmp={cmp}, w={w}, h={h}, sz={sz}, kx={kx}, ky={ky}")
        total_size += sz

    # Try to decompress
    print(f"\nData section (0x{data_start:X}):")
    if data_start < len(data):
        print(f"  First 32 bytes: {' '.join(f'{data[data_start+i]:02X}' for i in range(min(32, len(data)-data_start)))}")

        # First entry
        entry0_size = struct.unpack('<I', data[entries_start+16:entries_start+20])[0]
        compressed = data[data_start:data_start+entry0_size]

        print(f"\n  Trying to decompress entry 0 ({entry0_size} bytes)...")
        for skip in [0, 2]:  # Try with/without header
            try:
                if skip:
                    dec = zlib.decompress(compressed[skip:], -zlib.MAX_WBITS)
                else:
                    dec = zlib.decompress(compressed)
                print(f"    Skip {skip}: SUCCESS! {len(dec)} bytes")
                print(f"    First 16: {' '.join(f'{b:02X}' for b in dec[:16])}")
                break
            except Exception as e:
                print(f"    Skip {skip}: {e}")

def main():
    from test_npk import parse_npk

    base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    npk_path = os.path.join(base, "sprite_character.NPK")

    npk = parse_npk(npk_path)

    for entry in npk['files'][:3]:
        if entry['name'].endswith('.img'):
            data = read_img(npk_path, entry['offset'], entry['size'])
            analyze_img(data, entry['name'])

if __name__ == "__main__":
    main()
