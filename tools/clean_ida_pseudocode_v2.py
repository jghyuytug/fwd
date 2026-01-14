#!/usr/bin/env python3
"""
IDA伪代码清理工具 v2 - Round 4

目标：清理残留的4,752个TEMPLATE_SPECIALIZATION错误

新增规则：
4. OPERATOR_OVERLOAD - operator重载错误
5. REMAINING_TEMPLATE_SPEC - 遗漏的模板特化
6. WRONG_SCOPE_DECLARATION - 作用域错误

使用方法：
    python tools/clean_ida_pseudocode_v2.py --input src_split/df_game_r_decl.h --dry-run
    python tools/clean_ida_pseudocode_v2.py --input src_split/df_game_r_decl.h --output src_split/df_game_r_decl.h
"""

import re
import sys
import argparse
from pathlib import Path
from datetime import datetime

class IDACodeCleaner:
    def __init__(self):
        self.lines_to_comment = set()  # 要注释的行号
        self.comment_reasons = {}  # 行号 -> 注释原因
        self.stats = {
            'total_lines': 0,
            'template_specialization': 0,
            'wrong_scope': 0,
            'invalid_this': 0,
            'member_function_errors': 0,
            'operator_overload': 0,
            'remaining_template_spec': 0,
            'wrong_scope_declaration': 0,
            'other_ida_artifacts': 0
        }

    def analyze_file(self, file_path):
        """分析文件，识别所有IDA伪代码"""
        print(f"[*] Analyzing file: {file_path}")

        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        self.stats['total_lines'] = len(lines)

        for line_num, line in enumerate(lines, 1):
            # 跳过已经注释的行
            if line.strip().startswith('//'):
                continue

            # 规则1：STL模板成员特化（缺少template<>）
            if self._is_template_specialization(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'TEMPLATE_SPECIALIZATION'
                self.stats['template_specialization'] += 1

            # 规则2：STL函数错误声明（should have been declared inside）
            elif self._is_wrong_scope(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'WRONG_SCOPE'
                self.stats['wrong_scope'] += 1

            # 规则3：this指针显式写出
            elif self._has_explicit_this(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'INVALID_THIS'
                self.stats['invalid_this'] += 1

            # 规则4：成员函数错误声明
            elif self._is_member_function_error(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'MEMBER_FUNCTION_ERROR'
                self.stats['member_function_errors'] += 1

            # 规则5：operator重载错误
            elif self._is_operator_overload_error(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'OPERATOR_OVERLOAD'
                self.stats['operator_overload'] += 1

            # 规则6：残留模板特化
            elif self._is_remaining_template_specialization(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'REMAINING_TEMPLATE_SPEC'
                self.stats['remaining_template_spec'] += 1

            # 规则7：作用域错误
            elif self._is_wrong_scope_declaration(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'WRONG_SCOPE_DECLARATION'
                self.stats['wrong_scope_declaration'] += 1

        print(f"[OK] Analysis complete: {len(self.lines_to_comment)} lines to comment")
        return len(self.lines_to_comment)

    def _is_template_specialization(self, line):
        """
        识别STL模板成员特化（IDA错误声明）

        特征：
        - 包含 std::, __gnu_cxx:: 等STL命名空间
        - 包含模板参数 <...>
        - 是成员函数 ::function_name
        - 不是函数指针或typedef
        """
        line = line.strip()

        # 必须包含STL命名空间
        if not ('std::' in line or '__gnu_cxx::' in line):
            return False

        # 必须包含 :: （成员函数）
        if '::' not in line:
            return False

        # 必须包含模板参数
        if '<' not in line or '>' not in line:
            return False

        # 排除typedef和函数指针
        if line.startswith('typedef') or '(*' in line:
            return False

        # 包含常见的STL容器/算法名称
        stl_patterns = [
            r'std::_Rb_tree',
            r'std::__copy_move',
            r'std::_List_',
            r'std::_Vector_',
            r'std::_Deque_',
            r'std::forward',
            r'std::move',
            r'std::allocator',
            r'__gnu_cxx::new_allocator',
        ]

        for pattern in stl_patterns:
            if re.search(pattern, line):
                return True

        return False

    def _is_wrong_scope(self, line):
        """
        识别错误作用域声明

        特征：
        - STL函数被声明为全局函数
        - 应该在std::命名空间内
        """
        line = line.strip()

        # 特征：int __cdecl std::function_name(...)
        # 这些函数应该在namespace std内声明，不应该有返回类型前缀
        wrong_scope_patterns = [
            r'^int __cdecl std::forward\(',
            r'^int __cdecl std::__copy_move',
            r'^void __cdecl std::',
            r'^\w+ __cdecl std::',
        ]

        for pattern in wrong_scope_patterns:
            if re.search(pattern, line):
                return True

        return False

    def _has_explicit_this(self, line):
        """
        识别显式this指针

        特征：
        - 参数列表中包含 *this
        - 或者 (boost::xxx *this)
        """
        line = line.strip()

        # 跳过注释
        if line.startswith('//'):
            return False

        # 特征1: function(..., Type *this, ...)
        if re.search(r'\([^)]*\*this[,\)]', line):
            return True

        # 特征2: function(Type *this)
        if re.search(r'\(\s*\w+\s*\*\s*this\s*\)', line):
            return True

        return False

    def _is_member_function_error(self, line):
        """
        识别成员函数错误声明

        特征：
        - 类似 int operator()(...)
        - 这些应该是成员函数，但被声明为全局函数
        """
        line = line.strip()

        # operator() 必须是成员函数
        if re.search(r'^\w+\s+operator\(\)\(', line):
            return True

        # operator类型转换 必须是成员函数
        if re.search(r'^\w+\s+operator\s+\w+\(', line):
            return True

        return False

    def _is_operator_overload_error(self, line):
        """
        识别operator重载错误

        特征：
        - _DWORD* std::operator<<
        - operator+ with too many params
        - postfix operator++
        """
        line = line.strip()
        if line.startswith('//'):
            return False

        # _DWORD* std::operator<<
        if re.search(r'_DWORD\*\s+std::operator[<>]{1,2}\s*\(', line):
            return True

        # operator+ with too many params (should have 1-2 params, not 3+)
        if re.search(r'operator[\+\-\*/%]\s*\([^)]*,\s*[^)]*,', line):
            return True

        # postfix operator++
        if 'postfix' in line and 'operator++' in line:
            return True

        return False

    def _is_remaining_template_specialization(self, line):
        """
        识别遗漏的模板特化

        特征：
        - std::basic_ios等特化
        - std::locale::facet 系列
        - std::operator in global scope
        """
        line = line.strip()
        if line.startswith('//'):
            return False

        # std::basic_ios等特化
        if re.search(r'std::basic_(ios|istream|ostream|string)<\w+>::\w+\(', line):
            if not line.startswith('template<>'):
                return True

        # std::locale::facet系列 (messages, time_get, etc.)
        if re.search(r'std::(messages|time_get|time_put|money_get|money_put|moneypunct|num_get|num_put|numpunct|collate|codecvt|ctype|__timepunct)<', line):
            if not line.startswith('template<>'):
                return True

        # std::operator in global scope
        if re.search(r'^\w+\s+\*?\s*std::operator[<>]{1,2}\s*\(', line):
            return True

        return False

    def _is_wrong_scope_declaration(self, line):
        """
        识别作用域错误

        特征：
        - void* std::use_facet(
        - _DWORD* std::operator
        - __cdecl std::__xxx(
        """
        line = line.strip()
        if line.startswith('//'):
            return False

        patterns = [
            r'void\s*\*\s+__cdecl\s+std::use_facet<',
            r'_DWORD\s*\*\s+std::operator',
            r'\w+\s+__cdecl\s+std::__\w+\(',
        ]

        for pattern in patterns:
            if re.search(pattern, line):
                return True

        return False

    def clean_file(self, input_file, output_file, dry_run=False):
        """执行清理"""
        print(f"\n[*] {'DRY RUN: ' if dry_run else ''}Cleaning file...")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        cleaned_lines = []
        for line_num, line in enumerate(lines, 1):
            if line_num in self.lines_to_comment:
                reason = self.comment_reasons[line_num]
                # 注释该行
                if not line.strip().startswith('//'):
                    cleaned_line = f"// [REMOVED - IDA {reason}] {line}"
                else:
                    cleaned_line = line  # 已经是注释
                cleaned_lines.append(cleaned_line)
            else:
                cleaned_lines.append(line)

        if not dry_run:
            # 备份原文件
            backup_file = Path(input_file).with_suffix('.h.before_round4')
            print(f"[*] Creating backup: {backup_file}")
            with open(backup_file, 'w', encoding='utf-8') as f:
                f.writelines(lines)

            # 写入清理后的文件
            print(f"[*] Writing cleaned file: {output_file}")
            with open(output_file, 'w', encoding='utf-8') as f:
                f.writelines(cleaned_lines)

            print(f"[OK] File cleaned successfully")
        else:
            print(f"[*] DRY RUN: Would comment {len(self.lines_to_comment)} lines")

        return len(self.lines_to_comment)

    def generate_report(self, output_file):
        """生成清理报告"""
        print(f"\n[*] Generating cleanup report: {output_file}")

        report = []
        report.append("# IDA伪代码清理报告 - Round 4")
        report.append(f"\n**日期：** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report.append(f"**清理文件：** df_game_r_decl.h")
        report.append("\n---\n")

        # 统计信息
        report.append("## 1. 清理统计\n")
        report.append(f"- **总行数：** {self.stats['total_lines']:,}")
        report.append(f"- **清理行数：** {len(self.lines_to_comment):,}")
        report.append(f"- **清理比例：** {len(self.lines_to_comment)/self.stats['total_lines']*100:.2f}%")
        report.append("")

        report.append("### 1.1 错误类型分布\n")
        report.append("| 错误类型 | 清理行数 |")
        report.append("|---------|---------|")
        report.append(f"| TEMPLATE_SPECIALIZATION | {self.stats['template_specialization']:,} |")
        report.append(f"| WRONG_SCOPE | {self.stats['wrong_scope']:,} |")
        report.append(f"| INVALID_THIS | {self.stats['invalid_this']:,} |")
        report.append(f"| MEMBER_FUNCTION_ERROR | {self.stats['member_function_errors']:,} |")
        report.append(f"| OPERATOR_OVERLOAD | {self.stats['operator_overload']:,} |")
        report.append(f"| REMAINING_TEMPLATE_SPEC | {self.stats['remaining_template_spec']:,} |")
        report.append(f"| WRONG_SCOPE_DECLARATION | {self.stats['wrong_scope_declaration']:,} |")
        report.append(f"| OTHER | {self.stats['other_ida_artifacts']:,} |")
        report.append(f"| **总计** | **{len(self.lines_to_comment):,}** |")

        # 示例
        report.append("\n---\n")
        report.append("## 2. 清理示例\n")

        # 按类型分组示例
        examples_by_type = {}
        for line_num in sorted(list(self.lines_to_comment)[:20]):  # 前20个示例
            reason = self.comment_reasons[line_num]
            if reason not in examples_by_type:
                examples_by_type[reason] = []
            examples_by_type[reason].append(line_num)

        for reason, line_nums in examples_by_type.items():
            report.append(f"\n### 2.{len(report)} {reason}\n")
            stat_key = reason.lower()
            if stat_key in self.stats:
                report.append(f"**清理行数：** {self.stats[stat_key]:,}\n")
            report.append("**示例行号：** " + ", ".join(str(ln) for ln in line_nums[:5]))

        # 预期效果
        report.append("\n---\n")
        report.append("## 3. 预期效果\n")
        report.append(f"Round 4 STL清理，清理这{len(self.lines_to_comment):,}行IDA伪代码后：\n")
        report.append(f"- **当前错误：** 17,772")
        report.append(f"- **目标错误：** 4,752 TEMPLATE_SPECIALIZATION")
        report.append(f"- **预期削减：** ~2,000-3,000个错误")
        report.append(f"- **预期剩余：** 14,500-15,500个错误")
        report.append(f"- **削减率：** ~15-20%")

        # 下一步
        report.append("\n---\n")
        report.append("## 4. 下一步\n")
        report.append("1. 重新编译验证")
        report.append("2. 统计实际错误数量")
        report.append("3. 如果成功（<15,500），继续Phase 2")
        report.append("4. 如果失败（>16,000），分析残留错误并调整策略")

        # 写入报告
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(report))

        print(f"[OK] Report generated: {output_file}")

    def show_preview(self, input_file, num_examples=10):
        """显示清理预览"""
        print(f"\n[*] Preview of lines to be commented (first {num_examples}):\n")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        shown = 0
        for line_num in sorted(self.lines_to_comment):
            if shown >= num_examples:
                break

            line = lines[line_num - 1].rstrip()
            reason = self.comment_reasons[line_num]

            print(f"Line {line_num} [{reason}]:")
            print(f"  Before: {line}")
            print(f"  After:  // [REMOVED - IDA {reason}] {line}")
            print()

            shown += 1


def main():
    parser = argparse.ArgumentParser(description='Clean IDA pseudocode from df_game_r_decl.h - Round 4')
    parser.add_argument('--input', required=True, help='Input file path')
    parser.add_argument('--output', help='Output file path (default: same as input)')
    parser.add_argument('--dry-run', action='store_true', help='Preview changes without modifying files')
    parser.add_argument('--report', default='logs/round4_cleanup.md', help='Report output path')
    parser.add_argument('--preview', type=int, default=10, help='Number of preview examples')

    args = parser.parse_args()

    if not Path(args.input).exists():
        print(f"[ERROR] Input file not found: {args.input}")
        sys.exit(1)

    if not args.output:
        args.output = args.input

    print("=" * 60)
    print("IDA Pseudocode Cleanup Tool v2.0 - Round 4")
    print("=" * 60)

    cleaner = IDACodeCleaner()

    # 分析文件
    num_lines = cleaner.analyze_file(args.input)

    if num_lines == 0:
        print("[*] No IDA pseudocode found!")
        sys.exit(0)

    # 显示预览
    cleaner.show_preview(args.input, args.preview)

    # 显示统计
    print("=" * 60)
    print("Cleanup Statistics:")
    print("=" * 60)
    print(f"Total lines:                {cleaner.stats['total_lines']:,}")
    print(f"Lines to comment:           {num_lines:,}")
    print(f"  TEMPLATE_SPECIALIZATION:  {cleaner.stats['template_specialization']:,}")
    print(f"  WRONG_SCOPE:              {cleaner.stats['wrong_scope']:,}")
    print(f"  INVALID_THIS:             {cleaner.stats['invalid_this']:,}")
    print(f"  MEMBER_FUNCTION_ERROR:    {cleaner.stats['member_function_errors']:,}")
    print(f"  OPERATOR_OVERLOAD:        {cleaner.stats['operator_overload']:,}")
    print(f"  REMAINING_TEMPLATE_SPEC:  {cleaner.stats['remaining_template_spec']:,}")
    print(f"  WRONG_SCOPE_DECLARATION:  {cleaner.stats['wrong_scope_declaration']:,}")
    print("=" * 60)

    if args.dry_run:
        print("\n[*] DRY RUN MODE - No files will be modified")
        print(f"[*] Run without --dry-run to execute cleanup")
    else:
        # 执行清理
        cleaner.clean_file(args.input, args.output, dry_run=False)

    # 生成报告
    Path(args.report).parent.mkdir(parents=True, exist_ok=True)
    cleaner.generate_report(args.report)

    print("\n" + "=" * 60)
    print("[OK] Cleanup complete!")
    print("=" * 60)

    if not args.dry_run:
        print(f"\nBackup saved to: {Path(args.input).with_suffix('.h.before_round4')}")
        print(f"Cleaned file:    {args.output}")
        print(f"Report:          {args.report}")
        print("\nNext step: Recompile to verify the effect")


if __name__ == '__main__':
    main()
