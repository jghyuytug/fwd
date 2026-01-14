#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
暴力搜索g_AdditionalKey值

尝试不同的g_AdditionalKey值，找到能让CRC32匹配的值
"""

import struct
import sys


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def decrypt_index_with_key(encrypted_data: bytes,
                            crc_init: int,
                            checksum: int,
                            key: int,
                            g_additional_key: int) -> tuple:
    """
    使用指定的g_AdditionalKey解密索引

    Returns:
        (是否CRC匹配, 实际CRC32值, 解密数据)
    """
    size = len(encrypted_data)

    if (size & 3) != 0:
        return False, 0, None

    # 生成CRC32表
    def make_crc_table():
        table = []
        for i in range(256):
            c = i
            for _ in range(8):
                if c & 1:
                    c = 0xEDB88320 ^ (c >> 1)
                else:
                    c = c >> 1
            table.append(c)
        return table

    crc_table = make_crc_table()
    crc = ~crc_init & 0xFFFFFFFF

    decrypted = bytearray(encrypted_data)

    for i in range(0, size, 4):
        dword = struct.unpack('<I', decrypted[i:i+4])[0]

        # 步骤1: XOR g_AdditionalKey
        if g_additional_key:
            dword ^= g_additional_key

        # 步骤2: ROR(key ^ data, 6)
        dword = ror32(key ^ dword, 6)

        struct.pack_into('<I', decrypted, i, dword)

        # 步骤3: 更新CRC32
        for j in range(4):
            byte = decrypted[i + j]
            crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

    final_crc = ~crc & 0xFFFFFFFF

    return final_crc == checksum, final_crc, bytes(decrypted)


def main(pvf_path: str):
    print("=" * 80)
    print("暴力搜索g_AdditionalKey")
    print("=" * 80)
    print()

    # 读取Script.pvf头部和索引
    with open(pvf_path, 'rb') as f:
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"文件: {pvf_path}")
        print(f"文件数量: {index_count:,}")
        print(f"索引大小: {index_header_size:,} 字节")
        print(f"索引CRC32: 0x{index_crc32:08x}")
        print()

        encrypted_index = f.read(index_header_size)

    # 尝试的g_AdditionalKey候选值
    candidates = [
        ("0 (禁用)", 0),
        ("反编译值", 0x81A12D11),
        ("index_crc32", index_crc32),
        ("~index_crc32", ~index_crc32 & 0xFFFFFFFF),
        ("index_count", index_count),
        ("~index_count", ~index_count & 0xFFFFFFFF),
        ("revision", revision),
        ("~revision", ~revision & 0xFFFFFFFF),
    ]

    print(f"尝试 {len(candidates)} 个候选值...")
    print()

    for name, g_add in candidates:
        matched, actual_crc, decrypted = decrypt_index_with_key(
            encrypted_index,
            index_count,
            index_crc32,
            index_crc32,
            g_add
        )

        status = "[+] 匹配!" if matched else "[.]"
        print(f"{status} {name:20s} = 0x{g_add:08x} → CRC32=0x{actual_crc:08x}")

        if matched:
            print()
            print(f"[+] 找到匹配的g_AdditionalKey: 0x{g_add:08x} ({name})")
            print()

            # 尝试解析第一个索引条目
            try:
                offset = 0
                hash_value = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4
                name_length = struct.unpack('<I', decrypted[offset:offset+4])[0]
                offset += 4

                if name_length < 1000:
                    filename = decrypted[offset:offset+name_length].decode('utf-8', errors='ignore')
                    print(f"第一个文件: {filename}")
                    print(f"哈希: 0x{hash_value:08x}")

                    # 保存解密数据
                    with open('decrypted_index.bin', 'wb') as out:
                        out.write(decrypted)
                    print()
                    print("[+] 解密数据已保存到 decrypted_index.bin")
                    return True
            except Exception as e:
                print(f"[!] 解析失败: {e}")

    print()
    print("[!] 没有找到匹配的g_AdditionalKey值")
    print()
    print("可能的原因:")
    print("1. g_AdditionalKey是其他未测试的值")
    print("2. 加密算法有其他变体")
    print("3. 文件版本不匹配")

    return False


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python bruteforce_g_additional_key.py <Script.pvf>")
        sys.exit(1)

    success = main(sys.argv[1])
    sys.exit(0 if success else 1)
