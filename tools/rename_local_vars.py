#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
局部变量重命名工具 - Phase 2.5

基于上下文智能推断 v1, v2, v3 等局部变量的含义并重命名

策略：
1. 分析变量的赋值语句，从右值推断用途
2. 分析变量的使用位置，从函数调用推断类型
3. 使用启发式规则生成有意义的名称
"""

import re
import sys
from collections import defaultdict, OrderedDict

class LocalVarRenamer:
    def __init__(self):
        # 变量重命名映射: (function_name, var_name) -> new_name
        self.var_mappings = {}

        # 统计信息
        self.total_replacements = 0
        self.renamed_vars = defaultdict(int)

        # 当前处理的函数
        self.current_function = None
        self.current_function_vars = {}  # 当前函数内的变量映射

        # 启发式规则
        self.setup_heuristics()

    def setup_heuristics(self):
        """设置变量命名的启发式规则"""

        # 基于赋值右值的命名规则
        self.assignment_patterns = [
            # 函数调用模式
            (r'(\w+)\s*=\s*CUser::get_acc_name', 'acc_name'),
            (r'(\w+)\s*=\s*CUser::get_acc_id', 'acc_id'),
            (r'(\w+)\s*=\s*CUser::get_charac_no', 'charac_no'),
            (r'(\w+)\s*=\s*CUser::GetUID', 'uid'),
            (r'(\w+)\s*=\s*CUser::getHackAnalyzer', 'hack_analyzer'),
            (r'(\w+)\s*=\s*CUser::GetGoldControl', 'gold_control'),
            (r'(\w+)\s*=\s*CUser::getDungeonGainedGold', 'dungeon_gained_gold'),

            (r'(\w+)\s*=\s*CUserCharacInfo::get_charac_level', 'charac_level'),
            (r'(\w+)\s*=\s*CUserCharacInfo::get_charac_name', 'charac_name'),
            (r'(\w+)\s*=\s*CUserCharacInfo::getCurCharacInvenR', 'inventory'),

            (r'(\w+)\s*=\s*CInventory::GetEventCoin', 'event_coin'),
            (r'(\w+)\s*=\s*CInventory::gain_money', 'result'),
            (r'(\w+)\s*=\s*CInventory::use_money', 'result'),

            (r'(\w+)\s*=\s*CItem::get_index', 'item_index'),
            (r'(\w+)\s*=\s*CItem::get_name', 'item_name'),
            (r'(\w+)\s*=\s*CItem::get_count', 'item_count'),

            (r'(\w+)\s*=\s*G_CDataManager\(\)', 'data_manager'),
            (r'(\w+)\s*=\s*CDataManager::Get\w+', 'data'),

            (r'(\w+)\s*=\s*NumberToString', 'str'),
            (r'(\w+)\s*=\s*std::string::', 'str'),

            (r'(\w+)\s*=\s*MySQL::get', 'db_value'),
            (r'(\w+)\s*=\s*MySQL::exec', 'db_result'),

            (r'(\w+)\s*=\s*operator new', 'ptr'),
            (r'(\w+)\s*=\s*malloc', 'buffer'),

            # 偏移访问模式
            (r'(\w+)\s*=\s*\*\(_DWORD \*\)\((\w+) \+ 4\)', 'value'),  # offset 4 通常是值
            (r'(\w+)\s*=\s*\*\(_DWORD \*\)(\w+)', 'ptr_value'),

            # 常量赋值
            (r'(\w+)\s*=\s*MAX_INT32', 'max_value'),
            (r'(\w+)\s*=\s*INVALID_ID', 'invalid_id'),
            (r'(\w+)\s*=\s*0[xX]FFFF', 'invalid_id'),

            # 时间相关
            (r'(\w+)\s*=\s*time\(', 'current_time'),
            (r'(\w+)\s*=\s*CSystemTime::getCurSec', 'current_sec'),
            (r'(\w+)\s*=\s*localtime', 'local_time'),

            # 循环变量
            (r'for\s*\(\s*(\w+)\s*=\s*0', 'i'),  # for ( v5 = 0 ) -> i
        ]

        # 基于类型推断的命名
        self.type_based_names = {
            'CUser *': 'user',
            'CUser **': 'user_ptr',
            'CUserCharacInfo *': 'charac_info',
            'CInventory *': 'inventory',
            'CItem *': 'item',
            'MySQL *': 'mysql',
            'CDataManager *': 'data_manager',
            'char *': 'str',
            'const char *': 'str',
            'unsigned int': 'value',
            'int': 'result',
            '_BOOL4': 'is_valid',
        }

    def analyze_function(self, function_lines):
        """分析一个函数内的变量使用"""
        function_vars = OrderedDict()  # 保持顺序

        for line in function_lines:
            # 查找变量声明
            # 例如: char *acc_name; // esi
            #       unsigned int v7; // eax
            var_decl_match = re.match(r'\s*(.+?)\s+(\w+);\s*//.*', line)
            if var_decl_match:
                var_type = var_decl_match.group(1).strip()
                var_name = var_decl_match.group(2)

                # 如果是 v 变量，添加到待重命名列表
                if re.match(r'^v\d+$', var_name):
                    # 先根据类型给一个默认名称
                    if var_type in self.type_based_names:
                        base_name = self.type_based_names[var_type]
                    else:
                        base_name = 'var'

                    function_vars[var_name] = {'type': var_type, 'new_name': None, 'base_name': base_name}

            # 查找变量赋值，更精确地推断名称
            for pattern, suggested_name in self.assignment_patterns:
                match = re.search(pattern, line)
                if match:
                    var_name = match.group(1)
                    if var_name in function_vars and function_vars[var_name]['new_name'] is None:
                        function_vars[var_name]['new_name'] = suggested_name

        # 为还没有新名称的变量生成名称（使用base_name + 序号）
        name_counters = defaultdict(int)

        for var_name, info in function_vars.items():
            if info['new_name'] is None:
                base = info['base_name']
                name_counters[base] += 1

                # 如果是第一个，不加序号
                if name_counters[base] == 1:
                    info['new_name'] = base
                else:
                    info['new_name'] = f"{base}{name_counters[base]}"

        return function_vars

    def extract_function_body(self, lines, start_idx):
        """提取函数体（从函数定义到下一个函数定义）"""
        function_lines = []
        i = start_idx

        # 找到函数签名
        while i < len(lines) and not re.match(r'^[a-zA-Z_].*::\w+\(', lines[i]):
            i += 1

        if i >= len(lines):
            return None, i

        function_name_match = re.search(r'(\w+::\w+)\s*\(', lines[i])
        if function_name_match:
            self.current_function = function_name_match.group(1)

        # 提取函数体
        brace_count = 0
        started = False

        while i < len(lines):
            line = lines[i]
            function_lines.append(line)

            # 计算大括号
            if '{' in line:
                brace_count += line.count('{')
                started = True
            if '}' in line:
                brace_count -= line.count('}')

            # 如果大括号平衡，函数结束
            if started and brace_count == 0:
                i += 1
                break

            i += 1

            # 如果遇到下一个函数定义（没有缩进的）
            if started and re.match(r'^[a-zA-Z_]', line) and '::' in line:
                break

        return function_lines, i

    def rename_in_function(self, function_lines, var_mappings):
        """在函数体内重命名变量"""
        renamed_lines = []
        replacements_in_function = 0

        for line in function_lines:
            new_line = line

            # 按照变量名长度降序排列（避免 v10 被误替换成 v1）
            sorted_vars = sorted(var_mappings.items(), key=lambda x: len(x[0]), reverse=True)

            for old_var, info in sorted_vars:
                new_var = info['new_name']

                # 使用正则表达式确保只替换完整的变量名
                # \b 表示单词边界
                pattern = r'\b' + re.escape(old_var) + r'\b'

                matches = re.findall(pattern, new_line)
                if matches:
                    new_line = re.sub(pattern, new_var, new_line)
                    replacements_in_function += len(matches)
                    self.renamed_vars[new_var] += len(matches)

            renamed_lines.append(new_line)

        self.total_replacements += replacements_in_function
        return renamed_lines

    def process_file(self, input_file, output_file, sample_size=None):
        """处理整个文件"""
        print(f"[*] 读取文件: {input_file}")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        total_lines = len(lines)
        print(f"[+] 总行数: {total_lines:,}")

        if sample_size:
            print(f"[!] 采样模式: 只处理前 {sample_size:,} 行")
            lines = lines[:sample_size]

        print(f"\n[*] 开始处理...")

        output_lines = []
        i = 0
        function_count = 0

        while i < len(lines):
            # 检测函数定义
            if re.match(r'^[a-zA-Z_].*::\w+\(', lines[i]):
                function_count += 1

                if function_count % 1000 == 0:
                    print(f"    处理了 {function_count:,} 个函数, 第 {i:,} 行...")

                # 提取函数体
                function_lines, next_i = self.extract_function_body(lines, i)

                if function_lines:
                    # 分析函数内的变量
                    var_mappings = self.analyze_function(function_lines)

                    # 执行重命名
                    if var_mappings:
                        renamed_function = self.rename_in_function(function_lines, var_mappings)
                        output_lines.extend(renamed_function)
                    else:
                        output_lines.extend(function_lines)

                    i = next_i
                else:
                    output_lines.append(lines[i])
                    i += 1
            else:
                output_lines.append(lines[i])
                i += 1

        print(f"\n[+] 处理完成")
        print(f"[+] 处理了 {function_count:,} 个函数")
        print(f"[+] 总替换次数: {self.total_replacements:,}")

        # 写入输出文件
        print(f"\n[*] 写入输出文件: {output_file}")
        with open(output_file, 'w', encoding='utf-8') as f:
            f.writelines(output_lines)

        print(f"[+] 完成！")

    def print_statistics(self):
        """打印统计信息"""
        print(f"\n{'='*80}")
        print("重命名统计 (Top 50)")
        print(f"{'='*80}\n")

        sorted_vars = sorted(self.renamed_vars.items(),
                            key=lambda x: x[1],
                            reverse=True)

        print(f"{'新变量名':<30} {'使用次数':<15}")
        print("-" * 80)

        for var_name, count in sorted_vars[:50]:
            print(f"{var_name:<30} {count:<15,}")

        if len(sorted_vars) > 50:
            print(f"\n... 还有 {len(sorted_vars) - 50} 个变量")

    def generate_report(self, output_file):
        """生成重命名报告"""
        print(f"\n[*] 生成报告: {output_file}")

        sorted_vars = sorted(self.renamed_vars.items(),
                            key=lambda x: x[1],
                            reverse=True)

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("# Phase 2.5: 局部变量重命名报告\n\n")
            f.write(f"## 总体统计\n\n")
            f.write(f"- 总替换次数: {self.total_replacements:,}\n")
            f.write(f"- 不同变量数: {len(self.renamed_vars)}\n\n")

            f.write(f"## 重命名详情\n\n")
            f.write(f"| 排名 | 新变量名 | 使用次数 |\n")
            f.write(f"|------|----------|----------|\n")

            for rank, (var_name, count) in enumerate(sorted_vars, 1):
                f.write(f"| {rank} | {var_name} | {count:,} |\n")

        print(f"[+] 报告已生成")


def main():
    print("="*80)
    print("Phase 2.5: 局部变量重命名工具")
    print("="*80)
    print()
    print("警告: 这是一个复杂且耗时的操作！")
    print("建议先在小样本上测试（例如前 100,000 行）")
    print()

    # 创建重命名器
    renamer = LocalVarRenamer()

    # 先进行小样本测试
    print("[*] 第一阶段: 小样本测试 (前 200,000 行)")
    print("[*] 注: 前 97,000 行是函数声明，函数定义从第 97,297 行开始")
    renamer.process_file('df_game_r_v3.c', 'df_game_r_v4_sample.c', sample_size=200000)

    # 打印统计
    renamer.print_statistics()

    # 生成报告
    renamer.generate_report('PHASE2.5_SAMPLE_REPORT.md')

    print("\n" + "="*80)
    print("小样本测试完成！请检查 df_game_r_v4_sample.c")
    print("如果结果满意，请运行完整处理")
    print("="*80)

    return 0


if __name__ == '__main__':
    sys.exit(main())
