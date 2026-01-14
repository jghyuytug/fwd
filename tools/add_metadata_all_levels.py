#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Add unified metadata headers to all documentation files (including 3rd level)
Processes all markdown files in docs/ directory tree
"""

import os
import re
from datetime import datetime

def get_metadata_header(file_path, folder_name):
    """Generate unified metadata header"""

    category_map = {
        'analysis': 'Analysis Report',
        'phases': 'Phase Report',
        'guides': 'Guide Document',
        'compilation': 'Compilation Document',
        'sessions': 'Session Report',
        'reports': 'Work Report',
        'summaries': 'Summary Document',
        'cleanup_reports': 'Cleanup Report',
        'archive': 'Archive Document',
        'design': 'Design Document',
        'generated': 'Generated Report',
        'session_vii': 'Session VII Report'
    }

    category = category_map.get(folder_name, 'Project Document')

    header = """---
**Document Type**: {}
**Generated**: 2025-11-05
**Updated**: 2025-11-05
**Version**: v1.0
**Status**: Complete
---

""".format(category)
    return header

def has_metadata_header(content):
    """Check if document already has metadata header"""
    if content.startswith('---'):
        if '**Document Type**' in content or '**生成时间**' in content or '**Generated**' in content:
            return True
    return False

def add_metadata_to_file(file_path):
    """Add metadata header to a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Skip if already has metadata header
        if has_metadata_header(content):
            return False, "already has header"

        # Get folder name
        folder_name = os.path.basename(os.path.dirname(file_path))

        # Generate metadata header
        metadata_header = get_metadata_header(file_path, folder_name)

        # Remove old metadata if exists
        if content.startswith('---'):
            first_close = content.find('---', 3)
            if first_close != -1:
                content = content[first_close + 3:].lstrip('\n')

        # Combine new content
        new_content = metadata_header + content

        # Write back to file
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)

        return True, "added"

    except Exception as e:
        return False, str(e)

def main():
    """Main function: process all markdown files in docs/ directory"""

    docs_dir = r"C:\Users\waw\Desktop\game\docs"

    if not os.path.exists(docs_dir):
        print("Error: docs directory not found")
        return

    stats = {
        'total': 0,
        'updated': 0,
        'skipped': 0,
        'failed': 0
    }

    # Walk through all directories under docs/
    for root, dirs, files in os.walk(docs_dir):
        for file in files:
            if file.endswith('.md'):
                file_path = os.path.join(root, file)
                stats['total'] += 1

                success, message = add_metadata_to_file(file_path)

                if success:
                    stats['updated'] += 1
                elif "already has header" in message:
                    stats['skipped'] += 1
                else:
                    stats['failed'] += 1

    # Print statistics
    print("\n" + "="*60)
    print("Metadata Header Addition - Results:")
    print("  Total files: {}".format(stats['total']))
    print("  Updated: {}".format(stats['updated']))
    print("  Skipped: {}".format(stats['skipped']))
    print("  Failed: {}".format(stats['failed']))
    print("="*60)

if __name__ == '__main__':
    main()
