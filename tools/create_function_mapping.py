#!/usr/bin/env python3
"""
从符号表创建函数名映射
用于还原反编译代码中的函数名

输入: symbols/df_game_r_symbols_full.txt
输出: symbols/function_name_mapping.txt
"""

import re
from pathlib import Path

def parse_symbols(symbol_file):
    """
    解析nm输出的符号表
    返回: {地址: 函数名}
    """
    mapping = {}

    print(f"[*] 读取符号表: {symbol_file}")

    with open(symbol_file, 'r', encoding='utf-8', errors='ignore') as f:
        for line_no, line in enumerate(f, 1):
            if line_no % 10000 == 0:
                print(f"    处理第 {line_no} 行...")

            # 匹配格式: 地址 类型 函数名
            # 例如: 000000000814657c T InitBillingCash(Taiwan::TaiwanCash*&)
            match = re.match(r'([0-9a-f]{16})\s+([TtWw])\s+(.+)', line.strip())
            if match:
                addr = match.group(1)
                symbol_type = match.group(2)
                func_name = match.group(3)

                # 只保留函数符号（T/t/W/w）
                # T: 全局函数, t: 局部函数, W: 弱符号
                if symbol_type in ['T', 't', 'W', 'w']:
                    mapping[addr] = func_name

    return mapping

def generate_mapping_file(mapping, output_file):
    """
    生成映射文件，格式：
    0x84FC37E\tCInventory::ChangeEquip(INVEN_TYPE, int, int)
    """
    print(f"\n[*] 生成映射文件: {output_file}")

    with open(output_file, 'w', encoding='utf-8') as f:
        for addr, func_name in sorted(mapping.items()):
            # 地址格式：去掉前导0，转大写
            # 000000000814657c → 814657C
            clean_addr = addr.lstrip('0').upper() or '0'

            f.write(f"0x{clean_addr}\t{func_name}\n")

    print(f"[+] 映射条目数: {len(mapping)}")
    print(f"[+] 文件已保存: {output_file}")

def analyze_mapping(mapping):
    """
    分析映射统计
    """
    print(f"\n[统计分析]")
    print(f"  总函数符号: {len(mapping)}")

    # 统计关键类
    cuser_methods = [name for name in mapping.values() if 'CUser::' in name]
    cinventory_methods = [name for name in mapping.values() if 'CInventory::' in name]
    cparty_methods = [name for name in mapping.values() if 'CParty::' in name]
    taiwan_methods = [name for name in mapping.values() if 'Taiwan::' in name]
    mysql_methods = [name for name in mapping.values() if 'MySQL::' in name]

    print(f"\n[核心类统计]")
    print(f"  CUser类方法: {len(cuser_methods)}")
    print(f"  CInventory类方法: {len(cinventory_methods)}")
    print(f"  CParty类方法: {len(cparty_methods)}")
    print(f"  Taiwan类方法: {len(taiwan_methods)}")
    print(f"  MySQL类方法: {len(mysql_methods)}")

    # 示例
    print(f"\n[CUser类方法示例]")
    for method in cuser_methods[:5]:
        print(f"  - {method}")

    print(f"\n[CInventory类方法示例]")
    for method in cinventory_methods[:5]:
        print(f"  - {method}")

    print(f"\n[Taiwan类方法示例]")
    for method in taiwan_methods[:5]:
        print(f"  - {method}")

def main():
    # 文件路径
    symbol_file = Path("symbols/df_game_r_symbols_full.txt")
    output_file = Path("symbols/function_name_mapping.txt")

    # 检查输入文件
    if not symbol_file.exists():
        print(f"[!] 错误: 符号表文件不存在: {symbol_file}")
        print(f"[*] 请确保已下载符号表")
        return 1

    # 创建输出目录
    output_file.parent.mkdir(exist_ok=True)

    # 解析符号表
    print(f"[1/3] 解析符号表")
    mapping = parse_symbols(symbol_file)
    print(f"[+] 解析完成，找到 {len(mapping)} 个函数符号")

    # 生成映射文件
    print(f"\n[2/3] 生成映射文件")
    generate_mapping_file(mapping, output_file)

    # 统计分析
    print(f"\n[3/3] 统计分析")
    analyze_mapping(mapping)

    print(f"\n[✓] 完成！")
    print(f"[*] 下一步: 运行 python tools/rename_functions.py 批量替换函数名")

    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
