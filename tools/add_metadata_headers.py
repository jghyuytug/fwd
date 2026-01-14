#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Add unified metadata headers to all documentation files
Adds consistent metadata to markdown files in docs/ subdirectories
"""

import os
import re
from datetime import datetime
from pathlib import Path

def get_metadata_header(file_path, folder_name):
    """生成统一的元数据头"""

    # 根据文件夹确定类别
    category_map = {
        'analysis': '分析报告',
        'phases': '阶段报告',
        'guides': '指南文档',
        'compilation': '编译文档',
        'sessions': '会话报告',
        'reports': '工作报告',
        'summaries': '总结文档',
        'cleanup_reports': '清理报告',
        'archive': '存档文档',
        'design': '设计文档',
        'generated': '生成的报告'
    }

    category = category_map.get(folder_name, '项目文档')

    header = f"""---
**文档类型**: {category}
**生成时间**: 2025-11-05
**更新时间**: 2025-11-05
**版本**: v1.0
**状态**: ✅ 完成
---

"""
    return header

def has_metadata_header(content):
    """检查文档是否已有元数据头"""
    if content.startswith('---'):
        # 检查是否是新格式的元数据头
        if '**文档类型**' in content or '**生成时间**' in content:
            return True
    return False

def add_metadata_to_file(file_path):
    """为单个文件添加元数据头"""
    try:
        # 读取文件
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # 如果已有元数据头，跳过
        if has_metadata_header(content):
            return False, "已有元数据头"

        # 获取文件夹名称
        folder_name = os.path.basename(os.path.dirname(file_path))

        # 生成元数据头
        metadata_header = get_metadata_header(file_path, folder_name)

        # 检查原始内容的开头
        # 如果已有其他的---分隔符，先删除
        if content.startswith('---'):
            # 找到第一个---后的下一个---
            first_close = content.find('---', 3)
            if first_close != -1:
                content = content[first_close + 3:].lstrip('\n')

        # 组合新内容
        new_content = metadata_header + content

        # 写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)

        return True, "添加成功"

    except Exception as e:
        return False, str(e)

def main():
    """主函数：遍历所有二级文件夹中的markdown文件"""

    docs_dir = r"C:\Users\waw\Desktop\game\docs"

    if not os.path.exists(docs_dir):
        print(f"错误: 文档目录不存在: {docs_dir}")
        return

    # 统计信息
    total_files = 0
    updated_files = 0
    skipped_files = 0
    failed_files = 0

    # 遍历所有二级文件夹
    for root, dirs, files in os.walk(docs_dir):
        # 仅处理二级目录（相对于docs的深度为1）
        depth = root.replace(docs_dir, '').count(os.sep)

        if depth > 1:
            # 跳过超过二级的目录
            continue

        if depth < 1:
            # 跳过一级目录（直接在docs下）
            continue

        # 遍历该目录中的所有markdown文件
        for file in files:
            if file.endswith('.md'):
                file_path = os.path.join(root, file)
                total_files += 1

                success, message = add_metadata_to_file(file_path)

                if success:
                    updated_files += 1
                    print("[+] {}: {}".format(file, message))
                else:
                    if "已有元数据头" in message:
                        skipped_files += 1
                        print("[-] {}: {}".format(file, message))
                    else:
                        failed_files += 1
                        print("[!] {}: {}".format(file, message))

    # 打印统计
    print("\n" + "="*60)
    print("处理结果统计:")
    print(f"  总计文件数: {total_files}")
    print(f"  已更新: {updated_files}")
    print(f"  已跳过: {skipped_files}")
    print(f"  失败: {failed_files}")
    print("="*60)

if __name__ == '__main__':
    main()
