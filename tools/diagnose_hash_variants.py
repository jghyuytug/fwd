#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
哈希算法变体诊断工具

测试不同的FNV-1a实现和路径规范化方式，
找出与生产环境匹配的组合。

Usage:
    python tools/diagnose_hash_variants.py Script.pvf
"""

import struct
import sys
import zlib
from pathlib import Path
from typing import List, Tuple, Callable


class HashVariants:
    """不同的哈希算法变体"""

    @staticmethod
    def fnv1a_standard(string: str) -> int:
        """标准FNV-1a (小写)"""
        FNV_OFFSET_BASIS = 2166136261
        FNV_PRIME = 16777619

        hash_value = FNV_OFFSET_BASIS
        string_lower = string.lower()

        for char in string_lower:
            hash_value ^= ord(char)
            hash_value = (hash_value * FNV_PRIME) & 0xFFFFFFFF

        return hash_value

    @staticmethod
    def fnv1a_uppercase(string: str) -> int:
        """FNV-1a (大写)"""
        FNV_OFFSET_BASIS = 2166136261
        FNV_PRIME = 16777619

        hash_value = FNV_OFFSET_BASIS
        string_upper = string.upper()

        for char in string_upper:
            hash_value ^= ord(char)
            hash_value = (hash_value * FNV_PRIME) & 0xFFFFFFFF

        return hash_value

    @staticmethod
    def fnv1a_original(string: str) -> int:
        """FNV-1a (保持原样)"""
        FNV_OFFSET_BASIS = 2166136261
        FNV_PRIME = 16777619

        hash_value = FNV_OFFSET_BASIS

        for char in string:
            hash_value ^= ord(char)
            hash_value = (hash_value * FNV_PRIME) & 0xFFFFFFFF

        return hash_value

    @staticmethod
    def fnv1_standard(string: str) -> int:
        """FNV-1 (非FNV-1a, 小写)"""
        FNV_OFFSET_BASIS = 2166136261
        FNV_PRIME = 16777619

        hash_value = FNV_OFFSET_BASIS
        string_lower = string.lower()

        for char in string_lower:
            hash_value = (hash_value * FNV_PRIME) & 0xFFFFFFFF
            hash_value ^= ord(char)

        return hash_value


class PathNormalizers:
    """不同的路径规范化方式"""

    @staticmethod
    def no_change(path: str) -> str:
        """不改变"""
        return path

    @staticmethod
    def to_lowercase(path: str) -> str:
        """转换为小写"""
        return path.lower()

    @staticmethod
    def to_uppercase(path: str) -> str:
        """转换为大写"""
        return path.upper()

    @staticmethod
    def backslash_to_slash(path: str) -> str:
        """反斜杠转正斜杠"""
        return path.replace('\\', '/')

    @staticmethod
    def slash_to_backslash(path: str) -> str:
        """正斜杠转反斜杠"""
        return path.replace('/', '\\')

    @staticmethod
    def lowercase_and_slash(path: str) -> str:
        """小写 + 正斜杠"""
        return path.replace('\\', '/').lower()

    @staticmethod
    def lowercase_and_backslash(path: str) -> str:
        """小写 + 反斜杠"""
        return path.replace('/', '\\').lower()


def decrypt_xor(data: bytes, key: int) -> bytes:
    """XOR解密"""
    key_bytes = struct.pack('<I', key)
    decrypted = bytearray()

    for i, byte in enumerate(data):
        decrypted.append(byte ^ key_bytes[i % 4])

    return bytes(decrypted)


def test_variant(pvf_path: Path,
                 filename: str,
                 file_offset: int,
                 file_size: int,
                 aligned_size: int,
                 expected_crc32: int,
                 header_size: int,
                 index_size: int,
                 hash_func: Callable,
                 path_normalizer: Callable,
                 hash_name: str,
                 norm_name: str) -> bool:
    """
    测试特定的哈希算法和路径规范化组合

    Returns:
        True if CRC32 matches
    """
    try:
        # 应用路径规范化
        normalized_path = path_normalizer(filename)

        # 计算哈希
        hash_value = hash_func(normalized_path)

        # 读取加密数据
        with open(pvf_path, 'rb') as f:
            file_position = header_size + index_size + file_offset
            f.seek(file_position)
            encrypted_data = f.read(aligned_size)

        # XOR解密
        decrypted_data = decrypt_xor(encrypted_data, hash_value)

        # 取实际大小
        actual_data = decrypted_data[:file_size]

        # 验证CRC32
        calculated_crc32 = zlib.crc32(actual_data) & 0xFFFFFFFF

        match = (calculated_crc32 == expected_crc32)

        if match:
            print(f"[+] 匹配成功!")
            print(f"    哈希算法: {hash_name}")
            print(f"    路径规范化: {norm_name}")
            print(f"    原始路径: {filename}")
            print(f"    规范化路径: {normalized_path}")
            print(f"    哈希值: 0x{hash_value:08x}")
            print(f"    CRC32: 0x{calculated_crc32:08x} (匹配 0x{expected_crc32:08x})")
            print()

        return match

    except Exception as e:
        return False


def parse_pvf_basic(pvf_path: Path) -> Tuple[int, int, List[dict]]:
    """
    基础PVF解析（获取头部和索引信息）

    Returns:
        (header_size, index_size, index_entries)
    """
    with open(pvf_path, 'rb') as f:
        # 读取头部
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8', errors='ignore')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        header_size = f.tell()

        # 读取索引（假设未加密）
        index_data = f.read(index_header_size)

        # 解析索引条目
        entries = []
        offset = 0

        for i in range(min(index_count, 10)):  # 只解析前10个
            try:
                # 哈希值
                hash_value = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                # 文件名长度
                name_length = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                # 文件名
                filename = index_data[offset:offset+name_length].decode('utf-8', errors='ignore')
                offset += name_length

                # offset, size, aligned_size, crc32
                file_offset = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                file_size = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                aligned_size = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                file_crc32 = struct.unpack('<I', index_data[offset:offset+4])[0]
                offset += 4

                entries.append({
                    'filename': filename,
                    'stored_hash': hash_value,
                    'offset': file_offset,
                    'size': file_size,
                    'aligned_size': aligned_size,
                    'crc32': file_crc32
                })

            except Exception as e:
                break

        return header_size, index_header_size, entries


def main():
    if len(sys.argv) < 2:
        print("Usage: python diagnose_hash_variants.py <Script.pvf>")
        return 1

    pvf_path = Path(sys.argv[1])

    if not pvf_path.exists():
        print(f"[-] 文件不存在: {pvf_path}")
        return 1

    print("=" * 80)
    print("哈希算法变体诊断工具")
    print("=" * 80)
    print()

    # 解析PVF基础信息
    print("[*] 解析PVF文件...")
    header_size, index_size, entries = parse_pvf_basic(pvf_path)

    print(f"[+] 头部大小: {header_size} 字节")
    print(f"[+] 索引大小: {index_size} 字节")
    print(f"[+] 解析到 {len(entries)} 个索引条目")
    print()

    if not entries:
        print("[-] 无法解析索引条目，索引可能已加密")
        return 1

    # 定义所有变体
    hash_variants = [
        (HashVariants.fnv1a_standard, "FNV-1a (小写)"),
        (HashVariants.fnv1a_uppercase, "FNV-1a (大写)"),
        (HashVariants.fnv1a_original, "FNV-1a (原样)"),
        (HashVariants.fnv1_standard, "FNV-1 (小写)"),
    ]

    path_normalizers = [
        (PathNormalizers.no_change, "不改变"),
        (PathNormalizers.to_lowercase, "小写"),
        (PathNormalizers.to_uppercase, "大写"),
        (PathNormalizers.backslash_to_slash, "反斜杠->正斜杠"),
        (PathNormalizers.slash_to_backslash, "正斜杠->反斜杠"),
        (PathNormalizers.lowercase_and_slash, "小写+正斜杠"),
        (PathNormalizers.lowercase_and_backslash, "小写+反斜杠"),
    ]

    # 测试每个文件
    print("[*] 开始测试变体组合...")
    print()

    matches_found = []

    for entry_idx, entry in enumerate(entries):
        print(f"[{entry_idx + 1}/{len(entries)}] 测试文件: {entry['filename']}")
        print(f"    大小: {entry['size']} 字节")
        print(f"    存储的哈希: 0x{entry['stored_hash']:08x}")
        print(f"    期望CRC32: 0x{entry['crc32']:08x}")
        print()

        # 测试所有组合
        found_match = False

        for hash_func, hash_name in hash_variants:
            for norm_func, norm_name in path_normalizers:
                if test_variant(
                    pvf_path,
                    entry['filename'],
                    entry['offset'],
                    entry['size'],
                    entry['aligned_size'],
                    entry['crc32'],
                    header_size,
                    index_size,
                    hash_func,
                    norm_func,
                    hash_name,
                    norm_name
                ):
                    found_match = True
                    matches_found.append({
                        'file': entry['filename'],
                        'hash': hash_name,
                        'norm': norm_name
                    })

        if not found_match:
            print(f"[-] 文件 {entry['filename']} 未找到匹配的变体")
            print()

    # 总结
    print("=" * 80)
    print("诊断总结")
    print("=" * 80)
    print()

    if matches_found:
        print(f"[+] 找到 {len(matches_found)} 个匹配")
        print()

        # 统计最常见的组合
        from collections import Counter
        combinations = [(m['hash'], m['norm']) for m in matches_found]
        most_common = Counter(combinations).most_common(1)[0]

        print(f"[+] 推荐使用的组合:")
        print(f"    哈希算法: {most_common[0][0]}")
        print(f"    路径规范化: {most_common[0][1]}")
        print(f"    匹配率: {most_common[1]}/{len(matches_found)}")
        print()

        print("[*] 请修改 tools/pvf_parser.py 中的以下函数:")
        print()

        if most_common[0][0] == "FNV-1a (小写)":
            print("# _fnv1a_hash 函数保持不变")
        elif most_common[0][0] == "FNV-1a (大写)":
            print("# 修改 _fnv1a_hash 函数，使用 string.upper()")
        elif most_common[0][0] == "FNV-1a (原样)":
            print("# 修改 _fnv1a_hash 函数，去掉 .lower()")
        elif most_common[0][0] == "FNV-1 (小写)":
            print("# 修改 _fnv1a_hash 函数，交换 XOR 和 乘法 的顺序")

        print()

        if most_common[0][1] != "不改变":
            print(f"# 在计算哈希前，对文件名进行: {most_common[0][1]}")

    else:
        print("[-] 未找到任何匹配的变体")
        print()
        print("[!] 可能的原因:")
        print("    1. 索引已加密（但解析成功说明不太可能）")
        print("    2. 使用了完全不同的哈希算法")
        print("    3. XOR解密方式不同")
        print()
        print("[*] 建议:")
        print("    1. 检查反编译代码中的哈希函数实现")
        print("    2. 使用IDA查看 PackSet::get() 函数")
        print("    3. 分析 compute_hash() 或类似函数")

    return 0


if __name__ == '__main__':
    sys.exit(main())
