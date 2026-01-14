#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试加密/解密逻辑 - 使用DJB2哈希算法
"""

import struct
import zlib

def normalize_path(path: str) -> str:
    """规范化文件路径"""
    result = []
    for c in path:
        if c == '\\' or c == '/':
            result.append('/')
        elif 'A' <= c <= 'Z':
            result.append(chr(ord(c) + 32))  # 转小写
        else:
            result.append(c)
    return ''.join(result)

def djb2_hash(string: str) -> int:
    """DJB2哈希算法（生产服务器实际使用）"""
    hash_value = 5381

    for char in string:
        hash_value = (hash_value * 33 + ord(char)) & 0xFFFFFFFF

    return hash_value

def encrypt_xor(data: bytes, key: int) -> bytes:
    """XOR加密"""
    key_bytes = struct.pack('<I', key)
    encrypted = bytearray()

    for i, byte in enumerate(data):
        encrypted.append(byte ^ key_bytes[i % 4])

    return bytes(encrypted)

def decrypt_xor(data: bytes, key: int) -> bytes:
    """XOR解密（与加密相同）"""
    return encrypt_xor(data, key)

# 测试
filename = "test/hello.txt"
content = b"Hello, World!"

print("="*80)
print("加密/解密测试 - DJB2哈希算法")
print("="*80)

# 规范化路径
normalized = normalize_path(filename)
print(f"\n原始路径: {filename}")
print(f"规范化路径: {normalized}")

# 计算哈希
hash_value = djb2_hash(normalized)
print(f"DJB2哈希值: 0x{hash_value:08x}")

# 加密
encrypted = encrypt_xor(content, hash_value)
print(f"\n原始数据: {content}")
print(f"加密数据: {encrypted.hex()}")

# 解密
decrypted = decrypt_xor(encrypted, hash_value)
print(f"解密数据: {decrypted}")

# 验证
if decrypted == content:
    print("\n[+] 加密/解密成功!")
else:
    print("\n[-] 加密/解密失败!")

# 计算CRC32
crc32_value = zlib.crc32(content) & 0xFFFFFFFF
print(f"\nCRC32: 0x{crc32_value:08x}")
