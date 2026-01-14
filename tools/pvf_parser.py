#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script.pvf Parser - Proof of Concept

Based on production server analysis (2025-11-10):
- File format: GUID + encrypted index + encrypted file data
- Encryption: XOR with filename hash as key
- Verification: CRC32 checksums
- Indexing: Hash-based with binary search

Usage:
    python pvf_parser.py <path_to_Script.pvf>

Example:
    python pvf_parser.py /home/dxf/game/Script.pvf
    python pvf_parser.py Script.pvf --list-files
    python pvf_parser.py Script.pvf --extract stackable/potion.stk
"""

import struct
import sys
import zlib
from pathlib import Path
from typing import List, Tuple, Optional, BinaryIO
from dataclasses import dataclass


@dataclass
class PVFHeader:
    """PVF文件头部结构"""
    tag_length: int
    tag_string: str
    revision: int
    index_header_size: int
    index_crc32: int
    index_count: int


@dataclass
class IndexEntry:
    """索引条目结构"""
    name: str
    offset: int
    size: int
    aligned_size: int
    crc32: int  # 实际是unknown_field，保留字段名兼容性
    hash_value: int


class PVFParser:
    """Script.pvf解析器"""

    # 预期的GUID (从生产服务器获得)
    EXPECTED_GUID = "fa08bf71-4395-6a4b-a3e3-2617c9fee116"

    def __init__(self, filepath: str):
        self.filepath = Path(filepath)
        self.header: Optional[PVFHeader] = None
        self.header_size: int = 0
        self.indexes: List[IndexEntry] = []
        self.fp: Optional[BinaryIO] = None

    def open(self) -> bool:
        """打开PVF文件并解析头部"""
        try:
            self.fp = open(self.filepath, 'rb')

            # Step 1: 读取GUID标签长度
            tag_length_bytes = self.fp.read(4)
            if len(tag_length_bytes) != 4:
                print(f"[-] 无法读取标签长度")
                return False

            tag_length = struct.unpack('<I', tag_length_bytes)[0]
            print(f"[+] 标签长度: {tag_length} 字节")

            # Step 2: 读取GUID字符串
            tag_string_bytes = self.fp.read(tag_length)
            if len(tag_string_bytes) != tag_length:
                print(f"[-] 无法读取完整标签 (期望 {tag_length} 字节)")
                return False

            tag_string = tag_string_bytes.decode('utf-8', errors='ignore')
            print(f"[+] GUID: {tag_string}")

            # 验证GUID
            if tag_string != self.EXPECTED_GUID:
                print(f"[!]  警告: GUID不匹配")
                print(f"   期望: {self.EXPECTED_GUID}")
                print(f"   实际: {tag_string}")

            # Step 3: 读取版本号
            revision_bytes = self.fp.read(4)
            revision = struct.unpack('<I', revision_bytes)[0]
            print(f"[+] 版本号: {revision}")

            # Step 4: 读取索引头部大小
            index_header_size_bytes = self.fp.read(4)
            index_header_size = struct.unpack('<I', index_header_size_bytes)[0]
            print(f"[+] 索引头部大小: {index_header_size} 字节")

            # Step 5: 读取索引CRC32
            index_crc32_bytes = self.fp.read(4)
            index_crc32 = struct.unpack('<I', index_crc32_bytes)[0]
            print(f"[+] 索引CRC32: 0x{index_crc32:08x}")

            # Step 6: 读取索引数量
            index_count_bytes = self.fp.read(4)
            index_count = struct.unpack('<I', index_count_bytes)[0]
            print(f"[+] 文件数量: {index_count}")

            # 创建头部对象
            self.header = PVFHeader(
                tag_length=tag_length,
                tag_string=tag_string,
                revision=revision,
                index_header_size=index_header_size,
                index_crc32=index_crc32,
                index_count=index_count
            )

            # 记录头部结束位置
            self.header_size = self.fp.tell()
            print(f"[+] 头部结束位置: 0x{self.header_size:08x} ({self.header_size} 字节)")

            return True

        except Exception as e:
            print(f"[-] 打开文件失败: {e}")
            return False

    def _read_string(self, data: bytes, offset: int) -> Tuple[str, int]:
        """
        从字节流中读取字符串

        格式: uint32_t length + char[length]

        Returns:
            (string, new_offset)
        """
        # 读取字符串长度
        str_length = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # 读取字符串内容
        str_bytes = data[offset:offset+str_length]
        offset += str_length

        # 解码为UTF-8
        string = str_bytes.decode('utf-8', errors='ignore')

        return string, offset

    def _decrypt_file_data(self, data: bytes, key: int) -> bytes:
        """
        文件数据解密（使用与索引相同的算法）

        算法：XOR g_AdditionalKey + ROR(key ^ data, 6)

        Args:
            data: 加密的数据
            key: 解密密钥（来自索引的unknown字段，现称为decrypt_key）

        Returns:
            解密后的数据
        """
        g_AdditionalKey = 0x81A79011  # 从conf目录获取的正确值

        decrypted = bytearray(data)
        size = len(decrypted)

        # 按4字节处理
        for i in range(0, size, 4):
            if i + 4 > size:
                break

            dword = struct.unpack('<I', decrypted[i:i+4])[0]

            # 步骤1: XOR g_AdditionalKey
            if g_AdditionalKey:
                dword ^= g_AdditionalKey

            # 步骤2: ROR(key ^ data, 6) 解密
            dword = self._ror32(key ^ dword, 6)

            struct.pack_into('<I', decrypted, i, dword)

        return bytes(decrypted)

    def _verify_crc32(self, data: bytes, expected_crc: int) -> bool:
        """
        验证CRC32校验和

        Args:
            data: 待验证的数据
            expected_crc: 期望的CRC32值

        Returns:
            True if CRC matches, False otherwise
        """
        calculated_crc = zlib.crc32(data) & 0xFFFFFFFF
        return calculated_crc == expected_crc

    def _normalize_path(self, path: str) -> str:
        """
        规范化文件路径（与生产服务器一致）

        基于 _correct_filename (df_game_r_part24.c:26750)

        规则:
        1. 路径分隔符统一为 '/'
        2. 大写字母转小写
        3. 其他字符保持不变
        """
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
        """
        DJB2哈希算法（生产服务器实际使用）

        基于 _hash (df_game_r_part24.c:26716)

        公式: hash = 5381
              for each char: hash = hash * 33 + char

        Args:
            string: 待哈希的字符串（已规范化的文件名）

        Returns:
            32位哈希值
        """
        hash_value = 5381

        for char in string:
            hash_value = (hash_value * 33 + ord(char)) & 0xFFFFFFFF

        return hash_value

    def _ror32(self, value: int, shift: int) -> int:
        """32位右旋转"""
        return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF

    def _decrypt_index(self, encrypted_data: bytes, crc_init: int, checksum: int, key: int) -> Optional[bytes]:
        """
        解密索引数据

        基于 _decrypt_N_crc32 (df_game_r_part24.c:26670)

        Args:
            encrypted_data: 加密的索引数据
            crc_init: CRC32初始值 (文件数量)
            checksum: 期望的CRC32校验和 (index_crc32)
            key: 解密密钥 (index_crc32)

        Returns:
            解密后的数据，如果CRC验证失败返回None
        """
        size = len(encrypted_data)

        # 检查对齐（必须是4的倍数）
        if (size & 3) != 0:
            print(f"[!] 索引数据大小 {size} 不是4的倍数")
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

        # 关键参数（从最新反汇编代码获取）
        XOR_KEY = 0
        g_AdditionalKey = 0x81A79011  # 正确的值！

        # 修改密钥
        if XOR_KEY:
            key ^= XOR_KEY

        # 解密
        decrypted = bytearray(encrypted_data)

        for i in range(0, size, 4):
            # 读取4字节
            dword = struct.unpack('<I', decrypted[i:i+4])[0]

            # 步骤1: XOR g_AdditionalKey
            if g_AdditionalKey:
                dword ^= g_AdditionalKey

            # 步骤2: ROR(key ^ data, 6) 解密
            dword = self._ror32(key ^ dword, 6)

            # 写回
            struct.pack_into('<I', decrypted, i, dword)

            # 更新CRC32
            for j in range(4):
                byte = decrypted[i + j]
                crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

        # 验证CRC32
        final_crc = ~crc & 0xFFFFFFFF

        if final_crc != checksum:
            print(f"[!] 索引CRC32不匹配:")
            print(f"    期望: 0x{checksum:08x}")
            print(f"    实际: 0x{final_crc:08x}")
            return None

        print(f"[+] 索引CRC32验证通过: 0x{final_crc:08x}")
        return bytes(decrypted)

    def parse_index(self) -> bool:
        """解析加密的索引表"""
        if not self.header:
            print("[-] 头部未解析")
            return False

        try:
            # Step 1: 读取加密的索引数据
            print(f"\n[*] 读取加密索引数据 ({self.header.index_header_size} 字节)...")
            encrypted_index = self.fp.read(self.header.index_header_size)

            if len(encrypted_index) != self.header.index_header_size:
                print(f"[-] 索引数据不完整")
                return False

            # Step 2: 解密索引
            print(f"[*] 解密索引数据...")
            decrypted_index = self._decrypt_index(
                encrypted_index,
                self.header.index_count,  # CRC初始值 = 文件数量
                self.header.index_crc32,  # 期望的CRC32
                self.header.index_crc32   # 解密密钥 = index_crc32
            )

            if decrypted_index is None:
                print(f"[-] 索引解密失败")
                return False

            print(f"[+] 索引解密成功")

            # Step 3: 解析索引条目
            offset = 0
            for i in range(self.header.index_count):
                try:
                    # 读取哈希值 (4 bytes)
                    if offset + 4 > len(decrypted_index):
                        print(f"[!]  索引 {i}: 数据不足，无法读取哈希值")
                        break

                    hash_value = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 读取文件名
                    if offset + 4 > len(decrypted_index):
                        print(f"[!]  索引 {i}: 数据不足，无法读取文件名长度")
                        break

                    filename, offset = self._read_string(decrypted_index, offset)

                    # 读取file_size (4 bytes) - 压缩后大小
                    if offset + 4 > len(decrypted_index):
                        print(f"[!]  索引 {i}: 数据不足，无法读取size")
                        break

                    file_size = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 读取unknown field (4 bytes) - 可能是tree_offset或其他
                    if offset + 4 > len(decrypted_index):
                        print(f"[!]  索引 {i}: 数据不足，无法读取unknown field")
                        break

                    unknown_field = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 读取file_offset (4 bytes) - 数据区偏移(4字节对齐)
                    if offset + 4 > len(decrypted_index):
                        print(f"[!]  索引 {i}: 数据不足，无法读取offset")
                        break

                    file_offset = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 计算aligned_size (for compatibility)
                    aligned_size = ((file_size + 3) // 4) * 4

                    # 创建索引条目
                    entry = IndexEntry(
                        name=filename,
                        offset=file_offset,
                        size=file_size,
                        aligned_size=aligned_size,
                        crc32=unknown_field,  # 存储unknown_field
                        hash_value=hash_value
                    )

                    self.indexes.append(entry)

                    # 打印前10个条目作为示例
                    if i < 10:
                        print(f"  [{i}] {filename}")
                        print(f"      Hash: 0x{hash_value:08x}, Offset: {file_offset}, Size: {file_size}")

                except Exception as e:
                    print(f"[!]  解析索引 {i} 失败: {e}")
                    break

            if len(self.indexes) > 10:
                print(f"  ... (省略 {len(self.indexes) - 10} 个条目)")

            print(f"\n[+] 成功解析 {len(self.indexes)}/{self.header.index_count} 个索引条目")

            if len(self.indexes) < self.header.index_count:
                print(f"[!]  索引可能已加密，需要实现解密逻辑")

            return len(self.indexes) > 0

        except Exception as e:
            print(f"[-] 解析索引失败: {e}")
            import traceback
            traceback.print_exc()
            return False

    def list_files(self, pattern: str = None):
        """列出所有文件"""
        if not self.indexes:
            print("[-] 索引为空，请先解析索引")
            return

        print(f"\n[DIR] 文件列表 (共 {len(self.indexes)} 个):")
        print("=" * 80)

        for i, entry in enumerate(self.indexes):
            if pattern and pattern.lower() not in entry.name.lower():
                continue

            print(f"{i+1:5d}. {entry.name}")
            print(f"       大小: {entry.size:,} 字节, CRC32: 0x{entry.crc32:08x}")

    def extract_file(self, filename: str, output_path: str = None) -> bool:
        """
        提取指定文件

        Args:
            filename: 文件名（相对路径）
            output_path: 输出路径（可选）

        Returns:
            True if successful, False otherwise
        """
        # 查找文件索引
        entry = None
        for idx in self.indexes:
            if idx.name == filename:
                entry = idx
                break

        if not entry:
            print(f"[-] 文件未找到: {filename}")
            return False

        print(f"\n[PKG] 提取文件: {filename}")
        print(f"   大小: {entry.size:,} 字节")
        print(f"   偏移: 0x{entry.offset:08x}")
        print(f"   存储的哈希: 0x{entry.hash_value:08x}")

        try:
            # 规范化文件名并计算哈希
            normalized_name = self._normalize_path(filename)
            calculated_hash = self._djb2_hash(normalized_name)

            print(f"   规范化路径: {normalized_name}")
            print(f"   计算的哈希: 0x{calculated_hash:08x}")

            # 验证哈希是否匹配
            if calculated_hash != entry.hash_value:
                print(f"[!]  警告: 哈希不匹配")
                print(f"      存储: 0x{entry.hash_value:08x}")
                print(f"      计算: 0x{calculated_hash:08x}")
            else:
                print(f"[+] 哈希匹配")

            # 文件解密密钥 = entry.crc32 (索引中的unknown字段，实际是decrypt_key)
            decrypt_key = entry.crc32
            print(f"   解密密钥: 0x{decrypt_key:08x}")

            # 定位到文件位置
            # 文件数据区 = 头部 + 索引数据 + entry.offset
            file_position = self.header_size + self.header.index_header_size + entry.offset
            self.fp.seek(file_position)

            # 读取加密数据
            encrypted_data = self.fp.read(entry.aligned_size)

            if len(encrypted_data) != entry.aligned_size:
                print(f"[-] 数据读取不完整")
                return False

            # 使用正确的解密算法（与索引解密相同）
            decrypted_data = self._decrypt_file_data(encrypted_data, decrypt_key)

            # 只取原始大小的数据 (去掉padding)
            actual_data = decrypted_data[:entry.size]

            # 注意：entry.crc32实际上是decrypt_key，不是文件内容的CRC32校验和
            # 因此不进行CRC32验证
            print(f"[+] 文件解密完成 ({len(actual_data)} 字节)")

            # 输出文件
            if output_path is None:
                output_path = filename.replace('/', '_').replace('\\', '_')

            output_file = Path(output_path)
            output_file.parent.mkdir(parents=True, exist_ok=True)

            with open(output_file, 'wb') as f:
                f.write(actual_data)

            print(f"[+] 文件已保存: {output_file}")
            return True

        except Exception as e:
            print(f"[-] 提取文件失败: {e}")
            import traceback
            traceback.print_exc()
            return False

    def close(self):
        """关闭文件"""
        if self.fp:
            self.fp.close()
            self.fp = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


def main():
    """主函数"""
    import argparse

    parser = argparse.ArgumentParser(
        description='Script.pvf解析器 - 提取游戏脚本文件',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  # 解析PVF文件并列出所有文件
  python pvf_parser.py Script.pvf --list

  # 搜索特定文件
  python pvf_parser.py Script.pvf --list --filter potion

  # 提取文件
  python pvf_parser.py Script.pvf --extract stackable/potion.stk

  # 提取文件到指定路径
  python pvf_parser.py Script.pvf --extract stackable/potion.stk --output potion.stk
"""
    )

    parser.add_argument('pvf_file', help='Script.pvf文件路径')
    parser.add_argument('--list', action='store_true', help='列出所有文件')
    parser.add_argument('--filter', help='过滤文件名')
    parser.add_argument('--extract', help='提取指定文件')
    parser.add_argument('--output', help='输出文件路径')

    args = parser.parse_args()

    # 检查文件是否存在
    if not Path(args.pvf_file).exists():
        print(f"[-] 文件不存在: {args.pvf_file}")
        return 1

    print("=" * 80)
    print("Script.pvf解析器 - Proof of Concept")
    print("=" * 80)
    print(f"\n[DIR] 打开文件: {args.pvf_file}")
    print(f"   大小: {Path(args.pvf_file).stat().st_size:,} 字节\n")

    # 解析PVF文件
    with PVFParser(args.pvf_file) as pvf:
        # Step 1: 打开并解析头部
        if not pvf.open():
            print("\n[-] 解析失败")
            return 1

        # Step 2: 解析索引
        if not pvf.parse_index():
            print("\n[!]  索引解析失败，可能需要正确的解密密钥")
            # 即使索引解析失败，我们也可以看到头部信息

        # Step 3: 列出文件
        if args.list or args.filter:
            pvf.list_files(pattern=args.filter)

        # Step 4: 提取文件
        if args.extract:
            pvf.extract_file(args.extract, args.output)

    print("\n[+] 完成")
    return 0


if __name__ == '__main__':
    sys.exit(main())
