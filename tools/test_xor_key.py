#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试XOR_KEY参数 - 方案A

测试g_AdditionalKey和XOR_KEY的组合，找到正确的参数对
"""

import struct
import sys
from typing import List, Tuple


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def decrypt_index_with_keys(encrypted_data: bytes,
                              crc_init: int,
                              checksum: int,
                              key: int,
                              g_additional_key: int,
                              xor_key: int) -> Tuple[bool, int, bytes]:
    """
    使用指定的g_AdditionalKey和XOR_KEY解密索引

    Args:
        encrypted_data: 加密的索引数据
        crc_init: CRC32初始值
        checksum: 期望的CRC32
        key: 基础密钥 (通常是index_crc32)
        g_additional_key: 附加密钥
        xor_key: XOR密钥

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

    # 应用XOR_KEY到密钥 (基于 df_game_r_part24.c:26682-26683)
    if xor_key:
        key ^= xor_key

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


def check_filename(data: bytes) -> Tuple[bool, str]:
    """
    检查解密数据的第一个文件名是否有效

    Returns:
        (是否有效, 文件名或错误信息)
    """
    try:
        offset = 0

        if len(data) < 8:
            return False, "数据太短"

        # 读取哈希值
        hash_value = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # 读取文件名长度
        name_length = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # 检查文件名长度
        if name_length > 500 or name_length == 0:
            return False, f"长度异常: {name_length}"

        if len(data) < offset + name_length:
            return False, f"数据不足"

        # 读取文件名
        filename = data[offset:offset+name_length].decode('utf-8', errors='strict')

        # 检查文件名字符
        if not all(c.isprintable() or c in ['/', '\\'] for c in filename):
            return False, f"包含非法字符"

        # 检查是否像文件路径
        if '/' not in filename and '\\' not in filename:
            if '.' not in filename:
                return False, f"不像文件路径"

        return True, filename

    except UnicodeDecodeError:
        return False, "非UTF-8"
    except Exception as e:
        return False, str(e)


def generate_key_pairs(index_crc32: int, index_count: int,
                        index_header_size: int, revision: int) -> List[Tuple[str, int, int]]:
    """
    生成(名称, g_AdditionalKey, XOR_KEY)候选对

    Returns:
        List of (描述, g_AdditionalKey, XOR_KEY)
    """
    pairs = []

    # g_AdditionalKey候选值 (从智能搜索中选择最有希望的)
    g_add_candidates = [
        ("0", 0),
        ("反编译", 0x81A12D11),
        ("crc32", index_crc32),
        ("~crc32", ~index_crc32 & 0xFFFFFFFF),
    ]

    # XOR_KEY候选值
    xor_key_candidates = [
        ("0", 0),
        ("反编译", 0x81A12D11),
        ("crc32", index_crc32),
        ("~crc32", ~index_crc32 & 0xFFFFFFFF),
        ("count", index_count),
        ("revision", revision),
        ("header_size", index_header_size),
        ("DEADBEEF", 0xDEADBEEF),
        ("CAFEBABE", 0xCAFEBABE),
        ("12345678", 0x12345678),
    ]

    # 生成所有组合
    for g_name, g_add in g_add_candidates:
        for x_name, xor_key in xor_key_candidates:
            pair_name = f"g_add={g_name}, xor={x_name}"
            pairs.append((pair_name, g_add, xor_key))

    return pairs


def main(pvf_path: str):
    print("=" * 80)
    print("测试XOR_KEY参数 - 方案A")
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
        print(f"版本号: {revision}")
        print()

        encrypted_index = f.read(index_header_size)

    # 生成参数对
    key_pairs = generate_key_pairs(index_crc32, index_count, index_header_size, revision)

    print(f"[*] 生成 {len(key_pairs)} 个参数组合")
    print()
    print("[*] 开始测试...")
    print()

    # 测试所有组合
    matches = []
    partial_matches = []

    for i, (pair_name, g_add, xor_key) in enumerate(key_pairs, 1):
        matched, actual_crc, decrypted = decrypt_index_with_keys(
            encrypted_index,
            index_count,
            index_crc32,
            index_crc32,
            g_add,
            xor_key
        )

        # 检查文件名
        filename_valid = False
        filename_info = ""
        if decrypted:
            filename_valid, filename_info = check_filename(decrypted)

        # CRC32完全匹配
        if matched:
            print(f"[+] 找到CRC32匹配! #{i}")
            print(f"    参数: {pair_name}")
            print(f"    g_AdditionalKey: 0x{g_add:08x}")
            print(f"    XOR_KEY: 0x{xor_key:08x}")
            print(f"    文件名: {filename_info if filename_valid else '[.]' + filename_info}")
            print()

            matches.append({
                'name': pair_name,
                'g_add': g_add,
                'xor_key': xor_key,
                'crc': actual_crc,
                'filename_valid': filename_valid,
                'filename_info': filename_info,
                'decrypted': decrypted
            })

        # 文件名有效（即使CRC不匹配）
        elif filename_valid:
            print(f"[*] 文件名有效! #{i}")
            print(f"    参数: {pair_name}")
            print(f"    g_AdditionalKey: 0x{g_add:08x}")
            print(f"    XOR_KEY: 0x{xor_key:08x}")
            print(f"    文件名: {filename_info}")
            print(f"    CRC32: 0x{actual_crc:08x} (期望 0x{index_crc32:08x})")
            print()

            partial_matches.append({
                'name': pair_name,
                'g_add': g_add,
                'xor_key': xor_key,
                'crc': actual_crc,
                'filename_info': filename_info,
                'decrypted': decrypted
            })

        # 进度指示（每10个显示一次）
        if i % 10 == 0:
            print(f"[.] 已测试 {i}/{len(key_pairs)} 个组合...")

    print()
    print("=" * 80)
    print("测试完成")
    print("=" * 80)
    print()

    # 报告结果
    if matches:
        print(f"[+] 找到 {len(matches)} 个完全匹配的参数组合!")
        print()

        best = matches[0]
        print("最佳匹配:")
        print(f"  参数: {best['name']}")
        print(f"  g_AdditionalKey: 0x{best['g_add']:08x}")
        print(f"  XOR_KEY: 0x{best['xor_key']:08x}")
        print(f"  CRC32: 0x{best['crc']:08x} [+] 完全匹配")
        print(f"  文件名: {best['filename_info']}")

        # 保存解密数据
        with open('decrypted_index_xor.bin', 'wb') as out:
            out.write(best['decrypted'])
        print()
        print("[+] 解密数据已保存到 decrypted_index_xor.bin")

        return True

    elif partial_matches:
        print(f"[*] 找到 {len(partial_matches)} 个部分匹配（文件名有效，CRC不匹配）")
        print()

        best = partial_matches[0]
        print("最佳部分匹配:")
        print(f"  参数: {best['name']}")
        print(f"  g_AdditionalKey: 0x{best['g_add']:08x}")
        print(f"  XOR_KEY: 0x{best['xor_key']:08x}")
        print(f"  CRC32: 0x{best['crc']:08x} [.] 期望 0x{index_crc32:08x}")
        print(f"  文件名: {best['filename_info']}")

        # 保存解密数据
        with open('decrypted_index_xor_partial.bin', 'wb') as out:
            out.write(best['decrypted'])
        print()
        print("[*] 解密数据已保存到 decrypted_index_xor_partial.bin")

        return False

    else:
        print("[!] 没有找到匹配或部分匹配")
        print()
        print("可能的原因:")
        print("1. XOR_KEY值不在候选列表中")
        print("2. 加密算法有其他变体（如ROR偏移量不是6）")
        print("3. 需要额外的加密参数")
        print("4. 版本差异导致算法完全不同")
        print()
        print("建议:")
        print("- 执行方案B: 尝试不同的ROR偏移量")
        print("- 执行方案C: 调试实际二进制获取真实参数")

        return False


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python test_xor_key.py <Script.pvf>")
        sys.exit(1)

    success = main(sys.argv[1])
    sys.exit(0 if success else 1)
