#!/usr/bin/env python3
"""
Day 3 最终结果分析脚本
深入分析编译日志，生成详细报告和建议
"""

import sys
import re
import os
from collections import Counter, defaultdict
from datetime import datetime

class FinalResultAnalyzer:
    def __init__(self, log_file):
        self.log_file = log_file
        self.errors = []
        self.warnings = []
        self.build_log = []

        self._load_log()

    def _load_log(self):
        """加载编译日志"""
        try:
            with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
                self.build_log = f.readlines()
        except FileNotFoundError:
            print(f"错误: 找不到日志文件 {self.log_file}")
            sys.exit(1)

    def extract_errors(self):
        """提取所有错误"""
        for line in self.build_log:
            if 'error:' in line:
                match = re.search(
                    r'^([^:]+):(\d+):(\d+): error: (.+)$',
                    line
                )
                if match:
                    self.errors.append({
                        'file': match.group(1),
                        'line': int(match.group(2)),
                        'col': int(match.group(3)),
                        'message': match.group(4).strip(),
                        'raw': line.strip()
                    })

    def extract_warnings(self):
        """提取所有警告"""
        for line in self.build_log:
            if 'warning:' in line:
                match = re.search(
                    r'^([^:]+):(\d+):(\d+): warning: (.+)$',
                    line
                )
                if match:
                    self.warnings.append({
                        'file': match.group(1),
                        'line': int(match.group(2)),
                        'col': int(match.group(3)),
                        'message': match.group(4).strip()
                    })

    def categorize_errors(self):
        """分类错误"""
        categories = defaultdict(list)

        for err in self.errors:
            msg = err['message']

            # 语法错误
            if any(x in msg for x in ['expected', 'unexpected token', 'expected \';\'', 'expected \'{\'', 'expected \'}\'', 'expected \'(\'', 'expected \')\'', 'expected \'[\'', 'expected \']\'', 'expected \'<\'', 'expected \'>\'', 'expected \':\'', 'expected \',\'']):
                categories['syntax_error'].append(err)
            # 类型错误
            elif any(x in msg for x in ['undeclared identifier', 'no type named', 'unknown type', 'incomplete type', 'invalid operands to binary operator', 'cannot convert']):
                categories['type_error'].append(err)
            # 函数错误
            elif any(x in msg for x in ['no matching function', 'too many arguments', 'too few arguments', 'redefinition of']):
                categories['function_error'].append(err)
            # 初始化错误
            elif any(x in msg for x in ['expected \'=\'', 'initializer', 'union']):
                categories['init_error'].append(err)
            # STL相关
            elif any(x in msg for x in ['std::', 'vector', 'map', 'list', 'queue']):
                categories['stl_error'].append(err)
            # 其他
            else:
                categories['other'].append(err)

        return categories

    def analyze_error_distribution(self):
        """分析错误分布"""
        file_errors = defaultdict(int)
        part_errors = defaultdict(int)

        for err in self.errors:
            filename = os.path.basename(err['file'])
            file_errors[filename] += 1

            # 按part分类
            if 'df_game_r_part' in filename:
                match = re.search(r'part_(\d+)', filename)
                if match:
                    part_num = int(match.group(1))
                    part_errors[f"part_{part_num:02d}"] += 1

        return file_errors, part_errors

    def calculate_metrics(self):
        """计算关键指标"""
        metrics = {
            'total_errors': len(self.errors),
            'total_warnings': len(self.warnings),
            'unique_errors': len(set(e['message'] for e in self.errors)),
            'files_with_errors': len(set(os.path.basename(e['file']) for e in self.errors))
        }

        # 计算与Day 2的对比
        day2_errors = 17772
        day2_round_b = 17772

        metrics['day2_errors'] = day2_errors
        metrics['reduction'] = day2_errors - metrics['total_errors']
        metrics['reduction_pct'] = (metrics['reduction'] / day2_errors * 100) if day2_errors > 0 else 0

        return metrics

    def identify_blocking_errors(self):
        """识别阻塞性错误 (重复/关键)"""
        error_counts = Counter(e['message'] for e in self.errors)
        blocking = [
            (msg, count) for msg, count in error_counts.most_common(20)
        ]
        return blocking

    def generate_report(self):
        """生成完整报告"""
        print("=" * 80)
        print("Day 3 Phase 5 - 最终编译结果分析")
        print("=" * 80)
        print(f"分析时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"日志文件: {self.log_file}")
        print("")

        # ================================================================
        # 第1部分: 快速总结
        # ================================================================
        print("【1】快速总结")
        print("-" * 80)

        self.extract_errors()
        self.extract_warnings()
        metrics = self.calculate_metrics()

        print(f"总错误数: {metrics['total_errors']:,}")
        print(f"总警告数: {metrics['total_warnings']:,}")
        print(f"错误文件数: {metrics['files_with_errors']}")
        print(f"唯一错误类型: {metrics['unique_errors']}")
        print("")

        # ================================================================
        # 第2部分: 与Day 2对比
        # ================================================================
        print("【2】与Day 2对比")
        print("-" * 80)

        print(f"Day 2 (编译B): {metrics['day2_errors']:,} 错误")
        print(f"Day 3 (Phase 5): {metrics['total_errors']:,} 错误")
        print(f"削减错误: {metrics['reduction']:,} ({metrics['reduction_pct']:.1f}%)")
        print("")

        # 评估
        if metrics['total_errors'] < 12000:
            print("✓ 达到Week 1调整目标 (<12,000)")
            status = "EXCELLENT"
        elif metrics['total_errors'] < 15000:
            print("~ 接近目标 (<15,000)")
            status = "GOOD"
        elif metrics['total_errors'] < 17772:
            print("^ 有改进 (<17,772)")
            status = "IMPROVED"
        else:
            print("⚠ 未改进")
            status = "STALLED"

        print(f"状态: {status}")
        print("")

        # ================================================================
        # 第3部分: 错误分类
        # ================================================================
        print("【3】错误分类")
        print("-" * 80)

        categories = self.categorize_errors()
        for category, errors in sorted(categories.items(), key=lambda x: len(x[1]), reverse=True):
            count = len(errors)
            pct = count / metrics['total_errors'] * 100 if metrics['total_errors'] > 0 else 0
            bar = "█" * int(pct / 2)
            print(f"{category:20s}: {count:5d} ({pct:5.1f}%) {bar}")

        print("")

        # ================================================================
        # 第4部分: 最常见的错误
        # ================================================================
        print("【4】Top 15 最常见的错误")
        print("-" * 80)

        blocking = self.identify_blocking_errors()
        for i, (msg, count) in enumerate(blocking[:15], 1):
            pct = count / metrics['total_errors'] * 100
            print(f"{i:2d}. [{count:5d}, {pct:5.1f}%] {msg[:70]}")

        print("")

        # ================================================================
        # 第5部分: 错误文件分布
        # ================================================================
        print("【5】错误最多的文件")
        print("-" * 80)

        file_errors, part_errors = self.analyze_error_distribution()

        top_files = sorted(file_errors.items(), key=lambda x: x[1], reverse=True)[:10]
        for filename, count in top_files:
            pct = count / metrics['total_errors'] * 100
            print(f"{filename:35s}: {count:5d} 错误 ({pct:5.1f}%)")

        print("")

        # ================================================================
        # 第6部分: Part分布
        # ================================================================
        print("【6】Part文件错误分布")
        print("-" * 80)

        if part_errors:
            sorted_parts = sorted(part_errors.items(), key=lambda x: x[1], reverse=True)
            for part, count in sorted_parts[:10]:
                pct = count / metrics['total_errors'] * 100 if metrics['total_errors'] > 0 else 0
                print(f"{part}: {count:5d} 错误 ({pct:5.1f}%)")
        else:
            print("未找到part文件错误")

        print("")

        # ================================================================
        # 第7部分: 建议和下一步
        # ================================================================
        print("【7】建议和下一步")
        print("-" * 80)

        if metrics['total_errors'] < 5000:
            print("✓ 优秀成绩！")
            print("  建议: 继续修复语法错误，准备编译测试")
        elif metrics['total_errors'] < 12000:
            print("✓ 良好进度")
            print("  建议: 优先修复Top 5错误类型")
            print("  估计: 再削减50-60%错误可达编译目标")
        elif metrics['total_errors'] < 17772:
            print("~ 有进步，但需要加强")
            print("  建议: 集中修复主要错误类型")
            print("  关键: 修复STL/类型相关错误")
        else:
            print("⚠ 修复效果不理想")
            print("  建议: 重新评估修复策略")
            print("  可考虑: 回滚+不同方法，或接受当前进度")

        print("")

        # ================================================================
        # 第8部分: 详细建议
        # ================================================================
        print("【8】详细修复建议")
        print("-" * 80)

        # 根据最常见的错误提供建议
        top_error = blocking[0][0] if blocking else "未知"

        suggestions = {
            'expected': '修复: 检查括号、花括号、分号的匹配',
            'undeclared identifier': '修复: 添加前向声明或类型定义',
            'no type named': '修复: 检查类型名称拼写，添加命名空间',
            'no matching function': '修复: 检查函数重载，参数类型',
            'union': '修复: 检查union定义，可能需要结构化处理',
            'unexpected token': '修复: 检查代码结构，可能是多余字符',
            'invalid operands': '修复: 检查操作符重载或类型转换',
            'cannot convert': '修复: 添加显式类型转换'
        }

        print("基于Top 1错误的修复建议:")
        print(f"  错误类型: {top_error}")
        for key, suggestion in suggestions.items():
            if key.lower() in top_error.lower():
                print(f"  {suggestion}")
                break

        print("")
        print("通用修复优先级:")
        print("  1. 语法错误 (括号、分号) → 最快解决")
        print("  2. 类型错误 (undeclared, no type) → 需要添加定义")
        print("  3. 函数错误 (no matching) → 可能需要重构")
        print("  4. STL错误 → 可能需要移除或适配")
        print("")

        # ================================================================
        # 第9部分: 回滚决策
        # ================================================================
        print("【9】回滚决策")
        print("-" * 80)

        if metrics['total_errors'] > 17772:
            print("⚠ 当前修复反而增加了错误")
            print("  建议: 考虑回滚Phase 4修复")
            print("  命令: bash scripts/rollback_phase4.sh")
        elif metrics['reduction_pct'] > 30:
            print("✓ 修复效果显著")
            print("  建议: 继续当前策略")
        else:
            print("~ 修复效果一般")
            print("  建议: 调整修复策略或接受当前进度")

        print("")

        # ================================================================
        # 总结
        # ================================================================
        print("=" * 80)
        print("分析完成")
        print("=" * 80)
        print(f"最终状态: {status}")
        print(f"详细日志: {self.log_file}")
        print("")

    def export_error_list(self, output_file=None):
        """导出错误列表"""
        if not output_file:
            output_file = self.log_file.replace('.log', '_errors.txt')

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("Day 3 Phase 5 - 错误详单\n")
            f.write(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"总错误数: {len(self.errors)}\n")
            f.write("=" * 80 + "\n\n")

            for i, err in enumerate(self.errors[:100], 1):
                f.write(f"{i:4d}. {os.path.basename(err['file'])}:{err['line']}\n")
                f.write(f"      {err['message']}\n\n")

            if len(self.errors) > 100:
                f.write(f"\n... 还有 {len(self.errors) - 100} 个错误\n")

        print(f"✓ 错误列表已导出: {output_file}")

def main():
    if len(sys.argv) < 2:
        print("使用: analyze_final_results.py <log_file> [--export]")
        sys.exit(1)

    log_file = sys.argv[1]
    export = '--export' in sys.argv

    analyzer = FinalResultAnalyzer(log_file)
    analyzer.generate_report()

    if export:
        analyzer.export_error_list()

if __name__ == '__main__':
    main()
