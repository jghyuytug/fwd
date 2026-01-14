#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Extended bruteforce for PVF g_AdditionalKey
Tries millions of key candidates
"""
import struct
import zlib
import sys
import time
from multiprocessing import Pool, cpu_count

# PVF file info (wew332/Script.pvf)
TARGET_CRC32 = 0xcf9e33b3
INDEX_SIZE = 43363336
FILE_COUNT = 517286
REVISION = 63569

def ror32(value, bits):
    """32-bit right rotation"""
    bits = bits % 32
    return ((value >> bits) | (value << (32 - bits))) & 0xFFFFFFFF

def decrypt_index_partial(data, crc_init, checksum, key, check_bytes=1024):
    """Decrypt first check_bytes of index and validate"""
    decrypted = bytearray(data[:check_bytes])

    for i in range(0, min(len(decrypted), check_bytes), 4):
        if i + 4 > len(decrypted):
            break
        dword = struct.unpack('<I', decrypted[i:i+4])[0]
        dword ^= key
        dword = ror32(checksum ^ dword, 6)
        struct.pack_into('<I', decrypted, i, dword)

    return bytes(decrypted)

def test_key(args):
    """Test a single key"""
    key, data, crc_init, checksum, target_crc = args

    try:
        decrypted = decrypt_index_partial(data, crc_init, checksum, key, 4096)

        # Quick validation: check if first few bytes look reasonable
        # File paths should start with printable ASCII
        printable_count = sum(1 for b in decrypted[:100] if 0x20 <= b <= 0x7E or b in (0x00, 0x0A, 0x0D))
        if printable_count < 20:
            return None

        # Check for common path patterns
        has_slash = b'/' in decrypted[:500] or b'\\' in decrypted[:500]
        has_dot = b'.' in decrypted[:500]

        if not (has_slash or has_dot):
            return None

        # Full CRC check on larger sample
        full_decrypted = decrypt_index_partial(data, crc_init, checksum, key, min(65536, len(data)))
        calc_crc = zlib.crc32(full_decrypted) & 0xFFFFFFFF

        # Return promising candidates
        score = printable_count
        if has_slash:
            score += 50
        if has_dot:
            score += 30
        if calc_crc == target_crc:
            score += 10000  # Perfect match!

        if score > 80:
            return (key, score, calc_crc == target_crc, decrypted[:64].hex())
    except:
        pass

    return None

def generate_candidates():
    """Generate key candidates to try"""
    candidates = []

    # Known working key and variants
    base_key = 0x81A79011

    # 1. Bit variations of known key (flip each bit)
    for i in range(32):
        candidates.append(base_key ^ (1 << i))

    # 2. Byte-level variations
    for byte_pos in range(4):
        for byte_val in range(256):
            mask = 0xFF << (byte_pos * 8)
            new_key = (base_key & ~mask) | (byte_val << (byte_pos * 8))
            candidates.append(new_key)

    # 3. Version-based keys
    candidates.extend([
        REVISION,
        REVISION << 16,
        REVISION | (REVISION << 16),
        ~REVISION & 0xFFFFFFFF,
        REVISION * 0x10001,
        REVISION ^ base_key,
        REVISION + base_key,
        (REVISION << 8) | (REVISION >> 8),
    ])

    # 4. CRC-based keys
    candidates.extend([
        TARGET_CRC32,
        ~TARGET_CRC32 & 0xFFFFFFFF,
        TARGET_CRC32 ^ base_key,
        TARGET_CRC32 ^ REVISION,
    ])

    # 5. Common magic numbers
    magic_numbers = [
        0x00000000, 0xFFFFFFFF, 0x12345678, 0x87654321,
        0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE, 0xBADC0DE,
        0x8BADF00D, 0xDEADC0DE, 0xFACEB00C, 0xC0FFEE,
        0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210,
        0x11111111, 0x22222222, 0x33333333, 0x44444444,
        0x55555555, 0x66666666, 0x77777777, 0x88888888,
        0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
        0xDDDDDDDD, 0xEEEEEEEE,
    ]
    candidates.extend(magic_numbers)

    # 6. Sequential scan around known key
    for offset in range(-100000, 100001):
        candidates.append((base_key + offset) & 0xFFFFFFFF)

    # 7. Powers of 2 and related
    for i in range(32):
        candidates.append(1 << i)
        candidates.append((1 << i) - 1)
        candidates.append(~(1 << i) & 0xFFFFFFFF)

    # 8. Repeating patterns
    for b in range(256):
        candidates.append(b * 0x01010101)

    # 9. Two-byte patterns
    for w in range(0, 0x10000, 0x100):
        candidates.append(w | (w << 16))

    # 10. XOR combinations
    for magic in magic_numbers:
        candidates.append(magic ^ base_key)
        candidates.append(magic ^ REVISION)
        candidates.append(magic ^ TARGET_CRC32)

    # Remove duplicates and sort
    candidates = list(set(candidates))
    print(f"Generated {len(candidates)} unique candidates")

    return candidates

def main():
    pvf_path = sys.argv[1] if len(sys.argv) > 1 else "wew332/Script.pvf"

    print("=" * 70)
    print("Extended PVF Key Bruteforce")
    print("=" * 70)
    print(f"File: {pvf_path}")
    print(f"Target CRC32: 0x{TARGET_CRC32:08x}")
    print()

    # Read PVF header and index
    with open(pvf_path, 'rb') as f:
        f.seek(36)  # Skip GUID
        revision = struct.unpack('<I', f.read(4))[0]
        index_size = struct.unpack('<I', f.read(4))[0]
        index_crc = struct.unpack('<I', f.read(4))[0]
        file_count = struct.unpack('<I', f.read(4))[0]

        print(f"Revision: {revision}")
        print(f"Index size: {index_size:,} bytes")
        print(f"Index CRC32: 0x{index_crc:08x}")
        print(f"File count: {file_count:,}")
        print()

        # Read encrypted index (first 64KB for speed)
        index_data = f.read(min(65536, index_size))

    candidates = generate_candidates()

    print(f"\nTesting {len(candidates):,} candidates...")
    print("-" * 70)

    start_time = time.time()
    found = []

    # Prepare arguments
    args_list = [(key, index_data, index_crc, index_crc, TARGET_CRC32) for key in candidates]

    # Use multiprocessing
    num_workers = max(1, cpu_count() - 1)
    print(f"Using {num_workers} worker processes")

    with Pool(num_workers) as pool:
        results = pool.map(test_key, args_list, chunksize=1000)

    # Filter results
    found = [r for r in results if r is not None]
    found.sort(key=lambda x: -x[1])  # Sort by score descending

    elapsed = time.time() - start_time
    print(f"\nCompleted in {elapsed:.1f} seconds")
    print(f"Speed: {len(candidates)/elapsed:.0f} keys/sec")

    if found:
        print("\n" + "=" * 70)
        print("RESULTS")
        print("=" * 70)

        for key, score, is_match, preview in found[:20]:
            status = "*** MATCH! ***" if is_match else ""
            print(f"\nKey: 0x{key:08X}  Score: {score}  {status}")
            print(f"  Preview: {preview[:64]}...")

        # Check for perfect match
        matches = [f for f in found if f[2]]
        if matches:
            print("\n" + "!" * 70)
            print(f"FOUND MATCHING KEY: 0x{matches[0][0]:08X}")
            print("!" * 70)
    else:
        print("\nNo promising candidates found.")
        print("The key might be completely different from expected patterns.")

if __name__ == '__main__':
    main()
