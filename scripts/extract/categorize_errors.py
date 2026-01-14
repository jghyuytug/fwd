#!/usr/bin/env python3
"""Categorize all errors by type"""
import re
from collections import Counter

with open('compile_after_syntax_fix_20251029_094736.log', 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Extract all error messages
error_pattern = re.compile(r"error: (.+?)$", re.MULTILINE)
errors = error_pattern.findall(content)

print(f"Total error instances: {len(errors)}")
print(f"\nError categories:")

# Category patterns
categories = {
    'template_args': r'(wrong number of template arguments|template argument \d+ is invalid|provided for .template)',
    'not_declared': r'(was not declared|has not been declared)',
    'not_member': r'is not a member of',
    'invalid_type': r'(does not name a type|incomplete type)',
    'syntax': r'(expected .* before|parse error|invalid use)',
    'stl_select1st': r'_Select1st',
    'namespace': r"('ARAD'|'WongWork'|'QuickParty'|'nexon')",
    'other': r'.*'
}

categorized = {cat: 0 for cat in categories}
for error in errors:
    for cat, pattern in categories.items():
        if re.search(pattern, error):
            categorized[cat] += 1
            break

for cat, count in sorted(categorized.items(), key=lambda x: x[1], reverse=True):
    pct = (count / len(errors) * 100) if errors else 0
    print(f"  {cat:20s}: {count:7d} ({pct:5.1f}%)")
