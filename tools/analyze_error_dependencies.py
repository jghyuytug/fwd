#!/usr/bin/env python3
"""
错误依赖分析工具

功能 
1. 解析gcc编译日志
2. 识别错误类型和位置
3. 建立错误依赖关系图
4. 找出"根因错误"和"雪崩触发因子"

使用方法 
    python tools/analyze_error_dependencies.py logs/step03_enum_dedup_FINAL_20251030_093009.log
"""

import re
import sys
from collections import defaultdict, Counter
from pathlib import Path
import json

class ErrorAnalyzer:
    def __init__(self, log_file):
        self.log_file = log_file
        self.errors = []  # [(file, line, error_msg), ...]
        self.error_types = Counter()
        self.file_errors = defaultdict(list)
        self.error_graph = defaultdict(set)  # 依赖关系图

    def parse_log(self):
        """解析编译日志 提取所有错误"""
        print(f"  解析日志文件: {self.log_file}")

        with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                # 匹配错误行 file:line:col: error: message
                match = re.match(r'^(.+?):(\d+):(?:\d+:)?\s*error:\s*(.+)$', line.strip())
                if match:
                    file_path, line_num, error_msg = match.groups()

                    # 清理文件路径
                    file_path = file_path.replace('../', '').strip()

                    # 提取错误类型
                    error_type = self._extract_error_type(error_msg)

                    self.errors.append({
                        'file': file_path,
                        'line': int(line_num),
                        'message': error_msg,
                        'type': error_type
                    })

                    self.error_types[error_type] += 1
                    self.file_errors[file_path].append(error_msg)

        print(f"[OK] 解析完成: 找到 {len(self.errors)} 个错误")
        return len(self.errors)

    def _extract_error_type(self, error_msg):
        """提取错误类型 核心错误信息 """
        # 去掉具体的标识符名称 保留错误模式

        # 模式1: 'XXX' has not been declared
        if "has not been declared" in error_msg:
            return "TYPE_NOT_DECLARED"

        # 模式2: 'XXX' is not a member of 'YYY'
        if "is not a member of" in error_msg:
            return "MEMBER_NOT_FOUND"

        # 模式3: template argument X is invalid
        if re.search(r'template argument \d+ is invalid', error_msg):
            return "TEMPLATE_ARG_INVALID"

        # 模式4: 'XXX' does not name a type
        if "does not name a type" in error_msg:
            return "NOT_A_TYPE"

        # 模式5: redeclared as different kind of symbol
        if "redeclared as different kind of symbol" in error_msg:
            return "REDECLARATION_CONFLICT"

        # 模式6: expected initializer before
        if "expected initializer before" in error_msg:
            return "SYNTAX_INITIALIZER"

        # 模式7: specializing member requires 'template<>' syntax
        if "requires 'template<>' syntax" in error_msg:
            return "TEMPLATE_SPECIALIZATION"

        # 模式8: is not a template
        if "is not a template" in error_msg:
            return "NOT_A_TEMPLATE"

        # 模式9: using typedef-name after 'class'
        if "using typedef-name" in error_msg and "after 'class'" in error_msg:
            return "TYPEDEF_CLASS_CONFLICT"

        # 模式10: must be a nonstatic member function
        if "must be a nonstatic member function" in error_msg:
            return "MEMBER_FUNCTION_ERROR"

        # 模式11: invalid use of 'this'
        if "invalid use of 'this'" in error_msg:
            return "INVALID_THIS"

        # 模式12: should have been declared inside
        if "should have been declared inside" in error_msg:
            return "WRONG_SCOPE"

        # 默认 返回简化的错误信息
        return error_msg[:80]

    def analyze_error_dependencies(self):
        """分析错误依赖关系"""
        print("\n  分析错误依赖关系...")

        # 按文件分组错误
        file_groups = defaultdict(list)
        for error in self.errors:
            file_groups[error['file']].append(error)

        # 识别"触发文件" 头文件 
        header_files = [f for f in file_groups.keys() if f.startswith('include/')]
        source_files = [f for f in file_groups.keys() if not f.startswith('include/')]

        print(f"   - 头文件错误: {len(header_files)} 个文件")
        print(f"   - 源文件错误: {len(source_files)} 个文件")

        # 头文件中的错误更可能是"根因"
        root_candidates = []
        for header in header_files[:10]:  # 分析前10个头文件
            errors = file_groups[header]
            if len(errors) > 0:
                root_candidates.append({
                    'file': header,
                    'error_count': len(errors),
                    'error_types': Counter([e['type'] for e in errors])
                })

        return root_candidates

    def find_root_causes(self):
        """找出根因错误"""
        print("\n  识别根因错误...")

        # 策略 根因错误通常出现在 
        # 1. 头文件的早期位置
        # 2. 类型定义、前向声明处
        # 3. 错误类型是TYPE_NOT_DECLARED或NOT_A_TYPE

        root_errors = []
        for error in self.errors[:100]:  # 分析前100个错误
            if error['type'] in ['TYPE_NOT_DECLARED', 'NOT_A_TYPE', 'REDECLARATION_CONFLICT']:
                if error['file'].startswith('include/'):
                    root_errors.append(error)

        print(f"[OK] 找到 {len(root_errors)} 个潜在根因错误")
        return root_errors

    def find_avalanche_triggers(self):
        """找出雪崩触发因子"""
        print("\n[WARNING] 识别雪崩触发因子...")

        # 策略 雪崩触发因子的特征 
        # 1. 同一个错误信息大量重复
        # 2. 涉及模板实例化、STL容器
        # 3. 错误类型是TEMPLATE_ARG_INVALID或MEMBER_NOT_FOUND

        avalanche_triggers = []
        for error_type, count in self.error_types.most_common(20):
            if count > 100:  # 重复超过100次
                # 找第一个这种类型的错误
                for error in self.errors:
                    if error['type'] == error_type:
                        avalanche_triggers.append({
                            'type': error_type,
                            'count': count,
                            'first_occurrence': error
                        })
                        break

        print(f"[OK] 找到 {len(avalanche_triggers)} 个雪崩触发因子")
        return avalanche_triggers

    def generate_report(self, output_file):
        """生成分析报告"""
        print(f"\n  生成分析报告: {output_file}")

        root_causes = self.find_root_causes()
        avalanche_triggers = self.find_avalanche_triggers()
        root_candidates = self.analyze_error_dependencies()

        report = []
        report.append("# 错误依赖分析报告")
        report.append(f"\n**日期 ** {Path(self.log_file).stem}")
        report.append(f"**总错误数 ** {len(self.errors):,}")
        report.append("\n---\n")

        # 1. 错误类型统计
        report.append("## 1. 错误类型分布 (Top 20)\n")
        report.append("| 排名 | 错误类型 | 数量 | 占比 |")
        report.append("|------|---------|------|------|")

        total = len(self.errors)
        for idx, (error_type, count) in enumerate(self.error_types.most_common(20), 1):
            percentage = (count / total) * 100
            report.append(f"| {idx} | `{error_type}` | {count:,} | {percentage:.1f}% |")

        # 2. 根因错误
        report.append("\n---\n")
        report.append("## 2. 根因错误 (Root Causes)\n")
        report.append("**定义 ** 不被其他错误触发的原始错误 修复它们可能消除大量下游错误。\n")
        report.append("### 2.1 潜在根因错误列表\n")

        for idx, error in enumerate(root_causes[:10], 1):
            report.append(f"\n**Root #{idx}**")
            report.append(f"- **类型 ** `{error['type']}`")
            report.append(f"- **位置 ** `{error['file']}:{error['line']}`")
            report.append(f"- **错误 ** {error['message']}")

        # 3. 雪崩触发因子
        report.append("\n---\n")
        report.append("## 3. 雪崩触发因子 (Avalanche Triggers)\n")
        report.append("**定义 ** 触发大量重复错误的因子 通常涉及模板实例化失败。\n")
        report.append("### 3.1 高频错误 重复>100次 \n")
        report.append("| 排名 | 错误类型 | 重复次数 | 首次出现 |")
        report.append("|------|---------|---------|----------|")

        for idx, trigger in enumerate(avalanche_triggers, 1):
            first = trigger['first_occurrence']
            location = f"{first['file']}:{first['line']}"
            report.append(f"| {idx} | `{trigger['type']}` | {trigger['count']:,} | {location} |")

        # 4. 文件级错误分布
        report.append("\n---\n")
        report.append("## 4. 文件级错误分布 (Top 20)\n")
        report.append("| 排名 | 文件 | 错误数 |")
        report.append("|------|------|--------|")

        file_error_counts = [(f, len(errors)) for f, errors in self.file_errors.items()]
        file_error_counts.sort(key=lambda x: x[1], reverse=True)

        for idx, (file, count) in enumerate(file_error_counts[:20], 1):
            report.append(f"| {idx} | `{file}` | {count:,} |")

        # 5. 头文件根因分析
        report.append("\n---\n")
        report.append("## 5. 头文件根因候选\n")
        report.append("**分析 ** 这些头文件的错误最可能是触发因子\n")

        for idx, candidate in enumerate(root_candidates, 1):
            report.append(f"\n### 5.{idx} {candidate['file']}")
            report.append(f"- **错误数 ** {candidate['error_count']}")
            report.append(f"- **错误类型分布 **")
            for error_type, count in candidate['error_types'].most_common(5):
                report.append(f"  - `{error_type}`: {count}次")

        # 6. 修复优先级建议
        report.append("\n---\n")
        report.append("## 6. 修复优先级建议\n")
        report.append("### 6.1 高优先级 立即修复 \n")

        # 基于根因错误和雪崩触发因子 给出建议
        priority_types = set()
        for error in root_causes[:5]:
            priority_types.add(error['type'])
        for trigger in avalanche_triggers[:3]:
            priority_types.add(trigger['type'])

        report.append("**应该优先修复以下类型的错误 **\n")
        for idx, error_type in enumerate(sorted(priority_types), 1):
            count = self.error_types[error_type]
            report.append(f"{idx}. `{error_type}` - {count:,}个错误")

        report.append("\n### 6.2 修复策略\n")
        report.append("**根据依赖关系 推荐按以下顺序修复 **\n")
        report.append("1. **TYPE_NOT_DECLARED / NOT_A_TYPE** - 类型定义问题 根因 ")
        report.append("2. **REDECLARATION_CONFLICT** - namespace/class冲突 根因 ")
        report.append("3. **TEMPLATE_ARG_INVALID** - 模板参数问题 雪崩 ")
        report.append("4. **MEMBER_NOT_FOUND** - 成员访问问题 雪崩 ")
        report.append("5. **其他语法错误** - 独立问题")

        # 7. 下一步行动
        report.append("\n---\n")
        report.append("## 7. 下一步行动 (Week 1 Day 2)\n")
        report.append("### 7.1 立即行动\n")
        report.append("根据以上分析 建议执行以下操作 \n")

        # 找出具体的文件和行号
        action_items = []
        for error in root_causes[:5]:
            action_items.append(f"- 修复 `{error['file']}:{error['line']}` - {error['type']}")

        if action_items:
            report.append("**具体修复点 **\n")
            report.extend(action_items)

        report.append("\n### 7.2 批量清理计划\n")
        report.append("从分析看 以下类型的错误可能可以批量处理 \n")

        # 识别可批量处理的错误
        batch_candidates = []
        for error_type, count in self.error_types.most_common(20):
            if count > 200 and error_type in ['TEMPLATE_SPECIALIZATION', 'WRONG_SCOPE', 'INVALID_THIS']:
                batch_candidates.append(f"- `{error_type}` ({count:,}个) - 可能是IDA伪代码")

        if batch_candidates:
            report.extend(batch_candidates)
        else:
            report.append("- 需要进一步分析")

        # 写入报告
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(report))

        print(f"[OK] 报告已生成: {output_file}")

        return {
            'total_errors': len(self.errors),
            'root_causes': len(root_causes),
            'avalanche_triggers': len(avalanche_triggers),
            'top_error_types': list(self.error_types.most_common(10))
        }

    def export_json(self, output_file):
        """导出JSON格式数据供其他工具使用"""
        data = {
            'total_errors': len(self.errors),
            'error_types': dict(self.error_types),
            'errors': self.errors[:1000],  # 只导出前1000个
            'file_errors': {k: len(v) for k, v in self.file_errors.items()}
        }

        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

        print(f"[OK] JSON数据已导出: {output_file}")


def main():
    if len(sys.argv) < 2:
        print("用法: python analyze_error_dependencies.py <log_file>")
        print("示例: python analyze_error_dependencies.py logs/step03_enum_dedup_FINAL_20251030_093009.log")
        sys.exit(1)

    log_file = sys.argv[1]

    if not Path(log_file).exists():
        print(f"[ERROR] 错误: 日志文件不存在: {log_file}")
        sys.exit(1)

    print("=" * 60)
    print("错误依赖分析工具 v1.0")
    print("=" * 60)

    analyzer = ErrorAnalyzer(log_file)

    # 解析日志
    error_count = analyzer.parse_log()

    if error_count == 0:
        print("[ERROR] 没有找到任何错误 ")
        sys.exit(1)

    # 生成报告
    output_dir = Path('docs/analysis')
    output_dir.mkdir(parents=True, exist_ok=True)

    report_file = output_dir / 'ERROR_DEPENDENCY_ANALYSIS.md'
    json_file = output_dir / 'error_data.json'

    stats = analyzer.generate_report(report_file)
    analyzer.export_json(json_file)

    # 打印摘要
    print("\n" + "=" * 60)
    print("  分析摘要")
    print("=" * 60)
    print(f"总错误数:       {stats['total_errors']:,}")
    print(f"根因错误:       {stats['root_causes']}")
    print(f"雪崩触发因子:   {stats['avalanche_triggers']}")
    print(f"\nTop 5 错误类型:")
    for idx, (error_type, count) in enumerate(stats['top_error_types'][:5], 1):
        percentage = (count / stats['total_errors']) * 100
        print(f"  {idx}. {error_type}: {count:,} ({percentage:.1f}%)")
    print("\n" + "=" * 60)
    print(f"[OK] 完成! 查看报告: {report_file}")
    print("=" * 60)


if __name__ == '__main__':
    main()
