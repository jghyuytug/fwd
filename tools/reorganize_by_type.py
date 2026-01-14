#!/usr/bin/env python3
"""
混合分类工具 - 按文件类型分类 + 时间标签

功能:
1. 反转时间文件夹结构回到原始状态
2. 按文件类型创建分类文件夹
3. 为文件添加时间标签（保留时间信息）
4. 生成分类报告

文件类型分类:
  - markdown/      .md 文档
  - source/        .c .cpp .cc 源代码
  - headers/       .h .hpp 头文件
  - config/        Makefile .xml .json .sh 配置
  - data/          .txt .dat .json 数据文件
  - archive/       .tar .tar.gz .zip 归档

用法:
    python reorganize_by_type.py [一级目录] [--dry-run]
"""

import os
import sys
import shutil
from datetime import datetime
from pathlib import Path
from collections import defaultdict

# 文件类型映射
FILE_TYPES = {
    'markdown': ['.md'],
    'source': ['.c', '.cpp', '.cc', '.cxx', '.c++'],
    'headers': ['.h', '.hpp', '.hxx', '.h++'],
    'config': ['Makefile', '.xml', '.json', '.sh', '.yml', '.yaml', '.conf', '.cfg'],
    'data': ['.txt', '.dat', '.csv', '.tsv', '.log'],
    'archive': ['.tar', '.gz', '.zip', '.bak', '.7z']
}

def get_file_extension(filename):
    """获取文件扩展名"""
    if filename.startswith('Makefile'):
        return 'Makefile'
    return os.path.splitext(filename)[1].lower()

def get_file_type(filename):
    """判断文件类型"""
    ext = get_file_extension(filename)

    for ftype, extensions in FILE_TYPES.items():
        if ext in extensions or filename in extensions:
            return ftype

    return None  # 未分类

def get_file_time_info(file_path):
    """获取文件时间信息"""
    try:
        stat = os.stat(file_path)
        mtime = datetime.fromtimestamp(stat.st_mtime)
        return mtime.strftime('%Y-%m-%d_%H%M%S')
    except:
        return None

def add_time_tag(filename, time_tag):
    """为文件名添加时间标签"""
    if not time_tag:
        return filename

    name, ext = os.path.splitext(filename)
    return f"{name}_[{time_tag}]{ext}"

def move_files_from_time_folders(subdir_path, dry_run=False):
    """从时间文件夹中提取文件，按类型重新分类"""
    print(f"\nProcessing: {subdir_path}")
    print("=" * 80)

    # 查找所有时间文件夹
    time_folders = []
    for item in os.listdir(subdir_path):
        item_path = os.path.join(subdir_path, item)
        if os.path.isdir(item_path) and '-W' in item and item[0:4].isdigit():
            time_folders.append((item, item_path))

    if not time_folders:
        print("  [info] No time-based folders found")
        return None

    # 创建类型分类文件夹
    type_folders = {}
    for ftype in FILE_TYPES.keys():
        type_folder = os.path.join(subdir_path, ftype)
        type_folders[ftype] = type_folder
        if not os.path.exists(type_folder):
            if not dry_run:
                os.makedirs(type_folder, exist_ok=True)
            print(f"  [create] Type folder: {ftype}/")

    # 处理时间文件夹中的文件
    moved_count = 0
    unclassified_count = 0

    for time_folder_name, time_folder_path in time_folders:
        print(f"\n  [extracting] From {time_folder_name}:")

        for filename in os.listdir(time_folder_path):
            file_path = os.path.join(time_folder_path, filename)

            if os.path.isdir(file_path):
                print(f"    [skip] Directory: {filename}")
                continue

            # 获取文件时间
            time_tag = get_file_time_info(file_path)

            # 判断文件类型
            file_type = get_file_type(filename)

            if not file_type:
                print(f"    [unclass] {filename}")
                unclassified_count += 1
                continue

            # 新文件名（带时间标签）
            new_filename = add_time_tag(filename, time_tag)
            target_path = os.path.join(type_folders[file_type], new_filename)

            if os.path.exists(target_path):
                print(f"    [warn] {new_filename} already exists")
                continue

            if not dry_run:
                try:
                    shutil.move(file_path, target_path)
                    print(f"    [ok] {filename} -> {file_type}/ (tagged: {time_tag})")
                    moved_count += 1
                except Exception as e:
                    print(f"    [error] Failed to move {filename}: {e}")
            else:
                print(f"    [sim] {filename} -> {file_type}/ (tagged: {time_tag})")
                moved_count += 1

    # 删除空的时间文件夹
    for time_folder_name, time_folder_path in time_folders:
        if not os.listdir(time_folder_path):
            if not dry_run:
                try:
                    os.rmdir(time_folder_path)
                    print(f"\n  [remove] Empty folder: {time_folder_name}/")
                except:
                    pass

    print(f"\n  [summary] Moved {moved_count} files, {unclassified_count} unclassified")

    return {
        'moved': moved_count,
        'unclassified': unclassified_count,
        'type_folders': len(type_folders)
    }

def reorganize_directory(parent_dir, dry_run=False):
    """重新组织一级目录下的所有二级目录"""
    if not os.path.isdir(parent_dir):
        print(f"ERROR: Directory does not exist: {parent_dir}")
        return

    print(f"\n{'='*80}")
    print("Reorganizing by Type Classification")
    print(f"{'='*80}")
    print(f"Parent directory: {parent_dir}")
    print(f"Mode: {'Dry-run (--dry-run)' if dry_run else 'Actual move'}\n")

    # 获取所有二级目录
    subdirs = [d for d in os.listdir(parent_dir)
               if os.path.isdir(os.path.join(parent_dir, d)) and not d.startswith('.')]

    if not subdirs:
        print("No subdirectories found")
        return

    print(f"Found {len(subdirs)} subdirectories:")
    for d in sorted(subdirs):
        print(f"  - {d}")

    # 处理每个二级目录
    results = {}
    for subdir in sorted(subdirs):
        subdir_path = os.path.join(parent_dir, subdir)
        result = move_files_from_time_folders(subdir_path, dry_run)
        if result:
            results[subdir] = result

    # 生成报告
    print(f"\n{'='*80}")
    print("Summary Report")
    print(f"{'='*80}\n")

    total_moved = sum(r['moved'] for r in results.values())
    total_unclass = sum(r['unclassified'] for r in results.values())

    print(f"Directories processed: {len(results)}")
    print(f"Files moved: {total_moved}")
    print(f"Unclassified files: {total_unclass}")

    if dry_run:
        print("\n[note] This is a dry-run. No actual operations were performed.")
        print("Remove --dry-run parameter to perform actual reorganization.")

    print(f"\n[ok] Processing complete!\n")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:")
        print(f"  python {sys.argv[0]} <parent_dir> [--dry-run]")
        print("\nExample:")
        print(f"  python {sys.argv[0]} docs --dry-run")
        print(f"  python {sys.argv[0]} docs")
        sys.exit(1)

    parent_dir = sys.argv[1]
    dry_run = '--dry-run' in sys.argv

    reorganize_directory(parent_dir, dry_run)
