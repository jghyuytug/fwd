#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
参数重命名工具 - Phase 2
根据类型和上下文推断参数的有意义名称

策略：
1. 基于类型的推断（CUser* → user）
2. 基于位置的推断（第一个参数通常是主体）
3. 基于函数名的推断（gain_money(amount, reason)）
4. 保留已有意义的名称
"""

import re
import sys
import os
from typing import Dict, List, Tuple, Optional

# ============================================================================
# 参数命名规则
# ============================================================================

# 基于类型的命名规则
TYPE_BASED_NAMES = {
    # 核心游戏类
    'CUser *': ['user', 'player', 'target_user', 'src_user', 'dst_user'],
    'CUser **': ['user_ptr', 'user_list', 'users'],
    'CUserCharacInfo *': ['charac_info', 'character', 'charac'],
    'CInventory *': ['inventory', 'inven'],
    'CParty *': ['party', 'group'],
    'CDungeon *': ['dungeon', 'dng'],
    'CBattle_Field *': ['battle', 'battlefield'],

    # 数据库类
    'MySQL *': ['mysql', 'db', 'connection', 'conn'],
    'MySQL **': ['mysql_ptr', 'db_ptr'],

    # 物品系统
    'Inven_Item *': ['item', 'equipment', 'gear'],
    'Inven_Item **': ['item_ptr', 'items'],

    # 字符串
    'char *': ['str', 'buffer', 'data', 'name', 'message', 'text'],
    'const char *': ['str', 'name', 'key', 'value', 'text'],
    'char **': ['str_ptr', 'argv', 'strings'],
    'std::string *': ['str', 'text', 'message'],
    'std::string &': ['str', 'text', 'message'],

    # 整数类型
    'int': ['result', 'ret', 'value', 'count', 'index', 'id', 'num'],
    'unsigned int': ['count', 'amount', 'size', 'value', 'id'],
    '_DWORD': ['value', 'data', 'param', 'arg'],
    '__int64': ['value', 'timestamp', 'id'],

    # 布尔类型
    'bool': ['flag', 'is_valid', 'success', 'enabled'],
    'char': ['flag', 'type', 'state', 'byte'],
    '_BOOL4': ['result', 'success', 'is_valid'],

    # 指针类型
    'void *': ['ptr', 'data', 'buffer', 'context'],
    '_BYTE *': ['buffer', 'data', 'bytes'],
}

# 基于函数名的上下文推断
FUNCTION_CONTEXT_PATTERNS = {
    r'gain_money': {
        'unsigned int': 'amount',
        'char *': 'reason',
        'char': 'write_log',
        'int': 'bonus'
    },
    r'use_money': {
        'int': 'amount',
        'char *': 'reason',
        'char': 'write_log'
    },
    r'add_item': {
        'int': 'slot',
        'unsigned int': 'item_id',
        '_DWORD': 'count'
    },
    r'delete_item': {
        'int': 'slot',
        'unsigned int': 'count',
        'char': 'type'
    },
    r'send.*packet': {
        'int': 'packet_type',
        'char *': 'data',
        'unsigned int': 'size'
    },
    r'recv.*packet': {
        'int': 'packet_type',
        'char *': 'buffer',
        'unsigned int': 'size'
    },
    r'check|is|has': {
        'int': 'id',
        'unsigned int': 'type'
    },
    r'get|load|read': {
        'int': 'id',
        'char *': 'key'
    },
    r'set|save|write': {
        'int': 'value',
        'char *': 'data'
    },
}

# 已有意义的参数名（不需要重命名）
MEANINGFUL_NAMES = {
    'user', 'player', 'charac', 'character', 'charac_info',
    'inventory', 'inven', 'item', 'party', 'dungeon',
    'mysql', 'db', 'connection', 'conn',
    'amount', 'count', 'size', 'length', 'index', 'offset',
    'buffer', 'data', 'str', 'name', 'message', 'text',
    'result', 'ret', 'value', 'flag', 'type', 'state',
    'acc_name', 'acc_id', 'charac_no', 'charac_level',
    'src', 'dst', 'source', 'destination', 'target',
    'old', 'new', 'current', 'previous', 'next',
    'min', 'max', 'total', 'average', 'sum',
    'x', 'y', 'z', 'width', 'height', 'depth',
    'start', 'end', 'begin', 'finish',
    'key', 'value', 'pair', 'node', 'list',
    'reason', 'cause', 'error', 'status', 'code',
}

# ============================================================================
# 参数解析
# ============================================================================

class Parameter:
    """表示一个函数参数"""
    def __init__(self, type_str: str, name: str, position: int):
        self.type_str = type_str.strip()
        self.name = name.strip()
        self.position = position
        self.new_name = None

    def is_generic(self) -> bool:
        """检查是否是泛型参数名（a1, a2, v1, v2）"""
        return re.match(r'^[av]\d+$', self.name) is not None

    def is_meaningful(self) -> bool:
        """检查是否已有意义的名称"""
        return self.name.lower() in MEANINGFUL_NAMES or not self.is_generic()

    def __repr__(self):
        return f"Parameter({self.type_str} {self.name}, pos={self.position})"


class FunctionSignature:
    """表示一个函数签名"""
    def __init__(self, line: str, line_no: int):
        self.original_line = line
        self.line_no = line_no
        self.function_name = ""
        self.return_type = ""
        self.parameters: List[Parameter] = []
        self.parse()

    def parse(self):
        """解析函数签名"""
        # 提取函数名
        # 格式：return_type __cdecl ClassName::FuncName(params)
        match = re.search(r'(\w+(?:::\w+)*)\s*\((.*)\)\s*;?\s*$', self.original_line)
        if not match:
            return

        self.function_name = match.group(1)
        params_str = match.group(2)

        # 解析参数列表
        if params_str.strip() and params_str.strip() != 'void':
            self.parameters = self._parse_parameters(params_str)

    def _parse_parameters(self, params_str: str) -> List[Parameter]:
        """解析参数列表"""
        parameters = []
        position = 0

        # 分割参数（处理函数指针和模板）
        depth = 0
        current_param = ""

        for char in params_str + ',':
            if char in '<(':
                depth += 1
            elif char in '>)':
                depth -= 1

            if char == ',' and depth == 0:
                if current_param.strip():
                    param = self._parse_single_parameter(current_param.strip(), position)
                    if param:
                        parameters.append(param)
                        position += 1
                current_param = ""
            else:
                current_param += char

        return parameters

    def _parse_single_parameter(self, param_str: str, position: int) -> Optional[Parameter]:
        """解析单个参数"""
        # 简化的参数解析
        # 格式：Type name 或 Type *name 或 Type &name

        # 移除调用约定
        param_str = re.sub(r'\b__cdecl\b', '', param_str)
        param_str = re.sub(r'\b__fastcall\b', '', param_str)
        param_str = re.sub(r'\b__stdcall\b', '', param_str)

        # 尝试匹配：type name
        match = re.match(r'^(.+?)\s+(\w+)\s*$', param_str)
        if match:
            type_str = match.group(1).strip()
            name = match.group(2).strip()
            return Parameter(type_str, name, position)

        # 只有类型没有名称
        match = re.match(r'^(.+?)$', param_str)
        if match:
            type_str = match.group(1).strip()
            return Parameter(type_str, f'a{position}', position)

        return None

    def needs_renaming(self) -> bool:
        """检查是否需要重命名"""
        return any(p.is_generic() for p in self.parameters)

    def __repr__(self):
        return f"FunctionSignature({self.function_name}, {len(self.parameters)} params)"


# ============================================================================
# 参数重命名引擎
# ============================================================================

class ParameterRenamer:
    """参数重命名引擎"""

    def __init__(self):
        self.stats = {
            'total_functions': 0,
            'functions_with_params': 0,
            'generic_params_found': 0,
            'params_renamed': 0,
            'meaningful_params_kept': 0,
        }

    def suggest_name(self, param: Parameter, func_sig: FunctionSignature) -> str:
        """为参数建议新名称"""

        # 1. 如果已经有意义，保持原名
        if param.is_meaningful():
            self.stats['meaningful_params_kept'] += 1
            return param.name

        # 2. 特殊处理 'this' 参数
        if param.name == 'this':
            return 'this'

        # 3. 基于函数名的上下文推断
        for pattern, type_names in FUNCTION_CONTEXT_PATTERNS.items():
            if re.search(pattern, func_sig.function_name, re.IGNORECASE):
                for type_pattern, suggested_name in type_names.items():
                    if type_pattern in param.type_str:
                        return suggested_name

        # 4. 基于类型的推断
        for type_pattern, name_candidates in TYPE_BASED_NAMES.items():
            if type_pattern in param.type_str:
                # 根据位置选择候选名称
                idx = min(param.position, len(name_candidates) - 1)
                return name_candidates[idx]

        # 5. 默认：保持原名但添加注释
        return param.name

    def rename_function_signature(self, func_sig: FunctionSignature) -> str:
        """重命名函数签名中的参数"""
        if not func_sig.needs_renaming():
            return func_sig.original_line

        new_line = func_sig.original_line

        # 按照从后往前的顺序替换（避免位置偏移）
        for param in reversed(func_sig.parameters):
            if param.is_generic():
                new_name = self.suggest_name(param, func_sig)
                if new_name != param.name:
                    # 使用单词边界确保精确匹配
                    pattern = r'\b' + re.escape(param.name) + r'\b'
                    new_line = re.sub(pattern, new_name, new_line)
                    self.stats['params_renamed'] += 1

        return new_line

    def process_file(self, input_file: str, output_file: str):
        """处理整个文件"""
        print(f"[*] 读取文件: {input_file}")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        print(f"[*] 总行数: {len(lines):,}")
        print(f"[*] 开始重命名参数...")

        new_lines = []
        for line_no, line in enumerate(lines, 1):
            # 只处理函数声明和定义
            if re.search(r'\w+\s*\([^)]*\)\s*;?\s*$', line):
                func_sig = FunctionSignature(line, line_no)
                if func_sig.function_name:
                    self.stats['total_functions'] += 1
                    if func_sig.parameters:
                        self.stats['functions_with_params'] += 1
                        self.stats['generic_params_found'] += sum(
                            1 for p in func_sig.parameters if p.is_generic()
                        )

                    new_line = self.rename_function_signature(func_sig)
                    new_lines.append(new_line)
                else:
                    new_lines.append(line)
            else:
                new_lines.append(line)

            if line_no % 100000 == 0:
                print(f"    处理第 {line_no:,} 行...")

        print(f"[*] 写入文件: {output_file}")
        with open(output_file, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)

        print(f"[+] 完成！")

    def print_stats(self):
        """打印统计信息"""
        print("\n[统计结果]")
        print(f"  总函数数: {self.stats['total_functions']:,}")
        print(f"  有参数的函数: {self.stats['functions_with_params']:,}")
        print(f"  泛型参数总数: {self.stats['generic_params_found']:,}")
        print(f"  已重命名参数: {self.stats['params_renamed']:,}")
        print(f"  保留有意义参数: {self.stats['meaningful_params_kept']:,}")

        if self.stats['generic_params_found'] > 0:
            coverage = (self.stats['params_renamed'] / self.stats['generic_params_found']) * 100
            print(f"\n[覆盖率]")
            print(f"  参数重命名覆盖率: {coverage:.1f}%")


# ============================================================================
# 主程序
# ============================================================================

def main():
    print("="*60)
    print("参数重命名工具 - Phase 2")
    print("="*60)

    # 文件路径
    input_file = 'df_game_r.c'
    output_file = 'df_game_r_params_renamed.c'

    # 检查文件存在
    if not os.path.exists(input_file):
        print(f"[!] 错误: 文件不存在: {input_file}")
        return 1

    # 创建重命名器
    renamer = ParameterRenamer()

    # 处理文件
    renamer.process_file(input_file, output_file)

    # 打印统计
    renamer.print_stats()

    return 0


if __name__ == '__main__':
    sys.exit(main())
