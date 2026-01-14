#!/usr/bin/env python3
"""
按时间分类工具 - 将子目录文件按月周分类并添加时间标记

功能:
1. 扫描子目录中的所有文件
2. 按修改时间分类 (月/周)
3. 为每个文件添加时间标记
4. 生成分类报告

用法:
    python organize_by_time.py [目录路径]
"""

import os
import sys
import json
from datetime import datetime
from pathlib import Path
from collections import defaultdict

def get_file_time_info(file_path):
    """获取文件的时间信息"""
    try:
        stat = os.stat(file_path)
        mtime = datetime.fromtimestamp(stat.st_mtime)

        # 计算月份和周数
        year = mtime.year
        month = mtime.month
        week = mtime.isocalendar()[1]  # ISO周数

        return {
            'path': file_path,
            'mtime': mtime,
            'datetime_str': mtime.strftime('%Y-%m-%d %H:%M:%S'),
            'year': year,
            'month': month,
            'week': week,
            'month_str': mtime.strftime('%Y年%m月'),
            'week_str': f'{year}年W{week}周',
            'size': stat.st_size,
            'size_kb': round(stat.st_size / 1024, 2)
        }
    except Exception as e:
        return None

def categorize_files(directory):
    """按月周分类文件"""
    categories = defaultdict(lambda: defaultdict(list))
    all_files = []

    # 遍历目录
    for root, dirs, files in os.walk(directory):
        # 跳过某些目录
        dirs[:] = [d for d in dirs if not d.startswith('.')]

        for file in files:
            file_path = os.path.join(root, file)
            time_info = get_file_time_info(file_path)

            if time_info:
                all_files.append(time_info)
                # 按月分类
                categories[time_info['month_str']][time_info['week_str']].append(time_info)

    return categories, all_files

def generate_time_marker(time_info):
    """生成文件的时间标记"""
    return f"[{time_info['month_str']} W{time_info['week']}周 {time_info['datetime_str']}]"

def print_categorized_report(categories):
    """打印分类报告"""
    print("\n" + "="*80)
    print("文件按时间分类报告")
    print("="*80 + "\n")

    for month in sorted(categories.keys()):
        print(f"\n📅 {month}")
        print("-" * 80)

        weeks = categories[month]
        for week in sorted(weeks.keys()):
            files = weeks[week]
            total_size = sum(f['size'] for f in files)
            total_size_kb = round(total_size / 1024, 2)

            print(f"\n  📦 {week} ({len(files)} 文件, {total_size_kb} KB)")

            for f in sorted(files, key=lambda x: x['mtime']):
                rel_path = f['path'].replace('\\', '/')
                print(f"    ✓ {rel_path:<60} ({f['size_kb']:>8} KB)")

def create_time_index(all_files, output_file):
    """创建时间索引文件"""
    categories = defaultdict(list)

    for f in sorted(all_files, key=lambda x: x['mtime'], reverse=True):
        key = f['month_str']
        categories[key].append({
            'name': os.path.basename(f['path']),
            'path': f['path'].replace('\\', '/'),
            'time': f['datetime_str'],
            'week': f['week'],
            'size_kb': f['size_kb']
        })

    # 生成Markdown索引
    content = []
    content.append("---")
    content.append("**Document Type**: Time-Based File Index")
    content.append(f"**Generated**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    content.append("**Status**: Complete")
    content.append("---\n")
    content.append("# 📅 子目录文件时间分类索引\n")
    content.append("## 按月周分类的所有文件\n")

    for month in sorted(categories.keys()):
        content.append(f"\n### {month}\n")

        # 按周分组
        weeks = defaultdict(list)
        for f in categories[month]:
            weeks[f['week']].append(f)

        for week in sorted(weeks.keys()):
            files = weeks[week]
            content.append(f"\n**W{week}周** ({len(files)} 文件)\n")
            content.append("| 文件名 | 路径 | 时间 | 大小 |")
            content.append("|--------|------|------|------|")

            for f in sorted(files, key=lambda x: x['time']):
                short_path = f['path'][:50] + "..." if len(f['path']) > 50 else f['path']
                content.append(f"| {os.path.basename(f['path'])} | `{short_path}` | {f['time']} | {f['size_kb']} KB |")

    # 保存文件
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(content))

    print(f"\n✅ 索引文件已生成: {output_file}")

def generate_time_marker_batch(directory, output_file=None):
    """批量生成时间标记"""
    print(f"\n扫描目录: {directory}")
    print("生成文件时间标记...\n")

    categories, all_files = categorize_files(directory)

    # 打印报告
    print_categorized_report(categories)

    # 生成统计信息
    print(f"\n{'='*80}")
    print("📊 统计信息")
    print(f"{'='*80}")
    print(f"总文件数: {len(all_files)}")

    total_size = sum(f['size'] for f in all_files)
    print(f"总大小: {round(total_size / 1024 / 1024, 2)} MB")

    month_count = len(categories)
    print(f"包含月份: {month_count}")

    # 生成时间索引
    if output_file is None:
        output_file = os.path.join(directory, '..', 'TIME_INDEX.md')

    create_time_index(all_files, output_file)

    # 生成JSON数据
    json_file = output_file.replace('.md', '.json')
    with open(json_file, 'w', encoding='utf-8') as f:
        json_data = {
            'total_files': len(all_files),
            'total_size_mb': round(total_size / 1024 / 1024, 2),
            'months': len(categories),
            'files': all_files
        }
        json.dump(json_data, f, indent=2, default=str, ensure_ascii=False)

    print(f"✅ JSON数据已生成: {json_file}")

    return all_files, categories

if __name__ == '__main__':
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    else:
        directory = '.'

    if not os.path.isdir(directory):
        print(f"❌ 错误: {directory} 不是有效的目录")
        sys.exit(1)

    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    generate_time_marker_batch(directory, output_file)
