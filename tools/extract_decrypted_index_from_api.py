#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
从pvfUtility API提取完整的解密索引数据

策略：通过API获取所有文件信息，重建完整的索引结构
然后与加密索引进行XOR分析，反推加密密钥流
"""

import struct
import requests
import sys
from typing import List, Tuple


def djb2_hash(s: str) -> int:
    """DJB2哈希算法（Phase 7.1.2已验证100%正确）"""
    hash_value = 5381
    for c in s:
        hash_value = ((hash_value * 33) ^ ord(c)) & 0xFFFFFFFF
    return hash_value


def get_all_files_from_api(base_url="http://localhost:27000") -> List[Tuple[str, int, int]]:
    """
    从API获取所有文件信息
    返回: [(file_path, tree_offset, file_size), ...]
    """
    print("[*] 正在从pvfUtility API获取文件列表...")

    try:
        # 获取根目录列表
        response = requests.get(f"{base_url}/Api/PvfUtiltiy/getPvfRootDirectory", timeout=30)
        if response.status_code != 200:
            print(f"[!] 获取根目录失败: HTTP {response.status_code}")
            return []

        data = response.json()
        if data.get('IsError'):
            print(f"[!] API返回错误: {data.get('Message')}")
            return []

        root_dirs = data.get('Data', [])
        print(f"[+] 找到 {len(root_dirs)} 个根目录")

        all_files = []

        # 遍历每个根目录
        for i, dir_name in enumerate(root_dirs, 1):
            print(f"[*] 处理目录 {i}/{len(root_dirs)}: {dir_name}")

            # 获取目录下的文件列表
            response = requests.get(
                f"{base_url}/Api/PvfUtiltiy/GetFileList",
                params={'dirName': dir_name, 'returnType': 1},
                timeout=30
            )

            if response.status_code != 200:
                print(f"[!] 获取目录文件列表失败: {dir_name}")
                continue

            data = response.json()
            if data.get('IsError'):
                print(f"[!] 目录 {dir_name} 返回错误")
                continue

            # 解析文件列表 (用 \r\n 分隔)
            file_list_str = data.get('Data', '')
            files = [f.strip() for f in file_list_str.split('\r\n') if f.strip()]

            print(f"[+] 目录 {dir_name} 包含 {len(files)} 个文件")

            # 对每个文件，我们需要获取其大小和偏移信息
            # 但API可能不直接提供，我们先收集文件名
            for file_path in files:
                # 暂时设置偏移和大小为0，稍后可以通过GetFileContent获取实际大小
                all_files.append((file_path, 0, 0))

        print(f"[+] 总共获取 {len(all_files)} 个文件")
        return all_files

    except Exception as e:
        print(f"[!] API请求失败: {e}")
        import traceback
        traceback.print_exc()
        return []


def build_plaintext_index(files: List[Tuple[str, int, int]]) -> bytes:
    """
    根据文件列表构建明文索引

    PVF索引格式:
    [4字节哈希][4字节文件名长度][文件名][4字节tree_offset][4字节file_size]
    """
    print("[*] 构建明文索引...")

    index_data = bytearray()

    for file_path, tree_offset, file_size in files:
        # 1. 计算DJB2哈希
        path_hash = djb2_hash(file_path)

        # 2. 文件名长度
        name_bytes = file_path.encode('utf-8')
        name_length = len(name_bytes)

        # 3. 写入索引条目
        index_data.extend(struct.pack('<I', path_hash))           # 哈希
        index_data.extend(struct.pack('<I', name_length))          # 名称长度
        index_data.extend(name_bytes)                              # 名称
        index_data.extend(struct.pack('<I', tree_offset))          # 偏移
        index_data.extend(struct.pack('<I', file_size))            # 大小

    # 对齐到16字节边界
    padding_needed = (16 - (len(index_data) % 16)) % 16
    if padding_needed:
        index_data.extend(b'\x00' * padding_needed)

    print(f"[+] 明文索引大小: {len(index_data):,} 字节")
    return bytes(index_data)


def read_encrypted_index(pvf_path: str) -> Tuple[bytes, dict]:
    """读取Script.pvf的加密索引"""
    print(f"[*] 读取 {pvf_path} 的加密索引...")

    with open(pvf_path, 'rb') as f:
        # 读取头部
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"[+] PVF信息:")
        print(f"    文件数量: {index_count:,}")
        print(f"    索引大小: {index_header_size:,} 字节")
        print(f"    索引CRC32: 0x{index_crc32:08x}")

        # 读取加密索引
        encrypted_index = f.read(index_header_size)

        return encrypted_index, {
            'index_count': index_count,
            'index_crc32': index_crc32,
            'revision': revision,
            'index_header_size': index_header_size
        }


def analyze_xor_pattern(plaintext: bytes, ciphertext: bytes, max_samples=1000):
    """
    分析明文和密文的XOR模式

    如果加密算法是: cipher = plain ^ keystream
    那么: keystream = plain ^ cipher
    """
    print("[*] 分析XOR模式...")

    # 只对齐部分进行分析
    min_size = min(len(plaintext), len(ciphertext), max_samples * 4)

    # 确保4字节对齐
    min_size = (min_size // 4) * 4

    print(f"[*] 分析前 {min_size} 字节 ({min_size//4} 个DWORD)")

    keystream = bytearray()
    for i in range(0, min_size, 4):
        plain_dword = struct.unpack('<I', plaintext[i:i+4])[0]
        cipher_dword = struct.unpack('<I', ciphertext[i:i+4])[0]

        # keystream = plain ^ cipher
        key_dword = plain_dword ^ cipher_dword
        keystream.extend(struct.pack('<I', key_dword))

    # 分析密钥流模式
    print()
    print("=" * 80)
    print("密钥流分析")
    print("=" * 80)
    print()

    # 显示前16个DWORD
    print("前16个密钥流DWORD:")
    for i in range(min(16, len(keystream) // 4)):
        key_dword = struct.unpack('<I', keystream[i*4:i*4+4])[0]
        print(f"  [{i:3d}] 0x{key_dword:08x}")
    print()

    # 检查密钥流是否重复
    print("检查密钥流模式:")

    # 检查是否为常量
    unique_keys = set()
    for i in range(0, len(keystream), 4):
        key_dword = struct.unpack('<I', keystream[i:i+4])[0]
        unique_keys.add(key_dword)

    print(f"  唯一密钥数量: {len(unique_keys)} / {len(keystream)//4}")

    if len(unique_keys) == 1:
        key = list(unique_keys)[0]
        print(f"  [+] 密钥流是常量: 0x{key:08x}")
        return key, 'constant'

    # 检查是否有周期性
    for period in [1, 2, 4, 8, 16, 32]:
        is_periodic = True
        for i in range(period, min(len(keystream)//4, 100)):
            if struct.unpack('<I', keystream[i*4:i*4+4])[0] != \
               struct.unpack('<I', keystream[(i%period)*4:(i%period)*4+4])[0]:
                is_periodic = False
                break

        if is_periodic:
            print(f"  [+] 密钥流有周期性: 周期={period}")
            return keystream[:period*4], 'periodic'

    print("  [-] 密钥流是复杂的（可能是基于CRC32的动态密钥）")

    return keystream, 'complex'


def main():
    print("=" * 80)
    print("从pvfUtility API提取解密索引")
    print("=" * 80)
    print()

    # 1. 从API获取文件列表
    files = get_all_files_from_api()

    if not files:
        print("[!] 无法从API获取文件列表")
        return False

    # 2. 构建明文索引
    plaintext_index = build_plaintext_index(files)

    # 3. 读取加密索引
    pvf_path = "C:\\Users\\waw\\Desktop\\game\\Script.pvf"
    encrypted_index, pvf_info = read_encrypted_index(pvf_path)

    # 4. 保存明文索引供参考
    with open('plaintext_index_reconstructed.bin', 'wb') as f:
        f.write(plaintext_index)
    print(f"[+] 已保存明文索引到: plaintext_index_reconstructed.bin")

    # 5. XOR分析
    print()
    keystream, pattern_type = analyze_xor_pattern(plaintext_index, encrypted_index)

    # 6. 保存密钥流
    if isinstance(keystream, bytes):
        with open('keystream_extracted.bin', 'wb') as f:
            f.write(keystream)
        print(f"[+] 已保存密钥流到: keystream_extracted.bin")

    print()
    print("=" * 80)
    print("分析完成")
    print("=" * 80)

    return True


if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
