#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
增强版参数重命名工具
在 df_game_r_renamed.c 的基础上继续改进，处理 int/char/unsigned int 等参数
"""

import re
import sys

# ============================================================================
# 基于函数名的参数推断规则
# ============================================================================

FUNCTION_PATTERNS = {
    # 金币相关
    r'gain.*money|add.*money': {
        'unsigned int': 'amount',
        'char \*': 'reason',
        'char': 'write_log',
        'int': 'bonus'
    },
    r'use.*money|lose.*money': {
        'int': 'amount',
        'char \*': 'reason',
        'char': 'write_log'
    },

    # 物品相关
    r'add.*item|gain.*item': {
        'int': 'item_id',
        'unsigned int': 'count',
        'char': 'quality'
    },
    r'delete.*item|remove.*item': {
        'int': 'slot',
        'unsigned int': 'count',
        'char': 'delete_type'
    },
    r'use.*item': {
        'int': 'slot',
        'char \*': 'target',
        'int': 'count'
    },

    # 发送/接收相关
    r'send.*packet|write.*packet': {
        'int': 'packet_type',
        'char \*': 'buffer',
        'unsigned int': 'size',
        'int': 'flags'
    },
    r'recv.*packet|read.*packet': {
        'char \*': 'buffer',
        'unsigned int': 'buffer_size',
        'int': 'flags'
    },

    # 查询相关
    r'get|load|fetch|read|find': {
        'int': 'id',
        'char \*': 'key',
        'unsigned int': 'index'
    },

    # 设置相关
    r'set|save|write|update': {
        'int': 'value',
        'char \*': 'data',
        'unsigned int': 'size'
    },

    # 检查相关
    r'check|is|has|can': {
        'int': 'id',
        'unsigned int': 'type',
        'char': 'flag'
    },

    # 创建相关
    r'create|make|generate': {
        'int': 'count',
        'char \*': 'name',
        'unsigned int': 'type'
    },
}

# 通用参数名映射（按位置）
GENERIC_NAMES = {
    'int': ['id', 'value', 'count', 'index', 'type', 'result', 'status'],
    'unsigned int': ['count', 'amount', 'size', 'index', 'id', 'type'],
    'char': ['flag', 'type', 'state', 'option', 'mode'],
    'char \*': ['buffer', 'data', 'str', 'name', 'key', 'message'],
    '_DWORD': ['value', 'param', 'arg', 'data'],
}

# ============================================================================
# 重命名引擎
# ============================================================================

class EnhancedRenamer:
    def __init__(self):
        self.stats = {
            'total_lines': 0,
            'renamed_lines': 0,
            'context_renamed': 0,
            'generic_renamed': 0,
        }

    def get_function_name(self, line):
        """从函数定义行提取函数名"""
        # 匹配函数定义：return_type function_name(params)
        match = re.search(r'\s+(\w+(?:::\w+)*)\s*\(', line)
        if match:
            return match.group(1)
        return None

    def match_function_pattern(self, func_name):
        """检查函数名是否匹配某个模式"""
        if not func_name:
            return None

        for pattern, param_rules in FUNCTION_PATTERNS.items():
            if re.search(pattern, func_name, re.IGNORECASE):
                return param_rules
        return None

    def rename_parameters(self, line, func_name):
        """重命名一行中的参数"""
        modified = False
        param_rules = self.match_function_pattern(func_name)

        # 跟踪已使用的名称（避免重复）
        used_names = set()

        # 优先使用上下文规则
        if param_rules:
            for type_pattern, suggested_name in param_rules.items():
                # 匹配：type a1, type a2, etc.
                pattern = re.escape(type_pattern) + r'\s+([av]\d+)\b'

                def replace_with_context(match):
                    nonlocal modified
                    old_name = match.group(1)
                    # 如果名称已使用，添加数字后缀
                    new_name = suggested_name
                    if new_name in used_names:
                        counter = 2
                        while f"{suggested_name}{counter}" in used_names:
                            counter += 1
                        new_name = f"{suggested_name}{counter}"

                    used_names.add(new_name)
                    modified = True
                    self.stats['context_renamed'] += 1
                    return f"{type_pattern} {new_name}"

                line = re.sub(pattern, replace_with_context, line)

        # 通用规则（对于未被上下文规则处理的参数）
        for type_pattern, name_list in GENERIC_NAMES.items():
            pattern = re.escape(type_pattern) + r'\s+([av]\d+)\b'

            # 统计当前还有多少个该类型的未命名参数
            remaining = len(re.findall(pattern, line))
            if remaining == 0:
                continue

            counter = 0
            def replace_generic(match):
                nonlocal modified, counter
                old_name = match.group(1)

                # 从名称列表中选择（循环使用）
                idx = counter % len(name_list)
                base_name = name_list[idx]

                # 如果名称已使用，添加数字
                new_name = base_name
                if new_name in used_names:
                    suffix = 2
                    while f"{base_name}{suffix}" in used_names:
                        suffix += 1
                    new_name = f"{base_name}{suffix}"

                used_names.add(new_name)
                counter += 1
                modified = True
                self.stats['generic_renamed'] += 1
                return f"{type_pattern} {new_name}"

            line = re.sub(pattern, replace_generic, line)

        return line, modified

    def process_file(self, input_file, output_file):
        """处理文件"""
        print(f"[*] 读取文件: {input_file}")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        print(f"[*] 总行数: {len(lines):,}")
        print(f"[*] 开始重命名...")

        current_function = None
        new_lines = []

        for line_no, line in enumerate(lines, 1):
            self.stats['total_lines'] += 1

            # 检测函数定义（判断是否是新函数开始）
            if re.match(r'^\w+.*\(.*\)\s*$', line.strip()) and not line.strip().startswith('//'):
                func_name = self.get_function_name(line)
                if func_name:
                    current_function = func_name

            # 重命名参数
            new_line, modified = self.rename_parameters(line, current_function)

            if modified:
                self.stats['renamed_lines'] += 1

            new_lines.append(new_line)

            if line_no % 100000 == 0:
                print(f"    处理第 {line_no:,} 行... (已重命名 {self.stats['renamed_lines']:,} 行)")

        print(f"[*] 写入文件: {output_file}")
        with open(output_file, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)

        print(f"[+] 完成！")

    def print_stats(self):
        """打印统计"""
        print("\n[统计结果]")
        print(f"  总行数: {self.stats['total_lines']:,}")
        print(f"  重命名行数: {self.stats['renamed_lines']:,}")
        print(f"  上下文重命名: {self.stats['context_renamed']:,}")
        print(f"  通用重命名: {self.stats['generic_renamed']:,}")

        if self.stats['total_lines'] > 0:
            pct = (self.stats['renamed_lines'] / self.stats['total_lines']) * 100
            print(f"\n[覆盖率]")
            print(f"  修改行比例: {pct:.2f}%")

# ============================================================================
# 主程序
# ============================================================================

def main():
    print("="*60)
    print("增强版参数重命名工具")
    print("="*60)

    # 基于已重命名的文件继续工作
    input_file = 'df_game_r_renamed.c'
    output_file = 'df_game_r_v2.c'

    import os
    if not os.path.exists(input_file):
        print(f"[!] 错误: 文件不存在: {input_file}")
        print(f"[*] 提示: 请先运行 rename_params_simple.py 生成 {input_file}")
        return 1

    renamer = EnhancedRenamer()
    renamer.process_file(input_file, output_file)
    renamer.print_stats()

    return 0

if __name__ == '__main__':
    sys.exit(main())
