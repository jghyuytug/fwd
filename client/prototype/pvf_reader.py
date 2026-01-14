#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF Reader - 尝试多种解密方式
支持加密和未加密的PVF文件
"""

import struct
import sys
import zlib
from pathlib import Path
from dataclasses import dataclass
from typing import List, Optional, BinaryIO


@dataclass
class PVFHeader:
    tag_length: int
    tag_string: str
    revision: int
    index_header_size: int
    index_crc32: int
    index_count: int


@dataclass
class IndexEntry:
    name: str
    offset: int
    size: int
    decrypt_key: int
    hash_value: int


class PVFReader:
    """PVF读取器 - 支持多种加密方式"""

    # 已知的AdditionalKey值
    KNOWN_KEYS = [
        0x81A79011,  # 国服/韩服
        0x00000000,  # 无加密
        0xCE24D9AF,  # 其他版本
        0x12345678,  # 测试
    ]

    def __init__(self, filepath: str):
        self.filepath = Path(filepath)
        self.header: Optional[PVFHeader] = None
        self.header_size: int = 0
        self.indexes: List[IndexEntry] = []
        self.fp: Optional[BinaryIO] = None
        self.additional_key = 0x81A79011  # 默认密钥

    def open(self) -> bool:
        """打开PVF并解析头部"""
        try:
            self.fp = open(self.filepath, 'rb')

            # 读取GUID长度
            tag_length = struct.unpack('<I', self.fp.read(4))[0]

            # 读取GUID
            tag_string = self.fp.read(tag_length).decode('utf-8', errors='ignore')

            # 读取其他头部字段
            revision = struct.unpack('<I', self.fp.read(4))[0]
            index_header_size = struct.unpack('<I', self.fp.read(4))[0]
            index_crc32 = struct.unpack('<I', self.fp.read(4))[0]
            index_count = struct.unpack('<I', self.fp.read(4))[0]

            self.header = PVFHeader(
                tag_length=tag_length,
                tag_string=tag_string,
                revision=revision,
                index_header_size=index_header_size,
                index_crc32=index_crc32,
                index_count=index_count
            )

            self.header_size = self.fp.tell()

            print(f"[PVF] GUID: {tag_string}")
            print(f"[PVF] Revision: {revision}")
            print(f"[PVF] Files: {index_count}")
            print(f"[PVF] Index Size: {index_header_size:,} bytes")

            return True

        except Exception as e:
            print(f"[PVF] Open failed: {e}")
            return False

    def _ror32(self, value: int, shift: int) -> int:
        """32位右旋转"""
        return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF

    def _make_crc_table(self):
        """生成CRC32表"""
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

    def _try_decrypt_index(self, encrypted_data: bytes, additional_key: int) -> Optional[bytes]:
        """尝试用指定密钥解密索引"""
        size = len(encrypted_data)
        if (size & 3) != 0:
            return None

        crc_table = self._make_crc_table()
        crc = (~self.header.index_count) & 0xFFFFFFFF
        key = self.header.index_crc32

        decrypted = bytearray(encrypted_data)

        for i in range(0, size, 4):
            dword = struct.unpack('<I', decrypted[i:i+4])[0]

            # XOR additional key
            if additional_key:
                dword ^= additional_key

            # ROR(key ^ data, 6)
            dword = self._ror32(key ^ dword, 6)

            struct.pack_into('<I', decrypted, i, dword)

            # Update CRC
            for j in range(4):
                byte = decrypted[i + j]
                crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

        final_crc = (~crc) & 0xFFFFFFFF

        if final_crc == self.header.index_crc32:
            return bytes(decrypted)
        return None

    def parse_index(self, max_entries: int = 0) -> bool:
        """解析索引，自动尝试多种解密方式"""
        if not self.header:
            return False

        # 读取加密索引数据
        encrypted_index = self.fp.read(self.header.index_header_size)
        if len(encrypted_index) != self.header.index_header_size:
            print("[PVF] Index data incomplete")
            return False

        # 尝试各种密钥
        decrypted_index = None
        for key in self.KNOWN_KEYS:
            print(f"[PVF] Trying key 0x{key:08X}...", end=" ")
            result = self._try_decrypt_index(encrypted_index, key)
            if result:
                print("OK!")
                decrypted_index = result
                self.additional_key = key
                break
            else:
                print("Failed")

        if decrypted_index is None:
            print("[PVF] All decryption keys failed")
            return False

        # 解析索引条目
        offset = 0
        count = max_entries if max_entries > 0 else self.header.index_count
        count = min(count, self.header.index_count)

        for i in range(count):
            try:
                if offset + 4 > len(decrypted_index):
                    break

                hash_value = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                offset += 4

                name_length = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                offset += 4

                name = decrypted_index[offset:offset+name_length].decode('utf-8', errors='ignore')
                offset += name_length

                file_size = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                offset += 4
                decrypt_key = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                offset += 4
                file_offset = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                offset += 4

                entry = IndexEntry(
                    name=name,
                    offset=file_offset,
                    size=file_size,
                    decrypt_key=decrypt_key,
                    hash_value=hash_value
                )
                self.indexes.append(entry)

            except Exception as e:
                print(f"[PVF] Parse entry {i} failed: {e}")
                break

        print(f"[PVF] Parsed {len(self.indexes)} entries")
        return len(self.indexes) > 0

    def _decrypt_file_data(self, data: bytes, key: int) -> bytes:
        """解密文件数据"""
        decrypted = bytearray(data)
        size = len(decrypted)

        for i in range(0, size - (size % 4), 4):
            dword = struct.unpack('<I', decrypted[i:i+4])[0]

            if self.additional_key:
                dword ^= self.additional_key

            dword = self._ror32(key ^ dword, 6)
            struct.pack_into('<I', decrypted, i, dword)

        return bytes(decrypted)

    def get_file(self, filepath: str) -> Optional[IndexEntry]:
        """按路径查找文件"""
        # 规范化路径
        normalized = filepath.lower().replace('\\', '/')

        for entry in self.indexes:
            if entry.name.lower().replace('\\', '/') == normalized:
                return entry
        return None

    def extract_file(self, entry: IndexEntry) -> Optional[bytes]:
        """提取文件内容"""
        try:
            # 计算文件位置
            data_start = self.header_size + self.header.index_header_size
            file_pos = data_start + entry.offset

            self.fp.seek(file_pos)

            # 读取对齐后的大小
            aligned_size = ((entry.size + 3) // 4) * 4
            encrypted_data = self.fp.read(aligned_size)

            # 解密
            decrypted = self._decrypt_file_data(encrypted_data, entry.decrypt_key)

            # 返回实际大小
            return decrypted[:entry.size]

        except Exception as e:
            print(f"[PVF] Extract failed: {e}")
            return None

    def list_files(self, pattern: str = None, limit: int = 20):
        """列出文件"""
        count = 0
        for entry in self.indexes:
            if pattern and pattern.lower() not in entry.name.lower():
                continue
            print(f"  {entry.name} ({entry.size:,} bytes)")
            count += 1
            if limit > 0 and count >= limit:
                print(f"  ... ({len(self.indexes) - count} more)")
                break

    def close(self):
        if self.fp:
            self.fp.close()
            self.fp = None


def main():
    if len(sys.argv) < 2:
        print("Usage: python pvf_reader.py <pvf_file> [--list] [--filter <pattern>] [--extract <file>]")
        return 1

    pvf_path = sys.argv[1]

    reader = PVFReader(pvf_path)

    if not reader.open():
        return 1

    # 解析索引（全部）
    if not reader.parse_index():
        reader.close()
        return 1

    # 命令行参数处理
    if "--list" in sys.argv:
        pattern = None
        if "--filter" in sys.argv:
            idx = sys.argv.index("--filter")
            if idx + 1 < len(sys.argv):
                pattern = sys.argv[idx + 1]
        reader.list_files(pattern, limit=50)

    if "--extract" in sys.argv:
        idx = sys.argv.index("--extract")
        if idx + 1 < len(sys.argv):
            filepath = sys.argv[idx + 1]
            entry = reader.get_file(filepath)
            if entry:
                data = reader.extract_file(entry)
                if data:
                    output_name = filepath.replace('/', '_').replace('\\', '_')
                    with open(output_name, 'wb') as f:
                        f.write(data)
                    print(f"[PVF] Extracted to {output_name}")
            else:
                print(f"[PVF] File not found: {filepath}")

    reader.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
