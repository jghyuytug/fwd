#!/usr/bin/env python3
"""
PVF 文件目录结构提取工具
根据 PVF_FORMAT_SPECIFICATION.md 实现
"""
import struct
import os
from collections import defaultdict

def read_pvf_file_list(pvf_path):
    """读取 PVF 文件索引，提取文件路径"""
    with open(pvf_path, 'rb') as f:
        # === 读取头部 ===
        # tag_length (4) + guid (N) + revision (4) + index_size (4) + index_crc (4) + file_count (4)
        tag_len = struct.unpack('<I', f.read(4))[0]
        guid = f.read(tag_len).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_size = struct.unpack('<I', f.read(4))[0]
        index_crc = struct.unpack('<I', f.read(4))[0]
        file_count = struct.unpack('<I', f.read(4))[0]

        print(f"GUID: {guid}")
        print(f"版本: {revision}")
        print(f"索引大小: {index_size:,} 字节")
        print(f"索引CRC: 0x{index_crc:08X}")
        print(f"文件数量: {file_count:,}")

        # === 读取加密索引 ===
        index_data = f.read(index_size)

        # 解密索引 (使用 index_crc 作为密钥)
        decrypted = bytearray(index_size)
        key = index_crc
        for i in range(index_size):
            decrypted[i] = index_data[i] ^ (key & 0xFF)
            key = ((key * 0x81) >> 7) & 0xFFFFFFFF  # 正确的密钥更新算法

        # === 解析索引条目 ===
        # 格式: hash(4) + name_len(4) + filename(N) + file_size(4) + decrypt_key(4) + file_offset(4)
        pos = 0
        files = []
        extensions = defaultdict(int)
        directories = defaultdict(int)

        for i in range(file_count):
            if pos + 8 > len(decrypted):
                print(f"警告: 索引在第 {i} 个文件处截断")
                break

            file_hash = struct.unpack('<I', decrypted[pos:pos+4])[0]
            name_len = struct.unpack('<I', decrypted[pos+4:pos+8])[0]
            pos += 8

            if name_len == 0 or name_len > 1000 or pos + name_len + 12 > len(decrypted):
                print(f"警告: 文件 {i} 名称长度异常: {name_len}")
                break

            filename = decrypted[pos:pos+name_len].decode('utf-8', errors='replace')
            pos += name_len

            file_size = struct.unpack('<I', decrypted[pos:pos+4])[0]
            decrypt_key = struct.unpack('<I', decrypted[pos+4:pos+8])[0]
            file_offset = struct.unpack('<I', decrypted[pos+8:pos+12])[0]
            pos += 12

            files.append((filename, file_hash, file_size, file_offset))

            # 统计扩展名
            ext = os.path.splitext(filename)[1].lower()
            if ext:
                extensions[ext] += 1

            # 统计顶级目录
            parts = filename.replace("\\", "/").split("/")
            if len(parts) > 1:
                directories[parts[0]] += 1

            # 进度显示
            if (i + 1) % 50000 == 0:
                print(f"  已解析 {i+1:,} / {file_count:,} ...")

        return files, extensions, directories

if __name__ == '__main__':
    files, extensions, directories = read_pvf_file_list('Script.pvf')

    print(f"\n成功解析文件数: {len(files):,}")

    print("\n=== 扩展名统计 (按数量排序) ===")
    for ext, count in sorted(extensions.items(), key=lambda x: -x[1])[:30]:
        print(f"  {ext:10s}: {count:6d} 个文件")

    print("\n=== 顶级目录统计 (按数量排序) ===")
    for dir_name, count in sorted(directories.items(), key=lambda x: -x[1])[:30]:
        print(f"  {dir_name:30s}: {count:6d} 个文件")

    # 保存完整文件列表
    with open('pvf_file_list.txt', 'w', encoding='utf-8') as out:
        out.write(f"# PVF File List\n")
        out.write(f"# Total files: {len(files)}\n\n")
        for filename, file_hash, file_size, file_offset in files:
            out.write(f"0x{file_hash:08X} | {file_size:8d} | {filename}\n")

    print(f"\n[+] 完整文件列表已保存到 pvf_file_list.txt")

    # 保存目录结构摘要
    with open('pvf_directory_structure.txt', 'w', encoding='utf-8') as out:
        out.write("# PVF Directory Structure Summary\n\n")
        out.write("## Extensions\n")
        for ext, count in sorted(extensions.items(), key=lambda x: -x[1]):
            out.write(f"{ext:10s}: {count:6d}\n")
        out.write("\n## Top-level Directories\n")
        for dir_name, count in sorted(directories.items(), key=lambda x: -x[1]):
            out.write(f"{dir_name:30s}: {count:6d}\n")

    print(f"[+] 目录结构摘要已保存到 pvf_directory_structure.txt")
