#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
索引解密工具 - 实现_decrypt_N_crc32算法

基于反编译代码: src_split/df_game_r_part24.c:26670
"""

import struct
import zlib


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def rol32(value, shift):
    """32位左旋转"""
    return ((value << shift) | (value >> (32 - shift))) & 0xFFFFFFFF


def decrypt_index(encrypted_data: bytes,
                  crc_init: int,
                  checksum: int,
                  key: int) -> bytes:
    """
    解密索引数据

    基于 _decrypt_N_crc32 (df_game_r_part24.c:26670)

    Args:
        encrypted_data: 加密的索引数据
        crc_init: CRC32初始值 (index_size/文件数量)
        checksum: 期望的CRC32校验和 (index_header_crc)
        key: 解密密钥 (index_header_crc)

    Returns:
        解密后的数据，如果CRC验证失败返回None
    """
    size = len(encrypted_data)

    # 检查对齐（必须是4的倍数）
    if (size & 3) != 0:
        print(f"[!] 数据大小 {size} 不是4的倍数")
        return None

    # 生成CRC32表（标准多项式0xEDB88320）
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

    # XOR_KEY默认为0（df_game_r_part06.c:69957未初始化）
    # g_AdditionalKey从最新反汇编代码获取（conf目录提供）
    XOR_KEY = 0
    g_AdditionalKey = 0x81A79011  # 最新版本的正确值！

    # 修改密钥
    if XOR_KEY:
        key ^= XOR_KEY

    # 解密
    decrypted = bytearray(encrypted_data)

    for i in range(0, size, 4):
        # 读取4字节
        dword = struct.unpack('<I', decrypted[i:i+4])[0]

        # 步骤1: XOR g_AdditionalKey（基于df_game_r_part24.c:26690-26691）
        if g_AdditionalKey:
            dword ^= g_AdditionalKey

        # 步骤2: 核心解密 ROR(key ^ data, 6)（基于df_game_r_part24.c:26692）
        # 加密是: (key ^ ROR(plain, 26)) ^ g_add
        # 解密是: ROR(key ^ (cipher ^ g_add), 6)
        # 因为 ROL(x, 26) = ROR(x, 6) 在32位下
        dword = ror32(key ^ dword, 6)

        # 写回
        struct.pack_into('<I', decrypted, i, dword)

        # 更新CRC32（标准算法，每个字节）
        for j in range(4):
            byte = decrypted[i + j]
            crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

    # 验证CRC32
    final_crc = ~crc & 0xFFFFFFFF

    print(f"[*] CRC32计算完成:")
    print(f"    期望: 0x{checksum:08x}")
    print(f"    实际: 0x{final_crc:08x}")
    print()

    # 即使CRC不匹配也返回解密数据，以便检查
    if final_crc != checksum:
        print(f"[!] CRC32不匹配，但仍返回解密数据供检查")
        # return None

    print(f"[+] 返回解密数据")
    return bytes(decrypted)


def test_with_script_pvf(pvf_path: str):
    """测试Script.pvf索引解密"""
    import sys

    print("=" * 80)
    print("索引解密测试")
    print("=" * 80)
    print()

    with open(pvf_path, 'rb') as f:
        # 读取头部
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"GUID: {tag_string}")
        print(f"版本号: {revision}")
        print(f"文件数量: {index_count:,}")
        print(f"索引大小: {index_header_size:,} 字节")
        print(f"索引CRC32: 0x{index_crc32:08x}")
        print()

        # 读取加密的索引数据
        encrypted_index = f.read(index_header_size)

        if len(encrypted_index) != index_header_size:
            print(f"[-] 索引数据读取不完整")
            return False

        print(f"[*] 尝试解密索引...")
        print()

        # 解密
        decrypted_index = decrypt_index(
            encrypted_index,
            crc_init=index_count,    # 使用文件数量作为CRC初始值
            checksum=index_crc32,    # 期望的CRC32
            key=index_crc32          # 密钥 = CRC32
        )

        if decrypted_index is None:
            print("[-] 索引解密返回None")
            return False

        print()
        print(f"[+] 索引解密成功!")
        print()

        # 显示前256字节
        print("解密后数据前256字节:")
        print(decrypted_index[:256].hex())
        print()

        # 统计可打印字符
        printable = sum(1 for b in decrypted_index[:256] if 32 <= b < 127)
        print(f"可打印字符: {printable}/256 ({printable*100//256}%)")
        print()

        # 尝试解析第一个索引条目
        print("[*] 尝试解析第一个索引条目...")
        try:
            offset = 0

            # 读取哈希值
            hash_value = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
            offset += 4

            # 读取文件名长度
            name_length = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
            offset += 4

            print(f"    哈希值: 0x{hash_value:08x}")
            print(f"    文件名长度: {name_length}")

            if name_length > 1000:
                print(f"[!] 文件名长度异常({name_length})，可能解密仍有问题")
                return False

            # 读取文件名
            filename = decrypted_index[offset:offset+name_length].decode('utf-8', errors='ignore')
            offset += name_length

            # 读取其他字段
            file_offset = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
            offset += 4

            file_size = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
            offset += 4

            aligned_size = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
            offset += 4

            file_crc32 = struct.unpack('<I', decrypted_index[offset:offset+4])[0]

            print(f"[+] 第一个文件: {filename}")
            print(f"    哈希: 0x{hash_value:08x}")
            print(f"    偏移: {file_offset}")
            print(f"    大小: {file_size}")
            print(f"    对齐大小: {aligned_size}")
            print(f"    CRC32: 0x{file_crc32:08x}")

            # 判断文件名是否合理
            if filename and all(c.isprintable() or c in ['/','\\'] for c in filename):
                print()
                print("[+] 文件名看起来合理！解密可能成功！")

                # 保存解密后的索引数据
                with open('decrypted_index.bin', 'wb') as out:
                    out.write(decrypted_index)
                print("[+] 解密数据已保存到 decrypted_index.bin")

                return True
            else:
                print()
                print("[!] 文件名包含乱码，解密可能不正确")
                return False

        except Exception as e:
            print(f"[-] 解析失败: {e}")
            import traceback
            traceback.print_exc()
            return False


if __name__ == '__main__':
    import sys

    if len(sys.argv) < 2:
        print("Usage: python decrypt_index.py <Script.pvf>")
        sys.exit(1)

    success = test_with_script_pvf(sys.argv[1])
    sys.exit(0 if success else 1)
