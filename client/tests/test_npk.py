#!/usr/bin/env python3
"""
NPK Parser Test - Python Implementation

Tests the NPK parsing logic using real Taiwan DNF files.
This validates our C implementation's algorithm.

Usage: python test_npk.py [npk_file]
"""

import os
import sys
import struct

# NPK Constants
NPK_MAGIC = b"NeoplePack_Bill"
NPK_MAGIC_LENGTH = 16
NPK_NAME_LENGTH = 256
NPK_ENTRY_SIZE = 264  # 4 + 4 + 256

# Decrypt key: "puchikon@neople dungeon and fighter DNF" + "DNF" padding
def build_decrypt_key():
    """Build the 256-byte NPK decrypt key."""
    base_key = b"puchikon@neople dungeon and fighter DNF"
    padding = b"DNF"

    key = bytearray(256)

    # Copy base string
    for i, b in enumerate(base_key):
        key[i] = b

    # Fill with "DNF" padding
    pad_idx = 0
    for i in range(len(base_key), 255):
        key[i] = padding[pad_idx]
        pad_idx = (pad_idx + 1) % 3

    key[255] = 0  # Null terminator

    return bytes(key)

DECRYPT_KEY = build_decrypt_key()

def decrypt_filename(encrypted: bytes) -> str:
    """XOR decrypt filename using the key."""
    decrypted = bytearray(NPK_NAME_LENGTH)

    for i in range(NPK_NAME_LENGTH):
        decrypted[i] = encrypted[i] ^ DECRYPT_KEY[i]

    # Find null terminator
    try:
        null_idx = decrypted.index(0)
        return decrypted[:null_idx].decode('utf-8', errors='replace')
    except ValueError:
        return decrypted.rstrip(b'\x00/\\').decode('utf-8', errors='replace')

def parse_npk(filepath: str) -> dict:
    """Parse an NPK archive file."""
    result = {
        'path': filepath,
        'magic': None,
        'file_count': 0,
        'files': [],
        'total_size': 0,
        'errors': []
    }

    try:
        with open(filepath, 'rb') as f:
            # Read magic
            magic = f.read(NPK_MAGIC_LENGTH)
            result['magic'] = magic

            if not magic.startswith(NPK_MAGIC[:14]):  # Check first 14 bytes
                result['errors'].append(f"Invalid magic: {magic}")
                return result

            # Read file count
            file_count_data = f.read(4)
            file_count = struct.unpack('<I', file_count_data)[0]
            result['file_count'] = file_count

            if file_count == 0 or file_count > 100000:
                result['errors'].append(f"Invalid file count: {file_count}")
                return result

            # Read file entries
            for i in range(file_count):
                entry_data = f.read(NPK_ENTRY_SIZE)
                if len(entry_data) < NPK_ENTRY_SIZE:
                    result['errors'].append(f"Incomplete entry at index {i}")
                    break

                offset = struct.unpack('<I', entry_data[0:4])[0]
                size = struct.unpack('<I', entry_data[4:8])[0]
                encrypted_name = entry_data[8:8+NPK_NAME_LENGTH]

                # Decrypt filename
                name = decrypt_filename(encrypted_name)

                result['files'].append({
                    'index': i,
                    'name': name,
                    'offset': offset,
                    'size': size
                })
                result['total_size'] += size

    except Exception as e:
        result['errors'].append(str(e))

    return result

def read_file_data(npk_path: str, entry: dict, file_count: int) -> bytes:
    """Read file data from NPK archive."""
    # Calculate data start position
    header_size = NPK_MAGIC_LENGTH + 4  # magic + file_count
    entries_size = file_count * NPK_ENTRY_SIZE
    data_start = header_size + entries_size
    absolute_offset = data_start + entry['offset']

    with open(npk_path, 'rb') as f:
        f.seek(absolute_offset)
        return f.read(entry['size'])

def test_decrypt_key():
    """Test and display the decrypt key."""
    print("=== Test 0: Decrypt Key ===")
    print(f"Key length: {len(DECRYPT_KEY)} bytes")

    # Show as string (first 64 chars)
    key_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in DECRYPT_KEY[:64])
    print(f"Key (first 64 bytes): {key_str}")

    # Show hex (first 48 bytes)
    hex_str = ' '.join(f'{b:02X}' for b in DECRYPT_KEY[:48])
    print(f"Key hex: {hex_str}")
    print()

def test_npk_file(filepath: str):
    """Test parsing a single NPK file."""
    print(f"=== Test 1: NPK Open ===")
    print(f"File: {filepath}")

    if not os.path.exists(filepath):
        print(f"ERROR: File not found: {filepath}")
        return False

    result = parse_npk(filepath)

    if result['errors']:
        print(f"ERRORS: {result['errors']}")
        return False

    print(f"SUCCESS: NPK opened")
    print(f"  Magic: {result['magic'][:16]}")
    print(f"  File count: {result['file_count']}")
    print(f"  Total data size: {result['total_size']:,} bytes ({result['total_size']/1024/1024:.2f} MB)")
    print()

    # Test 2: File listing
    print("=== Test 2: File Listing (first 20) ===")
    for entry in result['files'][:20]:
        print(f"  [{entry['index']:3d}] {entry['name'][:60]:<60} {entry['size']:>8} bytes")
    if result['file_count'] > 20:
        print(f"  ... ({result['file_count'] - 20} more files)")
    print()

    # Test 3: File reading
    print("=== Test 3: File Reading ===")
    if result['files']:
        entry = result['files'][0]
        print(f"Reading: {entry['name']}")

        try:
            data = read_file_data(filepath, entry, result['file_count'])
            print(f"Read SUCCESS: {len(data)} bytes")

            # Show first 32 bytes as hex
            hex_preview = ' '.join(f'{b:02X}' for b in data[:32])
            print(f"First 32 bytes: {hex_preview}")

            # Check for IMG magic
            if data.startswith(b'Neople Img File') or data.startswith(b'Neople Image File'):
                print("Detected: IMG file format!")
            elif len(data) >= 4:
                # Check for other signatures
                sig = data[:4]
                print(f"File signature: {sig}")

        except Exception as e:
            print(f"Read FAILED: {e}")
    print()

    return True

def test_multiple_npks():
    """Test multiple NPK files."""
    print("=== Multi-NPK Test ===")

    base_path = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2"
    test_files = [
        "sprite_character.NPK",
        "sprite.NPK",
        "sprite_character_common.NPK",
        "sprite_map_npc.NPK",
        "sprite_monster_apostle.NPK"
    ]

    for filename in test_files:
        filepath = os.path.join(base_path, filename)
        if os.path.exists(filepath):
            result = parse_npk(filepath)
            if not result['errors']:
                print(f"[OK]   {filename:<50} {result['file_count']:>4} files, {result['total_size']:>10,} bytes")
            else:
                print(f"[FAIL] {filename:<50} {result['errors'][0]}")
        else:
            print(f"[SKIP] {filename:<50} (not found)")
    print()

def main():
    print("=" * 50)
    print("  NPK Parser Test - Python Implementation")
    print("=" * 50)
    print()

    # Default NPK path
    default_npk = "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2/sprite_character.NPK"
    npk_path = sys.argv[1] if len(sys.argv) > 1 else default_npk

    # Test decrypt key
    test_decrypt_key()

    # Test main NPK
    if test_npk_file(npk_path):
        # Test multiple files
        test_multiple_npks()

    print("=== Test Complete ===")

if __name__ == "__main__":
    main()
