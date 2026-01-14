#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF文件生成器 - 用于测试pvf_parser.py

创建一个简单的测试PVF文件，包含几个文本文件，
用于验证解析器的正确性。

Usage:
    python create_test_pvf.py
"""

import struct
import zlib
from pathlib import Path


class PVFBuilder:
    """PVF文件构建器"""

    GUID = "fa08bf71-4395-6a4b-a3e3-2617c9fee116"

    def __init__(self, output_path: str = "test_Script.pvf"):
        self.output_path = Path(output_path)
        self.files = []

    def add_file(self, filename: str, content: bytes):
        """添加文件到PVF"""
        self.files.append({
            'name': filename,
            'content': content
        })

    def _normalize_path(self, path: str) -> str:
        """规范化文件路径（与生产服务器一致）"""
        result = []
        for c in path:
            if c == '\\' or c == '/':
                result.append('/')
            elif 'A' <= c <= 'Z':
                result.append(chr(ord(c) + 32))  # 转小写
            else:
                result.append(c)
        return ''.join(result)

    def _djb2_hash(self, string: str) -> int:
        """DJB2哈希算法（生产服务器实际使用）"""
        hash_value = 5381

        for char in string:
            hash_value = (hash_value * 33 + ord(char)) & 0xFFFFFFFF

        return hash_value

    def _encrypt_xor(self, data: bytes, key: int) -> bytes:
        """XOR加密"""
        key_bytes = struct.pack('<I', key)
        encrypted = bytearray()

        for i, byte in enumerate(data):
            encrypted.append(byte ^ key_bytes[i % 4])

        return bytes(encrypted)

    def _calculate_crc32_with_init(self, data: bytes, crc_init: int) -> int:
        """
        计算CRC32，支持自定义初始值

        匹配 _decrypt_N_crc32 的CRC32计算方式

        Args:
            data: 待计算的数据
            crc_init: CRC32初始值

        Returns:
            CRC32值
        """
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

        # 对每个字节计算CRC32
        for byte in data:
            crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

        return ~crc & 0xFFFFFFFF

    def _align_16(self, size: int) -> int:
        """16字节对齐"""
        return (size + 15) & ~15

    def _ror32(self, value: int, shift: int) -> int:
        """32位右旋转"""
        return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF

    def _encrypt_index(self, index_data: bytes, key: int, g_additional_key: int = 0) -> bytes:
        """
        加密索引数据

        基于 df_game_r_part24.c:26659-26663 的加密流程

        Args:
            index_data: 未加密的索引数据
            key: 加密密钥 (通常是index_crc32)
            g_additional_key: 附加密钥 (默认0)

        Returns:
            加密后的索引数据
        """
        size = len(index_data)

        # 检查对齐
        if (size & 3) != 0:
            raise ValueError(f"索引大小 {size} 不是4的倍数")

        encrypted = bytearray(index_data)

        # XOR_KEY默认为0
        # if XOR_KEY:
        #     key ^= XOR_KEY

        # 加密每个DWORD
        for i in range(0, size, 4):
            # 读取4字节
            dword = struct.unpack('<I', encrypted[i:i+4])[0]

            # 步骤1: encrypted_temp = key ^ ROR(plaintext, 26)
            dword = key ^ self._ror32(dword, 26)

            # 步骤2: encrypted_final = encrypted_temp ^ g_additional_key
            if g_additional_key:
                dword ^= g_additional_key

            # 写回
            struct.pack_into('<I', encrypted, i, dword)

        return bytes(encrypted)

    def _write_string(self, s: str) -> bytes:
        """写入字符串 (长度 + 内容)"""
        s_bytes = s.encode('utf-8')
        length = len(s_bytes)
        return struct.pack('<I', length) + s_bytes

    def build(self):
        """构建PVF文件"""
        print(f"[*] 创建测试PVF文件: {self.output_path}")
        print(f"    文件数量: {len(self.files)}")

        with open(self.output_path, 'wb') as f:
            # Step 1: 写入GUID标签
            tag_string = self.GUID.encode('utf-8')
            tag_length = len(tag_string)

            f.write(struct.pack('<I', tag_length))
            f.write(tag_string)

            print(f"    [+] GUID: {self.GUID}")

            # Step 2: 写入版本号
            revision = 12345
            f.write(struct.pack('<I', revision))
            print(f"    [+] 版本号: {revision}")

            # Step 3: 准备索引表
            index_data = bytearray()
            file_data = bytearray()
            current_offset = 0

            file_entries = []

            for file_info in self.files:
                filename = file_info['name']
                content = file_info['content']

                # 规范化文件名
                normalized_name = self._normalize_path(filename)

                # 计算哈希
                hash_value = self._djb2_hash(normalized_name)

                # 计算大小
                size = len(content)
                aligned_size = self._align_16(size)

                # 计算CRC32
                crc32 = zlib.crc32(content) & 0xFFFFFFFF

                # 加密内容
                # Padding到aligned_size
                padded_content = content + b'\x00' * (aligned_size - size)
                encrypted_content = self._encrypt_xor(padded_content, hash_value)

                # 保存文件条目
                file_entries.append({
                    'hash': hash_value,
                    'name': filename,
                    'offset': current_offset,
                    'size': size,
                    'aligned_size': aligned_size,
                    'crc32': crc32,
                    'encrypted_content': encrypted_content
                })

                # 添加到文件数据区
                file_data.extend(encrypted_content)
                current_offset += aligned_size

            # 按哈希值排序（模拟生产环境）
            file_entries.sort(key=lambda x: x['hash'])

            # 构建索引表
            for entry in file_entries:
                # 哈希值
                index_data.extend(struct.pack('<I', entry['hash']))

                # 文件名
                name_bytes = entry['name'].encode('utf-8')
                name_length = len(name_bytes)
                index_data.extend(struct.pack('<I', name_length))
                index_data.extend(name_bytes)

                # offset, size, aligned_size, crc32
                index_data.extend(struct.pack('<I', entry['offset']))
                index_data.extend(struct.pack('<I', entry['size']))
                index_data.extend(struct.pack('<I', entry['aligned_size']))
                index_data.extend(struct.pack('<I', entry['crc32']))

                print(f"    [+] [{entry['name']}] Hash=0x{entry['hash']:08x}, Size={entry['size']}, CRC32=0x{entry['crc32']:08x}")

            print(f"    [+] 索引大小(未对齐): {len(index_data)} 字节")

            # 对齐索引表
            index_data_aligned = bytes(index_data)
            aligned_index_size = self._align_16(len(index_data_aligned))
            if aligned_index_size > len(index_data_aligned):
                index_data_aligned += b'\x00' * (aligned_index_size - len(index_data_aligned))

            print(f"    [+] 索引大小(对齐): {aligned_index_size} 字节")

            # 计算索引CRC32 (使用文件数量作为初始值，匹配_decrypt_N_crc32)
            index_count = len(self.files)
            index_crc32 = self._calculate_crc32_with_init(index_data_aligned, index_count)

            print(f"    [+] 索引CRC32(明文,init={index_count}): 0x{index_crc32:08x}")

            # 加密索引 (使用index_crc32作为密钥)
            g_additional_key = 0  # 测试时先不使用附加密钥
            encrypted_index = self._encrypt_index(index_data_aligned, index_crc32, g_additional_key)

            print(f"    [+] 索引已加密 (key=0x{index_crc32:08x}, g_add={g_additional_key})")

            # Step 4: 写入索引头部信息
            index_header_size = len(encrypted_index)
            index_count = len(self.files)

            f.write(struct.pack('<I', index_header_size))
            f.write(struct.pack('<I', index_crc32))
            f.write(struct.pack('<I', index_count))

            print(f"    [+] 索引大小: {index_header_size} 字节")
            print(f"    [+] 索引CRC32: 0x{index_crc32:08x}")

            # Step 5: 写入索引数据
            f.write(encrypted_index)

            # 记录头部结束位置
            header_end = f.tell()
            print(f"    [+] 头部结束: 0x{header_end:08x} ({header_end} 字节)")

            # Step 6: 写入文件数据
            f.write(file_data)

            final_size = f.tell()
            print(f"    [+] 文件总大小: {final_size:,} 字节")

        print(f"\n[*] PVF文件创建成功: {self.output_path}")


def main():
    """创建测试PVF文件"""
    print("=" * 80)
    print("PVF测试文件生成器")
    print("=" * 80)
    print()

    builder = PVFBuilder("test_Script.pvf")

    # 添加测试文件
    builder.add_file("test/hello.txt", b"Hello, World!")
    builder.add_file("test/data.txt", b"This is a test file.\nLine 2\nLine 3")
    builder.add_file("stackable/potion.stk", b"[Potion]\ntype=hp\nvalue=100\nname=HP Potion")
    builder.add_file("equipment/weapon.equ", b"[Weapon]\ntype=sword\ndamage=50\nname=Iron Sword")
    builder.add_file("npc/merchant.npc", b"[Merchant]\nname=John\nlocation=town_square\nitems=potion,weapon")

    # 构建PVF
    builder.build()

    print()
    print("=" * 80)
    print("测试命令:")
    print("=" * 80)
    print()
    print("# 1. 解析并查看头部信息")
    print("python tools/pvf_parser.py test_Script.pvf")
    print()
    print("# 2. 列出所有文件")
    print("python tools/pvf_parser.py test_Script.pvf --list")
    print()
    print("# 3. 提取文件")
    print("python tools/pvf_parser.py test_Script.pvf --extract test/hello.txt")
    print()
    print("# 4. 搜索文件")
    print("python tools/pvf_parser.py test_Script.pvf --list --filter potion")
    print()


if __name__ == '__main__':
    main()
