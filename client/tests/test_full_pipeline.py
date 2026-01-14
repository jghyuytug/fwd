#!/usr/bin/env python3
"""
Full pipeline test: NPK -> IMG -> Decode -> PNG output

Validates the complete resource loading chain:
1. NPK parsing with absolute offsets
2. IMG parsing with interleaved format
3. Frame decoding (zlib + ARGB1555/4444/8888)
4. PNG output for visual verification

Usage:
    python test_full_pipeline.py
"""

import os
import sys
import struct
import zlib
from pathlib import Path

# Output directory
OUTPUT_DIR = Path(__file__).parent / "output"
OUTPUT_DIR.mkdir(exist_ok=True)

# NPK decrypt key (256 bytes)
NPK_KEY_BASE = "puchikon@neople dungeon and fighter DNF"
NPK_KEY = bytearray(256)
for i, c in enumerate(NPK_KEY_BASE):
    NPK_KEY[i] = ord(c)
padding = "DNF"
for i in range(len(NPK_KEY_BASE), 255):
    NPK_KEY[i] = ord(padding[(i - len(NPK_KEY_BASE)) % 3])
NPK_KEY[255] = 0


def decrypt_npk_name(encrypted: bytes) -> str:
    """XOR decrypt NPK filename."""
    decrypted = bytearray(256)
    for i in range(256):
        decrypted[i] = encrypted[i] ^ NPK_KEY[i]
    # Find null terminator
    try:
        end = decrypted.index(0)
    except ValueError:
        end = 256
    return decrypted[:end].decode('utf-8', errors='replace').rstrip('/')


def parse_npk(npk_path: str) -> dict:
    """Parse NPK archive."""
    with open(npk_path, 'rb') as f:
        # Read magic (16 bytes)
        magic = f.read(16)
        if not magic.startswith(b'NeoplePack'):
            raise ValueError(f"Invalid NPK magic: {magic[:16]}")

        # Read file count
        file_count = struct.unpack('<I', f.read(4))[0]
        print(f"NPK: {os.path.basename(npk_path)}")
        print(f"  Magic: {magic.rstrip(b'\\x00')}")
        print(f"  File count: {file_count}")

        # Read entries (264 bytes each: 4 offset + 4 size + 256 name)
        files = []
        for i in range(file_count):
            offset = struct.unpack('<I', f.read(4))[0]
            size = struct.unpack('<I', f.read(4))[0]
            encrypted_name = f.read(256)
            name = decrypt_npk_name(encrypted_name)
            files.append({
                'index': i,
                'offset': offset,  # ABSOLUTE offset
                'size': size,
                'name': name
            })

        return {'magic': magic, 'file_count': file_count, 'files': files}


def read_img_data(npk_path: str, offset: int, size: int) -> bytes:
    """Read IMG data from NPK at absolute offset."""
    with open(npk_path, 'rb') as f:
        f.seek(offset)
        return f.read(size)


def parse_img_interleaved(data: bytes, name: str) -> dict:
    """Parse IMG with interleaved entry/data format."""
    if len(data) < 32:
        return None

    # Check magic
    magic = data[:18].rstrip(b'\x00')
    if not magic.startswith(b'Neople'):
        print(f"  [!] Invalid IMG magic: {magic}")
        return None

    # Parse header
    image_count = struct.unpack('<I', data[28:32])[0]
    version = struct.unpack('<I', data[24:28])[0]

    print(f"\nIMG: {name}")
    print(f"  Magic: {magic}")
    print(f"  Version: {version}")
    print(f"  Frame count: {image_count}")

    if image_count > 4096:
        print(f"  [!] Frame count too large!")
        return None

    # Parse frames (interleaved: entry 36B + data)
    frames = []
    offset = 32  # Start after header

    for i in range(image_count):
        if offset + 36 > len(data):
            print(f"  [!] Frame {i}: truncated at offset 0x{offset:X}")
            break

        # Read entry (36 bytes)
        fmt = struct.unpack('<I', data[offset:offset+4])[0]
        compress = struct.unpack('<I', data[offset+4:offset+8])[0]
        width = struct.unpack('<i', data[offset+8:offset+12])[0]
        height = struct.unpack('<i', data[offset+12:offset+16])[0]
        size = struct.unpack('<I', data[offset+16:offset+20])[0]
        key_x = struct.unpack('<i', data[offset+20:offset+24])[0]
        key_y = struct.unpack('<i', data[offset+24:offset+28])[0]
        max_w = struct.unpack('<i', data[offset+28:offset+32])[0]
        max_h = struct.unpack('<i', data[offset+32:offset+36])[0]

        # Data offset is right after entry
        data_offset = offset + 36

        frame = {
            'index': i,
            'entry_offset': offset,
            'data_offset': data_offset,
            'format': fmt,
            'compress': compress,
            'width': width,
            'height': height,
            'size': size,
            'key_x': key_x,
            'key_y': key_y,
            'max_w': max_w,
            'max_h': max_h,
            'is_link': (fmt == 0 and compress == 0 and width == 0 and height == 0)
        }
        frames.append(frame)

        # Move to next entry (skip past data)
        if frame['is_link']:
            offset += 36  # Link frames have no data
        else:
            offset += 36 + size

    return {
        'magic': magic,
        'version': version,
        'frame_count': image_count,
        'frames': frames,
        'raw_data': data
    }


def decode_frame(img: dict, frame_index: int) -> bytes:
    """Decode a single frame to RGBA pixels."""
    if frame_index >= len(img['frames']):
        return None

    frame = img['frames'][frame_index]

    # Follow link chain
    max_links = 10
    while frame['is_link'] and max_links > 0:
        link_idx = frame['size']  # size field contains link target
        if link_idx >= len(img['frames']):
            return None
        frame = img['frames'][link_idx]
        max_links -= 1

    if frame['width'] <= 0 or frame['height'] <= 0:
        return None

    data = img['raw_data']
    compressed = data[frame['data_offset']:frame['data_offset'] + frame['size']]

    # Decompress if needed
    if frame['compress'] == 6:  # zlib
        try:
            pixels = zlib.decompress(compressed)
        except Exception as e:
            print(f"  [!] Frame {frame_index}: decompress failed: {e}")
            return None
    elif frame['compress'] == 5:  # raw
        pixels = compressed
    else:
        return None

    # Convert to RGBA
    width = frame['width']
    height = frame['height']

    if frame['format'] == 14:  # ARGB1555
        expected = width * height * 2
        if len(pixels) < expected:
            print(f"  [!] Frame {frame_index}: pixel data too short ({len(pixels)} < {expected})")
            return None

        rgba = bytearray(width * height * 4)
        for i in range(width * height):
            p = struct.unpack('<H', pixels[i*2:i*2+2])[0]
            a = 255 if (p & 0x8000) else 0
            r = ((p >> 10) & 0x1F) * 255 // 31
            g = ((p >> 5) & 0x1F) * 255 // 31
            b = (p & 0x1F) * 255 // 31
            rgba[i*4:i*4+4] = bytes([r, g, b, a])
        return bytes(rgba)

    elif frame['format'] == 15:  # ARGB4444
        expected = width * height * 2
        if len(pixels) < expected:
            return None

        rgba = bytearray(width * height * 4)
        for i in range(width * height):
            p = struct.unpack('<H', pixels[i*2:i*2+2])[0]
            a = ((p >> 12) & 0x0F) * 255 // 15
            r = ((p >> 8) & 0x0F) * 255 // 15
            g = ((p >> 4) & 0x0F) * 255 // 15
            b = (p & 0x0F) * 255 // 15
            rgba[i*4:i*4+4] = bytes([r, g, b, a])
        return bytes(rgba)

    elif frame['format'] == 16:  # ARGB8888
        expected = width * height * 4
        if len(pixels) < expected:
            return None

        rgba = bytearray(width * height * 4)
        for i in range(width * height):
            a = pixels[i*4]
            r = pixels[i*4+1]
            g = pixels[i*4+2]
            b = pixels[i*4+3]
            rgba[i*4:i*4+4] = bytes([r, g, b, a])
        return bytes(rgba)

    return None


def save_as_ppm(rgba: bytes, width: int, height: int, path: str):
    """Save RGBA data as PPM (simple format, no external deps)."""
    with open(path, 'wb') as f:
        f.write(f"P6\n{width} {height}\n255\n".encode())
        for i in range(width * height):
            r, g, b = rgba[i*4], rgba[i*4+1], rgba[i*4+2]
            f.write(bytes([r, g, b]))


def main():
    # Taiwan DNF path
    base = Path("C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2")

    if not base.exists():
        print(f"Error: {base} not found")
        return 1

    # Find NPK files to test
    test_npks = [
        "sprite_character.NPK",
        "sprite_monster.NPK",
        "sprite_interface.NPK"
    ]

    total_frames_decoded = 0
    total_frames_failed = 0

    for npk_name in test_npks:
        npk_path = base / npk_name
        if not npk_path.exists():
            print(f"Skipping {npk_name} (not found)")
            continue

        print(f"\n{'='*60}")
        print(f"Testing: {npk_name}")
        print(f"{'='*60}")

        try:
            npk = parse_npk(str(npk_path))
        except Exception as e:
            print(f"Failed to parse NPK: {e}")
            continue

        # Test first 3 IMG files
        imgs_tested = 0
        for entry in npk['files']:
            if not entry['name'].endswith('.img'):
                continue
            if imgs_tested >= 3:
                break
            imgs_tested += 1

            print(f"\n  Testing: {entry['name']}")
            print(f"    Offset: 0x{entry['offset']:X}, Size: {entry['size']}")

            # Read IMG data
            img_data = read_img_data(str(npk_path), entry['offset'], entry['size'])

            # Parse IMG
            img = parse_img_interleaved(img_data, entry['name'])
            if img is None:
                print(f"    [!] Failed to parse IMG")
                continue

            # Decode first 5 frames
            frames_to_decode = min(5, len(img['frames']))
            for i in range(frames_to_decode):
                frame = img['frames'][i]

                if frame['is_link']:
                    print(f"    Frame {i}: LINK -> {frame['size']}")
                    continue

                print(f"    Frame {i}: {frame['width']}x{frame['height']}, "
                      f"fmt={frame['format']}, cmp={frame['compress']}, "
                      f"size={frame['size']}, offset=({frame['key_x']},{frame['key_y']})")

                rgba = decode_frame(img, i)
                if rgba:
                    total_frames_decoded += 1

                    # Save first frame as PPM
                    if i == 0:
                        safe_name = entry['name'].replace('/', '_').replace('\\', '_')
                        out_path = OUTPUT_DIR / f"{npk_name}_{safe_name}_frame0.ppm"
                        save_as_ppm(rgba, frame['width'], frame['height'], str(out_path))
                        print(f"      -> Saved: {out_path.name}")
                else:
                    total_frames_failed += 1
                    print(f"      -> Decode FAILED")

    print(f"\n{'='*60}")
    print(f"Summary")
    print(f"{'='*60}")
    print(f"Frames decoded: {total_frames_decoded}")
    print(f"Frames failed: {total_frames_failed}")

    if total_frames_decoded > 0:
        print(f"\nSUCCESS! Check output in: {OUTPUT_DIR}")
        return 0
    else:
        print(f"\nFAILED - no frames decoded")
        return 1


if __name__ == "__main__":
    sys.exit(main())
