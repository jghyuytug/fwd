#!/usr/bin/env python3
"""
按时间组织二级目录文件 - 创建月/周文件夹并移动文件

功能:
1. 扫描二级目录中的所有文件
2. 按修改时间创建月/周子文件夹
3. 将文件移动到对应的时间文件夹
4. 生成移动报告

用法:
    python organize_subdirs_by_time.py [一级目录] [--dry-run]

    --dry-run: 只显示将要执行的操作，不实际移动文件
"""

import os
import sys
import shutil
from datetime import datetime
from pathlib import Path
from collections import defaultdict

def get_file_time_info(file_path):
    """获取文件的时间信息"""
    try:
        stat = os.stat(file_path)
        mtime = datetime.fromtimestamp(stat.st_mtime)

        year = mtime.year
        month = mtime.month
        week = mtime.isocalendar()[1]

        return {
            'path': file_path,
            'mtime': mtime,
            'year': year,
            'month': month,
            'week': week,
            'month_str': mtime.strftime('%Y-%m'),
            'week_str': f'{year}-W{week:02d}',
            'folder_name': f'{year}-{month:02d}-W{week:02d}'
        }
    except Exception as e:
        print(f"❌ 获取文件信息失败: {file_path} - {e}")
        return None

def organize_subdirectory(subdir_path, dry_run=False):
    """组织单个二级目录"""
    if not os.path.isdir(subdir_path):
        print(f"ERROR: Directory does not exist: {subdir_path}")
        return None

    print(f"\nProcessing directory: {subdir_path}")
    print("=" * 80)

    # 扫描文件
    files_info = []
    for file in os.listdir(subdir_path):
        file_path = os.path.join(subdir_path, file)

        # 跳过目录
        if os.path.isdir(file_path):
            print(f"  [skip] Subdirectory: {file}")
            continue

        time_info = get_file_time_info(file_path)
        if time_info:
            files_info.append(time_info)

    if not files_info:
        print("  [info] No files to process in this directory")
        return None

    # 按时间分组
    time_groups = defaultdict(list)
    for f in files_info:
        time_groups[f['folder_name']].append(f)

    # 移动文件
    move_count = 0
    for folder_name, files in sorted(time_groups.items()):
        target_folder = os.path.join(subdir_path, folder_name)

        # 创建时间文件夹
        if not os.path.exists(target_folder):
            if not dry_run:
                os.makedirs(target_folder, exist_ok=True)
            print(f"\n  [create] Time folder: {folder_name}")

        print(f"    Contains {len(files)} file(s):")

        for f in sorted(files, key=lambda x: x['mtime']):
            filename = os.path.basename(f['path'])
            time_str = f['mtime'].strftime('%Y-%m-%d %H:%M:%S')
            target_path = os.path.join(target_folder, filename)

            # 检查目标文件是否已存在
            if os.path.exists(target_path):
                print(f"      [warn] {filename} already exists (skip)")
                continue

            if not dry_run:
                try:
                    shutil.move(f['path'], target_path)
                    print(f"      [ok] {filename} -> {folder_name}/")
                    move_count += 1
                except Exception as e:
                    print(f"      [error] Move failed: {filename} - {e}")
            else:
                print(f"      [sim] {filename} -> {folder_name}/")
                move_count += 1

    print(f"\n  [summary] Moved {move_count} files to {len(time_groups)} time folders")
    return {
        'total_files': len(files_info),
        'total_folders': len(time_groups),
        'moved_files': move_count
    }

def organize_all_subdirs(parent_dir, dry_run=False):
    """组织一级目录下的所有二级目录"""
    if not os.path.isdir(parent_dir):
        print(f"❌ 目录不存在: {parent_dir}")
        return

    print(f"\n{'='*80}")
    print("Organizing subdirectories by time - Start")
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
        result = organize_subdirectory(subdir_path, dry_run)
        if result:
            results[subdir] = result

    # 生成总结报告
    print(f"\n{'='*80}")
    print("Summary Report")
    print(f"{'='*80}\n")

    total_files = sum(r['total_files'] for r in results.values())
    total_folders = sum(r['total_folders'] for r in results.values())
    total_moved = sum(r['moved_files'] for r in results.values())

    print(f"Directories processed: {len(results)}")
    print(f"Total original files: {total_files}")
    print(f"Time folders created: {total_folders}")
    print(f"Files moved: {total_moved}")

    if dry_run:
        print("\n[note] This is a dry-run. No actual file movements were performed.")
        print("Remove --dry-run parameter to perform actual file movements.")

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

    organize_all_subdirs(parent_dir, dry_run)
