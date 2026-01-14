#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enhancement System Configuration Parser
解析DNF装备强化/增幅系统配置文件

Based on extracted Script.pvf files:
- etc/upgrade_separate.etc (2427 bytes) - 强化系统配置
- etc/amplifyitem.etc (657 bytes) - 增幅系统配置
"""

import struct
import sys

def parse_upgrade_config(filename):
    """
    解析upgrade_separate.etc配置文件

    尝试识别强化等级、成功率、成本等参数
    """
    print(f"=== 解析强化配置文件: {filename} ===\n")

    with open(filename, 'rb') as f:
        data = f.read()

    print(f"文件大小: {len(data)} 字节\n")

    # 尝试查找浮点数（成功率）
    print("检测到的浮点数（可能是成功率）:")
    floats = []
    for i in range(0, len(data) - 3, 1):
        try:
            # 读取4字节浮点数（小端序）
            value = struct.unpack('<f', data[i:i+4])[0]
            # 筛选可能的成功率（0.0-1.0之间）
            if 0.0 <= value <= 1.0:
                floats.append((i, value))
        except:
            pass

    # 去重并排序
    unique_floats = {}
    for offset, value in floats:
        key = round(value, 6)
        if key not in unique_floats:
            unique_floats[key] = offset

    sorted_floats = sorted(unique_floats.items())

    for value, offset in sorted_floats[:30]:  # 显示前30个
        percentage = value * 100
        print(f"  偏移 0x{offset:04x}: {value:.6f} ({percentage:.2f}%)")

    print(f"\n共检测到 {len(sorted_floats)} 个不同的浮点数值\n")

    # 尝试查找整数（强化等级、成本等）
    print("检测到的小整数（可能是等级、数量）:")
    integers = []
    for i in range(0, len(data) - 3, 4):
        try:
            value = struct.unpack('<I', data[i:i+4])[0]
            # 筛选小整数（1-100范围）
            if 1 <= value <= 100:
                integers.append((i, value))
        except:
            pass

    # 统计频率
    int_freq = {}
    for offset, value in integers:
        if value not in int_freq:
            int_freq[value] = []
        int_freq[value].append(offset)

    # 显示出现次数最多的整数
    sorted_ints = sorted(int_freq.items(), key=lambda x: len(x[1]), reverse=True)

    for value, offsets in sorted_ints[:20]:
        print(f"  值 {value:3d}: 出现 {len(offsets):2d} 次 (首次偏移: 0x{offsets[0]:04x})")

    # 尝试查找强化等级区间模式
    print("\n\n=== 尝试识别强化等级配置 ===")
    print("基于浮点数分布猜测的强化成功率区间：\n")

    # 常见的强化等级
    levels = [0, 3, 6, 9, 10, 11, 12, 13, 15, 20]

    # 根据浮点数分布猜测成功率
    if len(sorted_floats) > 0:
        print("可能的成功率配置：")
        for i, (value, offset) in enumerate(sorted_floats[:15]):
            percentage = value * 100
            print(f"  等级区间 #{i}: {percentage:.2f}% 成功率")

def parse_amplify_config(filename):
    """
    解析amplifyitem.etc配置文件

    尝试识别增幅等级、属性数量、属性范围等参数
    """
    print(f"\n\n=== 解析增幅配置文件: {filename} ===\n")

    with open(filename, 'rb') as f:
        data = f.read()

    print(f"文件大小: {len(data)} 字节\n")

    # 查找浮点数
    print("检测到的浮点数（可能是增幅倍率）:")
    floats = []
    for i in range(0, len(data) - 3, 1):
        try:
            value = struct.unpack('<f', data[i:i+4])[0]
            if 0.0 <= value <= 10.0:  # 增幅倍率可能更大
                floats.append((i, value))
        except:
            pass

    unique_floats = {}
    for offset, value in floats:
        key = round(value, 6)
        if key not in unique_floats:
            unique_floats[key] = offset

    sorted_floats = sorted(unique_floats.items())

    for value, offset in sorted_floats[:20]:
        print(f"  偏移 0x{offset:04x}: {value:.6f}")

    # 查找整数
    print("\n检测到的小整数（可能是属性数量、属性范围）:")
    integers = []
    for i in range(0, len(data) - 3, 4):
        try:
            value = struct.unpack('<I', data[i:i+4])[0]
            if 1 <= value <= 1000:
                integers.append((i, value))
        except:
            pass

    int_freq = {}
    for offset, value in integers:
        if value not in int_freq:
            int_freq[value] = []
        int_freq[value].append(offset)

    sorted_ints = sorted(int_freq.items(), key=lambda x: len(x[1]), reverse=True)

    for value, offsets in sorted_ints[:20]:
        print(f"  值 {value:4d}: 出现 {len(offsets):2d} 次")

def parse_stackable(filename):
    """
    解析.stk文件（强化道具）
    """
    print(f"\n\n=== 解析道具文件: {filename} ===\n")

    with open(filename, 'rb') as f:
        data = f.read()

    print(f"文件大小: {len(data)} 字节\n")

    # 尝试以文本方式读取（某些stk文件是文本格式）
    try:
        text = data.decode('utf-8', errors='ignore')
        if len(text.strip()) > 0 and text.isprintable():
            print("文件内容（文本格式）:")
            print(text[:500])
        else:
            print("文件为二进制格式，显示前200字节（hex）:")
            for i in range(min(200, len(data))):
                print(f'{data[i]:02x}', end=' ')
                if (i+1) % 16 == 0:
                    print()
    except:
        print("无法解码为文本")

if __name__ == '__main__':
    parse_upgrade_config('extracted_upgrade_separate.etc')
    parse_amplify_config('extracted_amplifyitem.etc')
    parse_stackable('extracted_itemupgrade_low.stk')
