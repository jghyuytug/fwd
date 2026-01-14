#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""测试重命名逻辑"""

import re

# 测试数据
test_line = "int sub_807D720();"
pattern = r'\bsub_([0-9A-Fa-f]{7,8})\b'

# 测试映射
mapping = {
    '807D708': '_init',
    '0807D708': '_init',
    '807D720': 'test_function',
    '0807D720': 'test_function',
}

print("测试行:", test_line)
print("正则模式:", pattern)
print()

# 查找匹配
matches = re.findall(pattern, test_line)
print(f"找到 {len(matches)} 个匹配:")
for m in matches:
    print(f"  - {m}")
print()

# 测试替换逻辑
def test_replace(match):
    addr_raw = match.group(1).upper()
    original = match.group(0)

    print(f"处理匹配: {original}")
    print(f"  地址原始: {addr_raw}")

    # 尝试查找
    for addr_variant in [addr_raw, addr_raw.lstrip('0'), addr_raw.zfill(8)]:
        print(f"  尝试查找: {addr_variant} -> ", end='')
        if addr_variant in mapping:
            print(f"找到! {mapping[addr_variant]}")
            return f"{original} /* {mapping[addr_variant]} */"
        else:
            print("未找到")

    return original

result = re.sub(pattern, test_replace, test_line)
print()
print("替换结果:", result)
