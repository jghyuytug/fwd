#!/usr/bin/env python3
"""
compare_error_logs.py - Compare two compilation error logs
Identifies new errors, eliminated errors, and changes in error patterns
"""

import sys
import re
from collections import Counter, defaultdict

def extract_errors(log_file):
    """Extract all error messages from log file"""
    errors = []
    error_pattern = re.compile(r'error:\s*(.+)$')

    try:
        with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                match = error_pattern.search(line)
                if match:
                    error_msg = match.group(1).strip()
                    # Normalize error message (remove specific identifiers)
                    error_msg = re.sub(r"'[^']*'", "'X'", error_msg)
                    error_msg = re.sub(r'"[^"]*"', '"X"', error_msg)
                    error_msg = re.sub(r'\b\d+\b', 'N', error_msg)
                    errors.append(error_msg)
    except FileNotFoundError:
        print(f"Error: File not found: {log_file}")
        sys.exit(1)

    return errors

def categorize_error(error_msg):
    """Categorize error message into type"""
    error_msg_lower = error_msg.lower()

    if 'template' in error_msg_lower and 'argument' in error_msg_lower:
        return 'template_argument'
    elif 'not declared' in error_msg_lower or 'was not declared' in error_msg_lower:
        return 'not_declared'
    elif 'not a member' in error_msg_lower or 'no member named' in error_msg_lower:
        return 'not_member'
    elif 'incomplete type' in error_msg_lower or 'invalid type' in error_msg_lower:
        return 'type_error'
    elif 'redefinition' in error_msg_lower or 'conflicting' in error_msg_lower:
        return 'redefinition'
    elif 'namespace' in error_msg_lower:
        return 'namespace'
    elif 'no matching' in error_msg_lower:
        return 'no_match'
    elif 'cannot convert' in error_msg_lower or 'invalid conversion' in error_msg_lower:
        return 'conversion'
    elif 'expected' in error_msg_lower and 'before' in error_msg_lower:
        return 'syntax'
    else:
        return 'other'

def print_colored(text, color):
    """Print colored text"""
    colors = {
        'red': '\033[0;31m',
        'green': '\033[0;32m',
        'yellow': '\033[1;33m',
        'blue': '\033[0;34m',
        'cyan': '\033[0;36m',
        'bold': '\033[1m',
        'reset': '\033[0m'
    }
    print(f"{colors.get(color, '')}{text}{colors['reset']}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 compare_error_logs.py <old_log> <new_log>")
        sys.exit(1)

    old_log = sys.argv[1]
    new_log = sys.argv[2]

    print_colored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", 'bold')
    print_colored("错误日志对比分析", 'bold')
    print_colored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", 'bold')
    print()

    print("📂 提取错误信息...")
    old_errors = extract_errors(old_log)
    new_errors = extract_errors(new_log)

    print(f"   旧日志: {len(old_errors):,} 个错误")
    print(f"   新日志: {len(new_errors):,} 个错误")
    print()

    # Count unique error patterns
    old_counter = Counter(old_errors)
    new_counter = Counter(new_errors)

    old_unique = set(old_errors)
    new_unique = set(new_errors)

    # Calculate changes
    total_change = len(new_errors) - len(old_errors)
    change_percent = (total_change / len(old_errors) * 100) if len(old_errors) > 0 else 0

    print_colored("📊 总体变化", 'cyan')
    print(f"   错误总数: {len(old_errors):,} → {len(new_errors):,}")

    if total_change < 0:
        print_colored(f"   变化量: {total_change:,} ({change_percent:.1f}%) ⬇️", 'green')
    elif total_change > 0:
        print_colored(f"   变化量: +{total_change:,} ({change_percent:.1f}%) ⬆️", 'red')
    else:
        print_colored(f"   变化量: 0 (0.0%) ━", 'yellow')
    print()

    # New error patterns
    new_patterns = new_unique - old_unique
    print_colored(f"🆕 新出现的错误模式 ({len(new_patterns)} 种)", 'yellow')

    if new_patterns:
        # Count occurrences of new patterns
        new_pattern_counts = []
        for pattern in new_patterns:
            count = new_counter[pattern]
            new_pattern_counts.append((count, pattern))

        new_pattern_counts.sort(reverse=True)

        for i, (count, pattern) in enumerate(new_pattern_counts[:10], 1):
            print(f"   {i}. ({count:,}次) {pattern[:80]}")

        if len(new_patterns) > 10:
            print(f"   ... 还有 {len(new_patterns) - 10} 种新错误模式")
    else:
        print_colored("   ✓ 无新增错误模式", 'green')
    print()

    # Eliminated error patterns
    eliminated_patterns = old_unique - new_unique
    print_colored(f"✅ 消除的错误模式 ({len(eliminated_patterns)} 种)", 'green')

    if eliminated_patterns:
        eliminated_pattern_counts = []
        for pattern in eliminated_patterns:
            count = old_counter[pattern]
            eliminated_pattern_counts.append((count, pattern))

        eliminated_pattern_counts.sort(reverse=True)

        for i, (count, pattern) in enumerate(eliminated_pattern_counts[:10], 1):
            print(f"   {i}. ({count:,}次) {pattern[:80]}")

        if len(eliminated_patterns) > 10:
            print(f"   ... 还有 {len(eliminated_patterns) - 10} 种已消除")
    else:
        print_colored("   ⚠ 未消除任何错误模式", 'yellow')
    print()

    # Error type distribution
    print_colored("📈 错误类型分布变化", 'cyan')

    old_by_type = defaultdict(int)
    new_by_type = defaultdict(int)

    for error in old_errors:
        error_type = categorize_error(error)
        old_by_type[error_type] += 1

    for error in new_errors:
        error_type = categorize_error(error)
        new_by_type[error_type] += 1

    type_names = {
        'template_argument': '模板参数错误',
        'not_declared': '未声明错误',
        'not_member': '非成员错误',
        'type_error': '类型错误',
        'redefinition': '重定义错误',
        'namespace': '命名空间错误',
        'no_match': '函数匹配错误',
        'conversion': '类型转换错误',
        'syntax': '语法错误',
        'other': '其他错误'
    }

    all_types = sorted(set(list(old_by_type.keys()) + list(new_by_type.keys())))

    for error_type in all_types:
        old_count = old_by_type.get(error_type, 0)
        new_count = new_by_type.get(error_type, 0)
        change = new_count - old_count

        type_name = type_names.get(error_type, error_type)

        if change < 0:
            arrow = "⬇️"
            color = 'green'
        elif change > 0:
            arrow = "⬆️"
            color = 'red'
        else:
            arrow = "━"
            color = 'yellow'

        print(f"   {type_name:20s} {old_count:>8,} → {new_count:>8,} ({change:+,}) {arrow}")

    print()

    # Persistent errors (top patterns in both logs)
    persistent = old_unique & new_unique
    print_colored(f"🔄 持续存在的错误模式 ({len(persistent)} 种)", 'blue')

    if persistent:
        # Find patterns with significant counts in both
        persistent_counts = []
        for pattern in persistent:
            old_count = old_counter[pattern]
            new_count = new_counter[pattern]
            change = new_count - old_count
            persistent_counts.append((new_count, change, pattern))

        persistent_counts.sort(reverse=True)

        for i, (new_count, change, pattern) in enumerate(persistent_counts[:10], 1):
            change_str = f"{change:+,}" if change != 0 else "±0"
            print(f"   {i}. ({new_count:,}次, {change_str}) {pattern[:70]}")

        if len(persistent) > 10:
            print(f"   ... 还有 {len(persistent) - 10} 种持续错误")
    print()

    print_colored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", 'bold')

if __name__ == '__main__':
    main()
