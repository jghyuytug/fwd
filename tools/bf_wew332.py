#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Single-threaded bruteforce for wew332/Script.pvf"""
import struct
import zlib
import time

TARGET_CRC32 = 0xcf9e33b3
HEADER_OFFSET = 40

def ror32(value, bits):
    bits = bits % 32
    return ((value >> bits) | (value << (32 - bits))) & 0xFFFFFFFF

def decrypt_partial(data, key, checksum, size=8192):
    decrypted = bytearray(data[:size])
    for i in range(0, len(decrypted), 4):
        if i + 4 > len(decrypted):
            break
        dword = struct.unpack('<I', decrypted[i:i+4])[0]
        dword ^= key
        dword = ror32(checksum ^ dword, 6)
        struct.pack_into('<I', decrypted, i, dword)
    return bytes(decrypted)

def main():
    print("=" * 60)
    print("wew332/Script.pvf Bruteforce")
    print("=" * 60)

    with open('wew332/Script.pvf', 'rb') as f:
        f.seek(HEADER_OFFSET)
        revision = struct.unpack('<I', f.read(4))[0]
        index_size = struct.unpack('<I', f.read(4))[0]
        index_crc = struct.unpack('<I', f.read(4))[0]
        file_count = struct.unpack('<I', f.read(4))[0]

        print(f'Revision: {revision}')
        print(f'Index size: {index_size:,}')
        print(f'Index CRC32: 0x{index_crc:08x}')
        print(f'File count: {file_count:,}')

        index_data = f.read(min(index_size, 100000))

    print(f'Read {len(index_data):,} bytes')
    print()

    # Generate candidates
    candidates = set()
    base = 0x81A79011

    # Around known key
    for offset in range(-100000, 100001):
        candidates.add((base + offset) & 0xFFFFFFFF)

    # Repeating patterns
    for b in range(256):
        candidates.add(b * 0x01010101)

    # Magic numbers and variations
    magic = [0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE, 0x12345678, 0x87654321,
             revision, revision << 16, index_crc, file_count, ~index_crc & 0xFFFFFFFF]
    for m in magic:
        for offset in range(-500, 501):
            candidates.add((m + offset) & 0xFFFFFFFF)

    candidates = list(candidates)
    print(f'Testing {len(candidates):,} candidates...')

    start = time.time()
    results = []
    checked = 0

    for key in candidates:
        checked += 1
        if checked % 10000 == 0:
            print(f'  {checked:,} / {len(candidates):,}...')

        decrypted = decrypt_partial(index_data, key, index_crc, 4096)

        # Quick check
        has_slash = b'/' in decrypted[:500]
        has_dot = b'.' in decrypted[:500]
        printable = sum(1 for b in decrypted[:200] if 0x20 <= b <= 0x7E or b == 0)

        score = printable
        if has_slash:
            score += 100
        if has_dot:
            score += 30

        if score > 130:
            # More thorough check
            full = decrypt_partial(index_data, key, index_crc, len(index_data))
            calc_crc = zlib.crc32(full) & 0xFFFFFFFF

            if calc_crc == TARGET_CRC32:
                print(f'\n*** FOUND KEY: 0x{key:08X} ***')
                print(f'Preview: {decrypted[:100]}')
                results.append((key, 'MATCH', decrypted[:100]))
            elif score > 150:
                results.append((key, score, decrypted[:64]))

    elapsed = time.time() - start
    print(f'\nDone in {elapsed:.1f}s ({len(candidates)/elapsed:.0f} keys/sec)')

    if results:
        results.sort(key=lambda x: -10000 if x[1] == 'MATCH' else -x[1])
        print('\nTop results:')
        for key, score, preview in results[:10]:
            status = '*** MATCH ***' if score == 'MATCH' else f'score={score}'
            print(f'  0x{key:08X}: {status}')
            if isinstance(preview, bytes):
                print(f'    {preview[:40]}')
    else:
        print('\nNo good candidates found.')

if __name__ == '__main__':
    main()
