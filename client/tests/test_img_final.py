#!/usr/bin/env python3
"""
IMG Decoder Test - Final Version with correct absolute offset
"""

import os
import sys
import struct
import zlib
from test_npk import parse_npk, DECRYPT_KEY

def read_img_absolute(npk_path: str, offset: int, size: int) -> bytes:
    """Read IMG data using absolute offset."""
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)

def parse_img(data: bytes) -> dict:
    """Parse IMG file structure."""
    result = {
        'valid': False,
        'magic': None,
        'version': 0,
        'image_count': 0,
        'images': [],
        'error': None
    }

    if len(data) < 32:
        result['error'] = "Data too short"
        return result

    # Check magic
    magic = data[:18].rstrip(b'\x00')
    result['magic'] = magic

    if not magic.startswith(b'Neople'):
        result['error'] = f"Invalid magic: {magic}"
        return result

    # Parse header (starting at offset 18 for "Neople Image File")
    # Or offset 16 for "Neople Img File"
    header_offset = 18 if magic == b'Neople Image File' else 16

    # The header structure varies by version, let's detect it
    # Common layout after magic:
    # - index_size: 4 bytes
    # - reserved/version info
    # - image_count: 4 bytes

    # Try to find image_count by scanning
    # Usually at offset 28-32 range
    for test_offset in [28, 24, 32, 20]:
        if test_offset + 4 <= len(data):
            count = struct.unpack('<I', data[test_offset:test_offset+4])[0]
            if 0 < count < 10000:
                result['header_offset'] = test_offset
                result['image_count'] = count
                break

    if result['image_count'] == 0:
        result['error'] = "Could not find image count"
        return result

    result['valid'] = True

    # Parse entries - each entry is typically 36 bytes
    entry_start = result['header_offset'] + 4
    entry_size = 36

    for i in range(min(result['image_count'], 500)):
        offset = entry_start + i * entry_size
        if offset + entry_size > len(data):
            break

        entry = {
            'index': i,
            'format': struct.unpack('<I', data[offset:offset+4])[0],
            'compress': struct.unpack('<I', data[offset+4:offset+8])[0],
            'width': struct.unpack('<I', data[offset+8:offset+12])[0],
            'height': struct.unpack('<I', data[offset+12:offset+16])[0],
            'data_size': struct.unpack('<I', data[offset+16:offset+20])[0],
            'key_x': struct.unpack('<i', data[offset+20:offset+24])[0],
            'key_y': struct.unpack('<i', data[offset+24:offset+28])[0],
        }

        # Validate entry
        if entry['width'] > 0 and entry['height'] > 0 and entry['data_size'] > 0:
            result['images'].append(entry)
        elif entry['format'] == 0 and entry['compress'] == 0:
            # Link frame - skip
            result['images'].append(entry)

    return result

def decompress_zlib(data: bytes) -> bytes:
    """Decompress zlib data."""
    try:
        return zlib.decompress(data)
    except:
        try:
            return zlib.decompress(data[2:], -zlib.MAX_WBITS)
        except:
            try:
                return zlib.decompress(data, -zlib.MAX_WBITS)
            except:
                return data

def extract_frame(img_data: bytes, img_info: dict, frame_idx: int) -> bytes:
    """Extract raw pixel data for a frame."""
    if frame_idx >= len(img_info['images']):
        return b''

    entry = img_info['images'][frame_idx]
    if entry['data_size'] <= 0:
        return b''

    # Calculate data section start
    entry_start = img_info['header_offset'] + 4
    entry_size = 36
    data_start = entry_start + img_info['image_count'] * entry_size

    # Calculate offset for this frame
    offset = data_start
    for i in range(frame_idx):
        offset += img_info['images'][i]['data_size']

    # Extract data
    compressed = img_data[offset:offset + entry['data_size']]

    # Decompress if zlib
    if entry['compress'] == 6:
        return decompress_zlib(compressed)
    else:
        return compressed

def get_format_name(fmt: int) -> str:
    return {14: "ARGB1555", 15: "ARGB4444", 16: "ARGB8888", 17: "INDEX8"}.get(fmt, f"Unk({fmt})")

def test_npk_img(npk_path: str):
    """Test IMG decoding from NPK."""
    print("=" * 60)
    print(f"  Testing: {os.path.basename(npk_path)}")
    print("=" * 60)

    if not os.path.exists(npk_path):
        print(f"ERROR: Not found")
        return

    # Parse NPK
    npk = parse_npk(npk_path)
    if npk['errors']:
        print(f"NPK Error: {npk['errors']}")
        return

    print(f"Files: {npk['file_count']}")

    # Process first IMG file
    for entry in npk['files'][:3]:
        if not entry['name'].endswith('.img'):
            continue

        print(f"\n--- IMG: {entry['name']} ---")

        # Read using ABSOLUTE offset
        img_data = read_img_absolute(npk_path, entry['offset'], entry['size'])
        print(f"Size: {len(img_data)} bytes")
        print(f"Header: {img_data[:32]}")

        # Parse
        img_info = parse_img(img_data)

        if not img_info['valid']:
            print(f"Error: {img_info['error']}")
            continue

        print(f"Magic: {img_info['magic']}")
        print(f"Images: {img_info['image_count']}")

        # Show entries
        print(f"\n{'Idx':>4} {'Fmt':>8} {'Cmp':>4} {'W':>5} {'H':>5} {'Size':>8} {'KeyX':>6} {'KeyY':>6}")
        for img in img_info['images'][:10]:
            print(f"{img['index']:4d} {get_format_name(img['format']):>8} {img['compress']:4d} "
                  f"{img['width']:5d} {img['height']:5d} {img['data_size']:8d} "
                  f"{img['key_x']:6d} {img['key_y']:6d}")

        # Extract first valid frame
        for img in img_info['images'][:5]:
            if img['width'] > 0 and img['height'] > 0 and img['data_size'] > 0:
                print(f"\nExtracting frame {img['index']}...")
                raw = extract_frame(img_data, img_info, img['index'])
                print(f"  Decompressed: {len(raw)} bytes")
                print(f"  Expected 16-bit: {img['width'] * img['height'] * 2}")
                print(f"  Expected 32-bit: {img['width'] * img['height'] * 4}")

                if raw:
                    print(f"  First 32 bytes: {' '.join(f'{b:02X}' for b in raw[:32])}")

                    # Check for valid pixel data
                    if len(raw) == img['width'] * img['height'] * 2:
                        print("  SUCCESS: Valid 16-bit image data!")
                    elif len(raw) == img['width'] * img['height'] * 4:
                        print("  SUCCESS: Valid 32-bit image data!")
                break

def main():
    base = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    test_files = [
        "sprite_character.NPK",
        "sprite.NPK",
    ]

    for f in test_files:
        path = os.path.join(base, f)
        if os.path.exists(path):
            test_npk_img(path)
            print()

if __name__ == "__main__":
    main()
