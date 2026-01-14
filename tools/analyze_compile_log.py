#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
编译日志分析工具
用途: 分析大规模编译日志，提取错误统计和修复建议
作者: Claude AI
日期: 2025-10-28
"""

import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

class CompileLogAnalyzer:
    """编译日志分析器"""

    def __init__(self, log_file):
        self.log_file = Path(log_file)
        self.errors = []
        self.warnings = []
        self.error_types = Counter()
        self.error_locations = defaultdict(list)
        self.file_errors = Counter()

    def parse_log(self):
        """解析日志文件"""
        print(f"[*] 正在读取日志: {self.log_file.name}")
        print(f"[*] 文件大小: {self.log_file.stat().st_size / 1024 / 1024:.2f} MB")

        error_pattern = re.compile(r'^(.+?):(\d+):(\d+): error: (.+)$')
        warning_pattern = re.compile(r'^(.+?):(\d+):(\d+): warning: (.+)$')

        line_count = 0
        with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                line_count += 1
                if line_count % 50000 == 0:
                    print(f"  处理进度: {line_count:,} 行...")

                # 匹配错误
                match = error_pattern.match(line.strip())
                if match:
                    file_path, line_no, col, msg = match.groups()
                    self.errors.append({
                        'file': file_path,
                        'line': int(line_no),
                        'col': int(col),
                        'msg': msg
                    })
                    self.error_types[msg] += 1
                    self.error_locations[file_path].append(int(line_no))
                    self.file_errors[file_path] += 1
                    continue

                # 匹配警告
                match = warning_pattern.match(line.strip())
                if match:
                    file_path, line_no, col, msg = match.groups()
                    self.warnings.append({
                        'file': file_path,
                        'line': int(line_no),
                        'col': int(col),
                        'msg': msg
                    })

        print(f"[OK] 解析完成: {line_count:,} 行")
        print(f"[ERROR] 错误数量: {len(self.errors):,}")
        print(f"[WARN] 警告数量: {len(self.warnings):,}")

    def categorize_errors(self):
        """错误分类"""
        categories = {
            'STL模板错误': [],
            '未声明类型': [],
            '模板参数错误': [],
            'Boost库错误': [],
            '语法错误': [],
            '其他错误': []
        }

        for msg, count in self.error_types.items():
            if 'std::' in msg or 'template' in msg.lower():
                categories['STL模板错误'].append((msg, count))
            elif 'was not declared' in msg or 'incomplete type' in msg:
                categories['未声明类型'].append((msg, count))
            elif 'template argument' in msg:
                categories['模板参数错误'].append((msg, count))
            elif 'boost' in msg.lower():
                categories['Boost库错误'].append((msg, count))
            elif any(kw in msg for kw in ['expected', 'parse error', 'syntax']):
                categories['语法错误'].append((msg, count))
            else:
                categories['其他错误'].append((msg, count))

        return categories

    def generate_report(self, output_file='compile_analysis_report.md'):
        """生成分析报告"""
        print(f"\n[*] 生成分析报告: {output_file}")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("# 编译日志分析报告\n\n")
            f.write(f"**日志文件**: `{self.log_file.name}`\n")
            f.write(f"**分析时间**: {self._get_timestamp()}\n\n")

            # 总体统计
            f.write("## 📊 总体统计\n\n")
            f.write(f"- **错误总数**: {len(self.errors):,}\n")
            f.write(f"- **警告总数**: {len(self.warnings):,}\n")
            f.write(f"- **涉及文件数**: {len(self.file_errors)}\n")
            f.write(f"- **唯一错误类型**: {len(self.error_types)}\n\n")

            # 错误分类
            f.write("## 🗂️ 错误分类\n\n")
            categories = self.categorize_errors()
            for cat_name, errors in categories.items():
                if errors:
                    total = sum(count for _, count in errors)
                    percentage = (total / len(self.errors)) * 100
                    f.write(f"### {cat_name} ({total:,} 个, {percentage:.1f}%)\n\n")
                    f.write("| 错误信息 | 出现次数 |\n")
                    f.write("|---------|----------|\n")
                    for msg, count in sorted(errors, key=lambda x: x[1], reverse=True)[:10]:
                        f.write(f"| {msg[:80]}... | {count:,} |\n")
                    f.write("\n")

            # Top 20 错误
            f.write("## 🔝 Top 20 最频繁错误\n\n")
            f.write("| 排名 | 错误信息 | 出现次数 |\n")
            f.write("|------|---------|----------|\n")
            for i, (msg, count) in enumerate(self.error_types.most_common(20), 1):
                f.write(f"| {i} | {msg[:60]}... | {count:,} |\n")
            f.write("\n")

            # 错误分布 (按文件)
            f.write("## 📂 错误分布 (按文件)\n\n")
            f.write("| 文件 | 错误数量 |\n")
            f.write("|------|----------|\n")
            for file, count in self.file_errors.most_common(20):
                f.write(f"| {Path(file).name} | {count:,} |\n")
            f.write("\n")

            # 修复建议
            f.write("## 🔧 修复建议\n\n")
            self._write_fix_suggestions(f, categories)

        print(f"[OK] 报告已生成: {output_file}")

    def _write_fix_suggestions(self, f, categories):
        """编写修复建议"""
        f.write("### 1. STL模板错误\n\n")
        if categories['STL模板错误']:
            f.write("**根本原因**: Phase 2.5 注释了STL内部函数声明\n\n")
            f.write("**修复方案**:\n")
            f.write("```bash\n")
            f.write("# 恢复 df_game_r_decl.h 中的STL声明\n")
            f.write("sed -i 's|^// \\(.*std::_.*\\)|\\1|' df_game_r_decl.h\n")
            f.write("sed -i 's|^// \\(.*template<>.*\\)|\\1|' df_game_r_decl.h\n")
            f.write("```\n\n")

        f.write("### 2. 未声明类型错误\n\n")
        if categories['未声明类型']:
            f.write("**根本原因**: 类型定义被注释或缺失\n\n")
            f.write("**修复方案**:\n")
            f.write("- 分析日志中的未声明类型列表\n")
            f.write("- 在 df_game_r_decl.h 中搜索这些类型\n")
            f.write("- 如果存在但被注释，恢复它们\n")
            f.write("- 如果不存在，需要从其他源码文件提取定义\n\n")

        f.write("### 3. Boost库错误\n\n")
        if categories['Boost库错误']:
            f.write("**根本原因**: Boost库声明被注释\n\n")
            f.write("**修复方案**:\n")
            f.write("```bash\n")
            f.write("# 恢复 Boost 相关声明\n")
            f.write("sed -i 's|^// \\(.*boost::.*\\)|\\1|' df_game_r_decl.h\n")
            f.write("```\n\n")

        f.write("### 修复优先级\n\n")
        f.write("1. **高优先级**: STL模板错误 (数量最多)\n")
        f.write("2. **中优先级**: 未声明类型\n")
        f.write("3. **低优先级**: Boost库错误\n\n")

    def extract_undeclared_types(self, output_file='undeclared_types.txt'):
        """提取所有未声明的类型"""
        print(f"\n[*] 提取未声明类型: {output_file}")

        undeclared_pattern = re.compile(r"'(.+?)' (?:was not declared|incomplete type)")
        undeclared_types = Counter()

        for error in self.errors:
            match = undeclared_pattern.search(error['msg'])
            if match:
                type_name = match.group(1)
                undeclared_types[type_name] += 1

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("# 未声明类型列表\n\n")
            f.write(f"总计: {len(undeclared_types)} 个唯一类型\n\n")
            for type_name, count in undeclared_types.most_common():
                f.write(f"{type_name}\t{count}\n")

        print(f"[OK] 已提取 {len(undeclared_types)} 个未声明类型")
        return undeclared_types

    def generate_fix_script(self, output_file='fix_compile_errors.sh'):
        """生成自动修复脚本"""
        print(f"\n[*] 生成修复脚本: {output_file}")

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("#!/bin/bash\n")
            f.write("# 编译错误自动修复脚本\n")
            f.write("# 生成时间: " + self._get_timestamp() + "\n\n")
            f.write("set -e\n\n")

            f.write("echo \"🔧 开始修复编译错误...\"\n\n")

            f.write("# 备份原文件\n")
            f.write("echo \"📦 备份 df_game_r_decl.h...\"\n")
            f.write("cp src_split/df_game_r_decl.h src_split/df_game_r_decl.h.backup_$(date +%Y%m%d_%H%M%S)\n\n")

            f.write("# 1. 恢复STL内部函数声明\n")
            f.write("echo \"🔄 恢复STL内部函数声明...\"\n")
            f.write("sed -i 's|^// \\(.*std::_.*\\)|\\1|' src_split/df_game_r_decl.h\n")
            f.write("sed -i 's|^// \\(.*std::basic_.*\\)|\\1|' src_split/df_game_r_decl.h\n\n")

            f.write("# 2. 恢复模板特化声明\n")
            f.write("echo \"🔄 恢复模板特化声明...\"\n")
            f.write("sed -i 's|^// \\(template<>.*\\)|\\1|' src_split/df_game_r_decl.h\n\n")

            f.write("# 3. 恢复Boost库声明\n")
            f.write("echo \"🔄 恢复Boost库声明...\"\n")
            f.write("sed -i 's|^// \\(.*boost::.*\\)|\\1|' src_split/df_game_r_decl.h\n\n")

            f.write("# 4. 统计恢复结果\n")
            f.write("echo \"📊 统计修复结果...\"\n")
            f.write("echo \"STL声明: $(grep -c 'std::_' src_split/df_game_r_decl.h || true)\"\n")
            f.write("echo \"模板特化: $(grep -c 'template<>' src_split/df_game_r_decl.h || true)\"\n")
            f.write("echo \"Boost声明: $(grep -c 'boost::' src_split/df_game_r_decl.h || true)\"\n\n")

            f.write("echo \"✅ 修复完成！\"\n")
            f.write("echo \"请运行: cd src_split && make clean && make -j12 all\"\n")

        import os
        try:
            os.chmod(output_file, 0o755)
        except:
            pass  # Windows may not support chmod
        print(f"[OK] 修复脚本已生成")

    @staticmethod
    def _get_timestamp():
        from datetime import datetime
        return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def main():
    """主函数"""
    # Windows console encoding fix
    import sys
    import io
    if sys.platform == 'win32':
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
        sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

    if len(sys.argv) < 2:
        print("用法: python analyze_compile_log.py <compile.log>")
        sys.exit(1)

    log_file = sys.argv[1]
    if not Path(log_file).exists():
        print(f"[ERROR] 日志文件不存在: {log_file}")
        sys.exit(1)

    print("=" * 60)
    print("编译日志分析工具")
    print("=" * 60)

    analyzer = CompileLogAnalyzer(log_file)

    # 解析日志
    analyzer.parse_log()

    # 生成报告
    analyzer.generate_report()

    # 提取未声明类型
    analyzer.extract_undeclared_types()

    # 生成修复脚本
    analyzer.generate_fix_script()

    print("\n" + "=" * 60)
    print("[OK] 分析完成!")
    print("=" * 60)
    print("\n生成的文件:")
    print("  1. compile_analysis_report.md - 详细分析报告")
    print("  2. undeclared_types.txt - 未声明类型列表")
    print("  3. fix_compile_errors.sh - 自动修复脚本")
    print("\n下一步:")
    print("  1. 阅读 compile_analysis_report.md")
    print("  2. 运行 ./fix_compile_errors.sh 或 bash fix_compile_errors.sh")
    print("  3. 重新编译验证")


if __name__ == '__main__':
    main()
