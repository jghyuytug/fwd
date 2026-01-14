#!/usr/bin/env python3
"""
IMG Decoder Test - Based on SkillTreeViewer NpkService.cs

Tests IMG decoding using real Taiwan DNF files.
"""

import os
import sys
import struct
import zlib
from test_npk import parse_npk, DECRYPT_KEY

# IMG Constants
IMG_MAGIC = b"Neople Img File"

def read_npk_img_data(npk_path: str, entry: dict) -> bytes:
    """Read IMG data directly from NPK at the given offset."""
    with open(npk_path, 'rb') as f:
        # The offset in entry is absolute position in file
        # But we need to add header offset
        # Header: 16 bytes magic + 4 bytes count
        # Entries: file_count * 264 bytes
        # Then data starts

        # First read file count to calculate data start
        f.seek(16)
        file_count = struct.unpack('<I', f.read(4))[0]

        # Calculate data section start
        header_size = 16 + 4  # magic + count
        entries_size = file_count * 264
        data_start = header_size + entries_size

        # Seek to absolute position
        absolute_offset = data_start + entry['offset']
        f.seek(absolute_offset)
        return f.read(entry['size'])

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
    magic = data[:16].rstrip(b'\x00')
    result['magic'] = magic

    if not magic.startswith(b'Neople'):
        result['error'] = f"Invalid magic: {magic}"
        return result

    # Parse header
    # Offset 16: indexSize (4 bytes)
    # Offset 20: unknown (4 bytes)
    # Offset 24: version (4 bytes)
    # Offset 28: imageCount (4 bytes)
    index_size = struct.unpack('<I', data[16:20])[0]
    unknown = struct.unpack('<I', data[20:24])[0]
    version = struct.unpack('<I', data[24:28])[0]
    image_count = struct.unpack('<I', data[28:32])[0]

    result['index_size'] = index_size
    result['version'] = version
    result['image_count'] = image_count
    result['valid'] = True

    # Parse image entries (36 bytes each)
    for i in range(min(image_count, 100)):  # Limit to 100 for safety
        offset = 32 + i * 36
        if offset + 36 > len(data):
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
            'max_w': struct.unpack('<I', data[offset+28:offset+32])[0],
            'max_h': struct.unpack('<I', data[offset+32:offset+36])[0],
        }
        result['images'].append(entry)

    return result

def decompress_zlib(data: bytes) -> bytes:
    """Decompress zlib data (skip 2-byte header)."""
    try:
        # Try with header
        return zlib.decompress(data)
    except:
        try:
            # Try without header (raw deflate)
            return zlib.decompress(data, -zlib.MAX_WBITS)
        except:
            try:
                # Skip 2-byte zlib header
                return zlib.decompress(data[2:], -zlib.MAX_WBITS)
            except Exception as e:
                print(f"Decompress failed: {e}")
                return data

def extract_image_data(img_data: bytes, img_info: dict, image_index: int) -> bytes:
    """Extract raw pixel data for a specific image."""
    image_count = img_info['image_count']

    if image_index >= len(img_info['images']):
        return b''

    entry = img_info['images'][image_index]

    # Calculate data offset
    data_offset = 32 + image_count * 36

    # Sum up sizes of previous images
    for i in range(image_index):
        data_offset += img_info['images'][i]['data_size']

    # Extract compressed data
    compressed = img_data[data_offset:data_offset + entry['data_size']]

    # Decompress if needed
    if entry['compress'] == 6:  # zlib
        return decompress_zlib(compressed)
    else:
        return compressed

def get_format_name(fmt: int) -> str:
    """Get pixel format name."""
    formats = {
        14: "ARGB1555",
        15: "ARGB4444",
        16: "ARGB8888",
        17: "INDEX8"
    }
    return formats.get(fmt, f"Unknown({fmt})")

def test_img_from_npk(npk_path: str, max_imgs: int = 3):
    """Test IMG decoding from NPK file."""
    print("=" * 60)
    print("  IMG Decoder Test")
    print("=" * 60)

    if not os.path.exists(npk_path):
        print(f"ERROR: File not found: {npk_path}")
        return

    # Parse NPK
    npk = parse_npk(npk_path)
    if npk['errors']:
        print(f"NPK Error: {npk['errors']}")
        return

    print(f"\nNPK: {os.path.basename(npk_path)}")
    print(f"Files: {npk['file_count']}")

    # Process IMG files
    img_count = 0
    for entry in npk['files']:
        if not entry['name'].endswith('.img'):
            continue

        if img_count >= max_imgs:
            break
        img_count += 1

        print(f"\n{'='*50}")
        print(f"IMG: {entry['name']}")
        print(f"Size: {entry['size']} bytes, Offset: 0x{entry['offset']:X}")

        # Read IMG data
        img_data = read_npk_img_data(npk_path, entry)
        print(f"Read: {len(img_data)} bytes")

        # Show first 32 bytes
        print(f"First 32 bytes: {' '.join(f'{b:02X}' for b in img_data[:32])}")

        # Parse IMG
        img_info = parse_img(img_data)

        if not img_info['valid']:
            print(f"Parse Error: {img_info['error']}")
            continue

        print(f"Magic: {img_info['magic']}")
        print(f"Version: {img_info['version']}")
        print(f"Image Count: {img_info['image_count']}")

        # Show first 5 image entries
        print(f"\nImage Entries (first 5):")
        print(f"{'Idx':>4} {'Format':>10} {'Compress':>8} {'Width':>6} {'Height':>6} {'Size':>8}")
        print("-" * 50)

        for img_entry in img_info['images'][:5]:
            fmt_name = get_format_name(img_entry['format'])
            print(f"{img_entry['index']:4d} {fmt_name:>10} {img_entry['compress']:8d} "
                  f"{img_entry['width']:6d} {img_entry['height']:6d} {img_entry['data_size']:8d}")

        # Try to extract and decode first image
        if img_info['images']:
            first = img_info['images'][0]
            print(f"\nExtracting first image...")
            print(f"  Format: {get_format_name(first['format'])}")
            print(f"  Size: {first['width']}x{first['height']}")
            print(f"  Compressed size: {first['data_size']}")

            raw_data = extract_image_data(img_data, img_info, 0)
            print(f"  Decompressed: {len(raw_data)} bytes")

            expected_16bit = first['width'] * first['height'] * 2
            expected_32bit = first['width'] * first['height'] * 4
            print(f"  Expected 16-bit: {expected_16bit}, 32-bit: {expected_32bit}")

            if len(raw_data) == expected_16bit:
                print(f"  Match: 16-bit format!")
            elif len(raw_data) == expected_32bit:
                print(f"  Match: 32-bit format!")
            else:
                print(f"  WARNING: Size mismatch!")

            # Show first 32 bytes of raw data
            if raw_data:
                print(f"  Raw data (first 32 bytes): {' '.join(f'{b:02X}' for b in raw_data[:32])}")

def main():
    # Test with sprite_character.NPK
    base_path = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"

    # Test different NPK files
    test_files = [
        "sprite_character.NPK",  # Simple character sprites
        "sprite_character_swordman_effect.NPK",  # Skill icons
    ]

    for npk_name in test_files:
        npk_path = os.path.join(base_path, npk_name)
        if os.path.exists(npk_path):
            test_img_from_npk(npk_path, max_imgs=2)
        else:
            print(f"SKIP: {npk_name} not found")

    print("\n" + "=" * 60)
    print("Test Complete")
    print("=" * 60)

if __name__ == "__main__":
    main()
