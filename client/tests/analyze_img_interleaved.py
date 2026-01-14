#!/usr/bin/env python3
"""
IMG analysis - Interleaved format (entry + data alternating)
"""

import os
import struct
import zlib

def read_img(npk_path: str, offset: int, size: int) -> bytes:
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)

def analyze_interleaved(data: bytes, name: str):
    """Parse IMG with interleaved entry/data format."""
    print(f"\n{'='*70}")
    print(f"{name} ({len(data)} bytes)")
    print(f"{'='*70}")

    magic = data[:18].rstrip(b'\x00')
    image_count = struct.unpack('<I', data[28:32])[0]
    print(f"Magic: {magic}")
    print(f"Image Count: {image_count}")

    # Entry starts at offset 32
    offset = 32
    frames = []

    print(f"\n{'Idx':>3} {'Offset':>8} {'Fmt':>4} {'Cmp':>3} {'W':>5} {'H':>5} {'DataSz':>8} {'KeyX':>6} {'KeyY':>6} {'Status':<10}")
    print("-" * 80)

    for i in range(image_count):
        if offset + 20 > len(data):
            print(f"  [{i}] Truncated at offset 0x{offset:X}")
            break

        # Read entry (32 or 36 bytes - try to detect)
        fmt = struct.unpack('<I', data[offset:offset+4])[0]
        cmp = struct.unpack('<I', data[offset+4:offset+8])[0]
        w = struct.unpack('<I', data[offset+8:offset+12])[0]
        h = struct.unpack('<I', data[offset+12:offset+16])[0]
        sz = struct.unpack('<I', data[offset+16:offset+20])[0]
        kx = struct.unpack('<i', data[offset+20:offset+24])[0] if offset+24 <= len(data) else 0
        ky = struct.unpack('<i', data[offset+24:offset+28])[0] if offset+28 <= len(data) else 0

        # Validate entry
        valid = fmt in [0, 14, 15, 16, 17] and cmp in [0, 5, 6] and w < 4096 and h < 4096

        if not valid:
            # Maybe it's a link frame (format=0, all zeros)
            if fmt == 0 and cmp == 0 and w == 0 and h == 0:
                valid = True
                status = "LINK"
                entry_size = 8  # Link frames might be smaller
            else:
                status = "INVALID"
                print(f"  [{i}] 0x{offset:X}: fmt={fmt} cmp={cmp} w={w} h={h} -- INVALID, stopping")
                break
        else:
            status = "OK"

        # Detect entry size by looking for zlib signature or next valid entry
        entry_size = 36  # default
        for test_size in [28, 32, 36, 40]:
            next_pos = offset + test_size
            if next_pos + 2 <= len(data):
                if data[next_pos:next_pos+2] in [b'\x78\x9C', b'\x78\x01', b'\x78\xDA']:
                    entry_size = test_size
                    break

        print(f"  {i:3d} 0x{offset:06X} {fmt:4d} {cmp:3d} {w:5d} {h:5d} {sz:8d} {kx:6d} {ky:6d} {status:<10}")

        frame = {
            'index': i, 'offset': offset, 'entry_size': entry_size,
            'format': fmt, 'compress': cmp, 'width': w, 'height': h, 'size': sz,
            'key_x': kx, 'key_y': ky
        }
        frames.append(frame)

        # Move to data section
        data_offset = offset + entry_size

        if fmt == 0:
            # Link frame - skip data
            offset = data_offset
        else:
            # Real frame - data follows entry
            offset = data_offset + sz

    # Try to decode first valid frame
    for frame in frames:
        if frame['format'] in [14, 15, 16] and frame['size'] > 0:
            print(f"\nDecoding frame {frame['index']}:")
            data_start = frame['offset'] + frame['entry_size']
            compressed = data[data_start:data_start + frame['size']]

            print(f"  Data at 0x{data_start:X}, size={frame['size']}")
            print(f"  First 16 bytes: {' '.join(f'{b:02X}' for b in compressed[:16])}")

            # Check for zlib header
            if compressed[:2] in [b'\x78\x9C', b'\x78\x01', b'\x78\xDA']:
                print(f"  Detected zlib header!")
                try:
                    decompressed = zlib.decompress(compressed)
                    print(f"  Decompressed: {len(decompressed)} bytes")

                    expected = frame['width'] * frame['height'] * 2
                    if len(decompressed) == expected:
                        print(f"  MATCH: 16-bit pixel data!")
                    expected4 = frame['width'] * frame['height'] * 4
                    if len(decompressed) == expected4:
                        print(f"  MATCH: 32-bit pixel data!")

                    print(f"  First 16 pixels: {' '.join(f'{b:02X}' for b in decompressed[:32])}")
                    break
                except Exception as e:
                    print(f"  Decompress error: {e}")
            else:
                print(f"  No zlib header, may be raw data")
            break

def main():
    from test_npk import parse_npk

    base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    npk_path = os.path.join(base, "sprite_character.NPK")

    npk = parse_npk(npk_path)

    for entry in npk['files'][:3]:
        if entry['name'].endswith('.img'):
            data = read_img(npk_path, entry['offset'], entry['size'])
            analyze_interleaved(data, entry['name'])

if __name__ == "__main__":
    main()
