#!/usr/bin/env python3
"""Analyze error cascade relationships from compilation log"""
import re
from collections import defaultdict

# Read log file
with open('compile_after_syntax_fix_20251029_094736.log', 'r', encoding='utf-8', errors='ignore') as f:
    lines = f.readlines()

# Track errors by line number
error_pattern = re.compile(r'df_game_r_decl\.h:(\d+):')
errors_by_line = defaultdict(list)
current_line = None

for line in lines:
    match = error_pattern.search(line)
    if match:
        current_line = int(match.group(1))
    if 'error:' in line and current_line:
        errors_by_line[current_line].append(line.strip())

# Identify high-impact lines (lines with many cascade errors)
high_impact = {line: errs for line, errs in errors_by_line.items() if len(errs) >= 5}

print(f"Total unique error source lines: {len(errors_by_line)}")
print(f"High-impact lines (5+ errors): {len(high_impact)}")
print(f"\nTop 20 lines by error count:")
sorted_lines = sorted(errors_by_line.items(), key=lambda x: len(x[1]), reverse=True)[:20]
for line_num, errs in sorted_lines:
    print(f"  Line {line_num}: {len(errs)} errors")

# Calculate cascade multiplier
total_errors = sum(len(errs) for errs in errors_by_line.values())
avg_cascade = total_errors / len(errors_by_line) if errors_by_line else 0
print(f"\nAverage cascade multiplier: {avg_cascade:.2f} errors per source line")
