#!/usr/bin/env python3
"""
Day 3 编译监控脚本
持续监控编译进度，实时报告错误统计
"""

import sys
import os
import time
import re
from datetime import datetime
from collections import Counter, defaultdict

class CompilationMonitor:
    def __init__(self, log_file, container_id=None, threshold=15000, interval=30):
        self.log_file = log_file
        self.container_id = container_id
        self.threshold = threshold
        self.interval = interval
        self.last_position = 0
        self.error_history = []
        self.start_time = time.time()
        self.compilation_started = False

    def read_new_lines(self):
        """读取日志文件新行"""
        try:
            with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
                f.seek(self.last_position)
                lines = f.readlines()
                self.last_position = f.tell()
                return lines
        except FileNotFoundError:
            return []

    def extract_errors(self, lines):
        """从行中提取错误"""
        errors = []
        for line in lines:
            if 'error:' in line:
                # 提取文件名、行号和错误消息
                match = re.search(r'^([^:]+):(\d+):\d+: error: (.+)$', line)
                if match:
                    filename = os.path.basename(match.group(1))
                    lineno = match.group(2)
                    message = match.group(3).strip()
                    errors.append({
                        'file': filename,
                        'line': lineno,
                        'message': message
                    })
        return errors

    def analyze_errors(self, errors):
        """分析错误模式"""
        if not errors:
            return {}

        # 统计错误类型
        error_types = Counter()
        file_errors = defaultdict(int)

        for err in errors:
            # 简化错误消息以识别类型
            msg = err['message']
            if "expected" in msg and "{" in msg:
                error_types['expected_brace'] += 1
            elif "undeclared identifier" in msg:
                error_types['undeclared'] += 1
            elif "expected ';'" in msg:
                error_types['expected_semicolon'] += 1
            elif "no matching function" in msg:
                error_types['no_matching_func'] += 1
            elif "invalid operands" in msg:
                error_types['invalid_operands'] += 1
            elif "no type named" in msg:
                error_types['no_type'] += 1
            else:
                error_types['other'] += 1

            file_errors[err['file']] += 1

        return {
            'types': error_types,
            'files': file_errors,
            'total': len(errors)
        }

    def get_compilation_status(self):
        """获取编译状态"""
        status = {
            'started': False,
            'running': False,
            'complete': False,
            'progress': 0
        }

        try:
            with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

                # 检查编译是否开始
                if 'g++' in content or 'gcc' in content or 'make' in content:
                    status['started'] = True

                # 估计进度 (基于行数)
                line_count = len(content.split('\n'))
                status['progress'] = min(100, (line_count // 100))

                # 检查是否完成
                if any(x in content for x in ['make[', 'built successfully', 'All tests passed']):
                    status['complete'] = True
                elif content.count('error:') > 0:
                    status['running'] = True
        except:
            pass

        return status

    def monitor(self):
        """主监控循环"""
        print("=" * 70)
        print("编译监控 - 开始")
        print("=" * 70)
        print(f"日志文件: {self.log_file}")
        print(f"阈值: {self.threshold} 错误")
        print(f"检查间隔: {self.interval} 秒")
        print("")

        iteration = 0
        last_error_count = 0
        stall_counter = 0

        try:
            while True:
                iteration += 1
                current_time = datetime.now().strftime('%H:%M:%S')

                # 读取新行
                new_lines = self.read_new_lines()

                if new_lines:
                    # 提取错误
                    new_errors = self.extract_errors(new_lines)
                    self.error_history.extend(new_errors)
                    stall_counter = 0

                    # 分析错误
                    analysis = self.analyze_errors(self.error_history)
                    error_count = len(self.error_history)

                    # 输出进度
                    elapsed = int(time.time() - self.start_time)
                    print(f"[{current_time}] 迭代 #{iteration} | 已用时: {elapsed:3d}s | 错误数: {error_count:5d}", end="")

                    if error_count > last_error_count:
                        delta = error_count - last_error_count
                        print(f" (+{delta:3d})", end="")

                    # 评估
                    if error_count > self.threshold:
                        print(f" ⚠ 超过阈值!", end="")
                    elif error_count > self.threshold * 0.9:
                        print(f" ~ 接近阈值", end="")

                    print()

                    # 显示Top 3错误类型
                    if analysis['types']:
                        top_errors = list(analysis['types'].items())[:3]
                        for error_type, count in top_errors:
                            pct = count / error_count * 100 if error_count > 0 else 0
                            print(f"       {error_type:20s}: {count:4d} ({pct:5.1f}%)")

                    last_error_count = error_count

                else:
                    stall_counter += 1
                    status = self.get_compilation_status()

                    if status['complete']:
                        print(f"[{current_time}] ✓ 编译完成")
                        break
                    elif stall_counter <= 3:
                        print(f"[{current_time}] 等待编译... (已等待 {stall_counter * self.interval}s)")
                    else:
                        # 检查文件是否被修改
                        try:
                            mtime = os.path.getmtime(self.log_file)
                            age = time.time() - mtime
                            if age > 300:  # 5分钟未更新
                                print(f"[{current_time}] ⚠ 日志文件5分钟未更新，可能编译已停止")
                                break
                        except:
                            pass

                # 检查容器状态 (如果使用Docker)
                if self.container_id and iteration % 10 == 0:
                    self._check_container_status()

                # 等待下一个检查周期
                time.sleep(self.interval)

        except KeyboardInterrupt:
            print("\n\n监控已停止")

        # 生成最终报告
        self._generate_final_report()

    def _check_container_status(self):
        """检查Docker容器状态"""
        try:
            import subprocess
            result = subprocess.run(
                ['docker', 'ps', '--filter', f'id={self.container_id}'],
                capture_output=True,
                timeout=5
            )
            if self.container_id not in result.stdout.decode():
                print("⚠ Docker容器已停止")
        except:
            pass

    def _generate_final_report(self):
        """生成最终报告"""
        print("\n" + "=" * 70)
        print("最终统计")
        print("=" * 70)

        error_count = len(self.error_history)
        elapsed = int(time.time() - self.start_time)

        print(f"\n总错误数: {error_count:,}")
        print(f"运行时间: {elapsed} 秒 (~{elapsed // 60} 分钟)")

        # 错误分布
        analysis = self.analyze_errors(self.error_history)
        if analysis['types']:
            print("\n错误类型分布:")
            for error_type, count in analysis['types'].most_common():
                pct = count / error_count * 100 if error_count > 0 else 0
                bar = "█" * int(pct / 2)
                print(f"  {error_type:20s}: {count:5d} ({pct:5.1f}%) {bar}")

        # 文件错误分布 (Top 5)
        if analysis['files']:
            print("\nTop 5 错误文件:")
            for filename, count in sorted(analysis['files'].items(),
                                         key=lambda x: x[1], reverse=True)[:5]:
                pct = count / error_count * 100 if error_count > 0 else 0
                print(f"  {filename:30s}: {count:5d} 错误")

        # 与阈值对比
        print(f"\n与阈值对比:")
        print(f"  设定阈值: {self.threshold:,} 错误")
        if error_count <= self.threshold:
            reduction = self.threshold - error_count
            pct = reduction / self.threshold * 100
            print(f"  ✓ 低于阈值 ({reduction:,} 错误)")
        else:
            excess = error_count - self.threshold
            pct = excess / self.threshold * 100
            print(f"  ⚠ 超出阈值 (+{excess:,} 错误)")

        print("\n" + "=" * 70 + "\n")

def main():
    if len(sys.argv) < 2:
        print("使用: monitor_compilation.py <log_file> [--container ID] [--threshold N] [--interval S]")
        sys.exit(1)

    log_file = sys.argv[1]
    container_id = None
    threshold = 15000
    interval = 30

    # 解析参数
    i = 2
    while i < len(sys.argv):
        if sys.argv[i] == '--container':
            container_id = sys.argv[i + 1]
            i += 2
        elif sys.argv[i] == '--threshold':
            threshold = int(sys.argv[i + 1])
            i += 2
        elif sys.argv[i] == '--interval':
            interval = int(sys.argv[i + 1])
            i += 2
        else:
            i += 1

    # 启动监控
    monitor = CompilationMonitor(log_file, container_id, threshold, interval)
    monitor.monitor()

if __name__ == '__main__':
    main()
