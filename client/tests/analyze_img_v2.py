#!/usr/bin/env python3
"""
Precise IMG structure analysis
"""

import os
import struct
import zlib

def read_img_absolute(npk_path: str, offset: int, size: int) -> bytes:
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)

def analyze_img_precise(data: bytes, name: str):
    """Analyze IMG with precise offsets."""
    print(f"\n{'='*70}")
    print(f"Analyzing: {name} ({len(data)} bytes)")
    print(f"{'='*70}")

    # Raw header bytes
    print("\nRaw header (0x00-0x3F):")
    for i in range(0, min(64, len(data)), 16):
        hex_line = ' '.join(f'{data[i+j]:02X}' for j in range(min(16, len(data)-i)))
        print(f"  0x{i:02X}: {hex_line}")

    # Parse header
    magic = data[:18].rstrip(b'\x00')
    print(f"\nMagic: {magic}")

    # Header fields (based on SkillTreeViewer)
    index_size = struct.unpack('<I', data[16:20])[0]
    unknown = struct.unpack('<I', data[20:24])[0]
    version = struct.unpack('<I', data[24:28])[0]
    image_count = struct.unpack('<I', data[28:32])[0]

    print(f"Index Size: {index_size}")
    print(f"Unknown: {unknown}")
    print(f"Version: {version}")
    print(f"Image Count: {image_count}")

    if image_count > 1000:
        print("ERROR: Image count too large!")
        return

    # Calculate offsets
    entry_size = 36
    entries_start = 32
    data_start = entries_start + image_count * entry_size

    print(f"\nEntries start: 0x{entries_start:02X} ({entries_start})")
    print(f"Data starts: 0x{data_start:02X} ({data_start})")

    # Parse entries
    print(f"\n{'Idx':>3} {'Fmt':>4} {'Cmp':>3} {'W':>5} {'H':>5} {'Size':>8} {'KeyX':>6} {'KeyY':>6} {'MaxW':>5} {'MaxH':>5}")
    print("-" * 70)

    valid_entries = []
    for i in range(image_count):
        off = entries_start + i * entry_size
        if off + entry_size > len(data):
            print(f"  Entry {i} truncated!")
            break

        fmt = struct.unpack('<I', data[off:off+4])[0]
        cmp = struct.unpack('<I', data[off+4:off+8])[0]
        w = struct.unpack('<I', data[off+8:off+12])[0]
        h = struct.unpack('<I', data[off+12:off+16])[0]
        sz = struct.unpack('<I', data[off+16:off+20])[0]
        kx = struct.unpack('<i', data[off+20:off+24])[0]
        ky = struct.unpack('<i', data[off+24:off+28])[0]
        mw = struct.unpack('<I', data[off+28:off+32])[0]
        mh = struct.unpack('<I', data[off+32:off+36])[0]

        # Mark valid entries
        valid = fmt in [0, 14, 15, 16, 17] and cmp in [0, 5, 6] and w < 4096 and h < 4096
        marker = "" if valid else " !!!"

        print(f"{i:3d} {fmt:4d} {cmp:3d} {w:5d} {h:5d} {sz:8d} {kx:6d} {ky:6d} {mw:5d} {mh:5d}{marker}")

        if valid:
            valid_entries.append({
                'index': i, 'format': fmt, 'compress': cmp,
                'width': w, 'height': h, 'size': sz,
                'key_x': kx, 'key_y': ky, 'max_w': mw, 'max_h': mh
            })

    # Check data section
    print(f"\nData section starts at 0x{data_start:02X}:")
    if data_start < len(data):
        print(f"  First 16 bytes: {' '.join(f'{data[data_start+i]:02X}' for i in range(min(16, len(data)-data_start)))}")

        # Check for zlib magic
        if data[data_start:data_start+2] == b'\x78\x9C':
            print("  Detected: zlib compressed data (78 9C)")
        elif data[data_start:data_start+2] == b'\x78\x01':
            print("  Detected: zlib compressed data (78 01)")

    # Try to decompress first image
    if valid_entries:
        entry = valid_entries[0]
        if entry['size'] > 0 and entry['compress'] == 6:
            print(f"\nDecompressing frame 0...")

            # Calculate offset
            img_offset = data_start
            compressed = data[img_offset:img_offset + entry['size']]

            print(f"  Compressed size: {len(compressed)}")
            print(f"  First 8 bytes: {' '.join(f'{b:02X}' for b in compressed[:8])}")

            try:
                decompressed = zlib.decompress(compressed)
                print(f"  Decompressed: {len(decompressed)} bytes")

                expected_16 = entry['width'] * entry['height'] * 2
                expected_32 = entry['width'] * entry['height'] * 4

                if len(decompressed) == expected_16:
                    print(f"  MATCH: 16-bit format ({expected_16} bytes)")
                elif len(decompressed) == expected_32:
                    print(f"  MATCH: 32-bit format ({expected_32} bytes)")
                else:
                    print(f"  Expected: 16-bit={expected_16}, 32-bit={expected_32}")

                # Show first pixels
                print(f"  First 16 bytes: {' '.join(f'{b:02X}' for b in decompressed[:16])}")

            except Exception as e:
                print(f"  Decompress error: {e}")

def main():
    from test_npk import parse_npk

    base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    npk_path = os.path.join(base, "sprite_character.NPK")

    npk = parse_npk(npk_path)

    for entry in npk['files'][:2]:
        if entry['name'].endswith('.img'):
            data = read_img_absolute(npk_path, entry['offset'], entry['size'])
            analyze_img_precise(data, entry['name'])

if __name__ == "__main__":
    main()
