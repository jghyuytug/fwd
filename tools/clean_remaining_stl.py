#!/usr/bin/env python3
"""
第三轮清理：残留的STL伪代码

目标：清理Day 2遗漏的STL错误声明
预计清理：~1,500-2,000个错误

遗漏的模式：
1. 缺少template<>的模板特化（但不是std::_Rb_tree等常见类）
2. STL函数错误作用域声明
3. 其他STL相关错误

使用方法：
    python tools/clean_remaining_stl.py --input src_split/df_game_r_decl.h --dry-run
    python tools/clean_remaining_stl.py --input src_split/df_game_r_decl.h
"""

import re
import sys
import argparse
from pathlib import Path

class RemainingSTLCleaner:
    def __init__(self):
        self.lines_to_comment = set()
        self.comment_reasons = {}
        self.stats = {
            'template_specialization_missed': 0,
            'wrong_scope_missed': 0,
            'other_stl_errors': 0,
            'total_lines': 0
        }

    def analyze_file(self, file_path):
        """分析文件，识别残留的STL伪代码"""
        print(f"[*] Analyzing remaining STL errors in: {file_path}")

        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        self.stats['total_lines'] = len(lines)

        for line_num, line in enumerate(lines, 1):
            # 跳过已注释
            if line.strip().startswith('//'):
                continue

            # 规则1：遗漏的模板特化
            if self._is_missed_template_specialization(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'TEMPLATE_SPEC_MISSED'
                self.stats['template_specialization_missed'] += 1

            # 规则2：遗漏的作用域错误
            elif self._is_missed_scope_error(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'WRONG_SCOPE_MISSED'
                self.stats['wrong_scope_missed'] += 1

            # 规则3：其他STL错误模式
            elif self._is_other_stl_error(line):
                self.lines_to_comment.add(line_num)
                self.comment_reasons[line_num] = 'OTHER_STL'
                self.stats['other_stl_errors'] += 1

        print(f"[OK] Found {len(self.lines_to_comment)} remaining STL lines")
        return len(self.lines_to_comment)

    def _is_missed_template_specialization(self, line):
        """
        识别遗漏的模板特化

        特征：
        - 包含std::或__gnu_cxx::
        - 是成员函数
        - 但没有被Day 2清理捕获
        """
        line = line.strip()

        if not ('std::' in line or '__gnu_cxx::' in line):
            return False

        # 特定遗漏的模式
        # __uninitialized_copy 系列
        if '__uninitialized_copy' in line and '::' in line:
            return True

        # __copy 系列
        if re.search(r'std::__copy[_a-z]*<', line):
            return True

        # __move 系列
        if re.search(r'std::__move[_a-z]*<', line):
            return True

        # __advance 系列
        if re.search(r'std::__advance[_a-z]*<', line):
            return True

        # __iterator 系列
        if re.search(r'std::__iterator[_a-z]*<', line):
            return True

        return False

    def _is_missed_scope_error(self, line):
        """
        识别遗漏的作用域错误

        特征：
        - 返回类型 __cdecl std::function
        - 这些函数应该在std命名空间内
        """
        line = line.strip()

        # 特征：返回类型 __cdecl std::__function_name
        if re.search(r'^\w+\s+__cdecl\s+std::__\w+', line):
            return True

        # 特征：_DWORD* __userpurge std::__function
        if re.search(r'__userpurge\s+std::__\w+', line):
            return True

        # 特征：char* std::__copy_move_a
        if re.search(r'^\w+\s*\*?\s+std::__[a-z_]+\(', line):
            return True

        return False

    def _is_other_stl_error(self, line):
        """识别其他STL错误模式"""
        line = line.strip()

        # std::advance 错误声明
        if re.search(r'^\w+\s*\*?\s+std::advance\(', line):
            return True

        # std::distance 错误声明
        if re.search(r'^\w+\s*\*?\s+std::distance\(', line):
            return True

        # std::forward 错误声明（如果还有遗漏）
        if 'std::forward' in line and not line.startswith('namespace'):
            if re.search(r'^\w+\s+std::forward', line):
                return True

        return False

    def fix_file(self, input_file, output_file, dry_run=False):
        """执行清理"""
        print(f"\n[*] {'DRY RUN: ' if dry_run else ''}Cleaning remaining STL errors...")

        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        cleaned_lines = []
        for line_num, line in enumerate(lines, 1):
            if line_num in self.lines_to_comment:
                reason = self.comment_reasons[line_num]
                if not line.strip().startswith('//'):
                    cleaned_line = f"// [REMOVED - STL {reason}] {line}"
                else:
                    cleaned_line = line
                cleaned_lines.append(cleaned_line)
            else:
                cleaned_lines.append(line)

        if not dry_run:
            # 备份
            backup_file = Path(input_file).with_suffix('.h.before_round3')
            print(f"[*] Creating backup: {backup_file}")
            with open(backup_file, 'w', encoding='utf-8') as f:
                f.writelines(lines)

            # 写入
            print(f"[*] Writing cleaned file: {output_file}")
            with open(output_file, 'w', encoding='utf-8') as f:
                f.writelines(cleaned_lines)

            print(f"[OK] File cleaned")
        else:
            print(f"[*] DRY RUN: Would clean {len(self.lines_to_comment)} lines")

        return len(self.lines_to_comment)

    def generate_report(self, output_file):
        """生成报告"""
        print(f"\n[*] Generating report: {output_file}")

        report = []
        report.append("# 第三轮STL清理报告\n")
        report.append(f"**清理行数：** {len(self.lines_to_comment)}\n")
        report.append("\n## 清理统计\n")
        report.append(f"- 遗漏的模板特化：{self.stats['template_specialization_missed']}")
        report.append(f"- 遗漏的作用域错误：{self.stats['wrong_scope_missed']}")
        report.append(f"- 其他STL错误：{self.stats['other_stl_errors']}")
        report.append(f"- **总计：** {len(self.lines_to_comment)}\n")

        report.append("\n## 预期效果\n")
        report.append("根据编译A的错误分析：")
        report.append("- 模板特化错误：~408个")
        report.append("- 作用域错误：~372个")
        report.append("- 预期削减：~800-1,500个错误\n")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(report))

        print(f"[OK] Report generated")


def main():
    parser = argparse.ArgumentParser(description='Clean remaining STL errors')
    parser.add_argument('--input', required=True)
    parser.add_argument('--output', help='Output (default: same as input)')
    parser.add_argument('--dry-run', action='store_true')
    parser.add_argument('--report', default='logs/round3_stl_cleanup.md')

    args = parser.parse_args()

    if not Path(args.input).exists():
        print(f"[ERROR] File not found: {args.input}")
        sys.exit(1)

    if not args.output:
        args.output = args.input

    print("=" * 60)
    print("Round 3: Remaining STL Cleanup v1.0")
    print("=" * 60)

    cleaner = RemainingSTLCleaner()
    num_lines = cleaner.analyze_file(args.input)

    if num_lines == 0:
        print("[*] No remaining STL errors found!")
        sys.exit(0)

    print("\n" + "=" * 60)
    print("Cleanup Statistics:")
    print("=" * 60)
    print(f"Template specialization: {cleaner.stats['template_specialization_missed']}")
    print(f"Wrong scope:             {cleaner.stats['wrong_scope_missed']}")
    print(f"Other STL errors:        {cleaner.stats['other_stl_errors']}")
    print(f"Total lines:             {num_lines}")
    print("=" * 60)

    if not args.dry_run:
        cleaner.fix_file(args.input, args.output, dry_run=False)
    else:
        print("\n[*] DRY RUN - No files modified")

    Path(args.report).parent.mkdir(parents=True, exist_ok=True)
    cleaner.generate_report(args.report)

    print("\n" + "=" * 60)
    print("[OK] Round 3 cleanup complete!")
    print("=" * 60)


if __name__ == '__main__':
    main()
