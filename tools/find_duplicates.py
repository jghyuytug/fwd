#!/usr/bin/env python3
import os
import hashlib
from collections import defaultdict
import glob
from datetime import datetime

def get_file_hash(filepath, chunk_size=8192):
    """计算文件哈希值"""
    hash_md5 = hashlib.md5()
    try:
        with open(filepath, 'rb') as f:
            for chunk in iter(lambda: f.read(chunk_size), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()
    except Exception as e:
        return None

def get_file_info(filepath):
    """获取文件信息"""
    try:
        stat = os.stat(filepath)
        return {
            'path': filepath,
            'size': stat.st_size,
            'mtime': stat.st_mtime,
            'basename': os.path.basename(filepath)
        }
    except:
        return None

# 扫描所有源文件
base_dir = r"C:\Users\waw\Desktop\game"
files = []

for pattern in ['**/*.h', '**/*.c', '**/*.cpp']:
    for filepath in glob.glob(os.path.join(base_dir, pattern), recursive=True):
        info = get_file_info(filepath)
        if info:
            files.append(info)

print(f"[*] 总共找到 {len(files)} 个源文件")

# 按文件名分组
by_basename = defaultdict(list)
for file in files:
    by_basename[file['basename']].append(file)

# 按大小分组
by_size = defaultdict(list)
for file in files:
    by_size[file['size']].append(file)

# 查找同名文件
same_name_duplicates = {k: v for k, v in by_basename.items() if len(v) > 1}
print(f"[+] 找到 {len(same_name_duplicates)} 个有相同名称的文件组")

# 查找同大小文件
same_size_duplicates = {k: v for k, v in by_size.items() if len(v) > 1}
print(f"[+] 找到 {len(same_size_duplicates)} 个有相同大小的文件组")

# 计算所有文件哈希并分组
print("[*] 正在计算文件哈希值...")
hash_to_files = defaultdict(list)
for i, file in enumerate(files):
    file_hash = get_file_hash(file['path'])
    if file_hash:
        file['hash'] = file_hash
        hash_to_files[file_hash].append(file)
    if (i + 1) % 50 == 0:
        print(f"    已处理 {i+1}/{len(files)} 文件")

# 找真正的重复（内容相同）
real_duplicates = {h: v for h, v in hash_to_files.items() if len(v) > 1}
print(f"[+] 找到 {len(real_duplicates)} 个内容相同的文件组\n")

# 输出结果
print("="*100)
print("【重复文件分析报告】")
print("="*100)

print("\n" + "="*100)
print("1. 内容相同的重复文件（强重复 - 完全相同的内容）")
print("="*100)

total_duplicate_size = 0
duplicate_count = 0

for hash_val, file_group in sorted(real_duplicates.items()):
    print(f"\n├─ 哈希值: {hash_val}")
    print(f"├─ 数量: {len(file_group)} 个文件")
    print(f"├─ 文件大小: {file_group[0]['size']} 字节")
    print(f"├─ 冗余空间: {file_group[0]['size'] * (len(file_group) - 1)} 字节 ({(file_group[0]['size'] * (len(file_group) - 1)) / 1024:.1f} KB)")
    total_duplicate_size += file_group[0]['size'] * (len(file_group) - 1)
    duplicate_count += len(file_group) - 1
    print(f"└─ 文件列表:")
    for idx, f in enumerate(sorted(file_group, key=lambda x: x['path']), 1):
        mtime = datetime.fromtimestamp(f['mtime']).strftime('%Y-%m-%d %H:%M:%S')
        marker = "  ◆ " if idx == 1 else "  ◇ "
        print(f"   {marker}[{idx}] {f['path']}")
        print(f"        └─ 修改: {mtime}")

print("\n" + "-"*100)
print(f"总结: 发现 {len(real_duplicates)} 个文件组，共 {duplicate_count} 个冗余文件")
print(f"可清理空间: {total_duplicate_size / 1024:.1f} KB ({total_duplicate_size / (1024*1024):.2f} MB)")

# 输出同名但可能内容不同的文件
print("\n" + "="*100)
print("2. 同名但内容不同的文件（弱重复 - 名称相同但内容不同）")
print("="*100)

weak_duplicates_found = False
for basename, file_group in sorted(same_name_duplicates.items()):
    if len(file_group) > 1:
        hashes = set(f.get('hash', 'N/A') for f in file_group)
        if len(hashes) > 1 or 'N/A' in hashes:
            weak_duplicates_found = True
            print(f"\n├─ 文件名: {basename}")
            print(f"├─ 实例数: {len(file_group)}")
            print(f"└─ 详细信息:")
            for idx, f in enumerate(sorted(file_group, key=lambda x: x['path']), 1):
                mtime = datetime.fromtimestamp(f['mtime']).strftime('%Y-%m-%d %H:%M:%S')
                hash_str = f.get('hash', 'N/A')[:8] if 'hash' in f else 'N/A'
                print(f"   [{idx}] {f['path']}")
                print(f"       └─ 大小: {f['size']:>10} 字节 | 哈希: {hash_str}... | 修改: {mtime}")

if not weak_duplicates_found:
    print("\n   (无同名但内容不同的文件)")

print("\n" + "="*100)

