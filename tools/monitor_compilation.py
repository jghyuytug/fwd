#!/usr/bin/env python3
"""
编译监控工具 - 早期检测编译失败并自动终止

功能:
1. 监控日志文件增长
2. 检测错误数雪崩
3. 检测编译卡死
4. 检测目标文件生成失败
5. 自动终止无意义编译

用法:
    python tools/monitor_compilation.py logs/compile.log abc123 --threshold 20000
"""

import sys
import time
import os
import subprocess
import argparse
from pathlib import Path

class CompilationMonitor:
    def __init__(self, log_file, container_id, threshold=20000, check_interval=30):
        self.log_file = Path(log_file)
        self.container_id = container_id
        self.threshold = threshold
        self.check_interval = check_interval

        # 失败检测标准
        self.MAX_STALL_COUNT = 3       # 连续3次不增长 = 卡死
        self.ERROR_GROWTH_RATE = 1000  # 每30秒增长>1000 = 雪崩
        self.NO_OBJ_AFTER_MIN = 10     # 10分钟后无.o = 失败

        # 状态变量
        self.last_size = 0
        self.stall_count = 0
        self.last_error_count = 0
        self.start_time = time.time()

    def get_file_size(self):
        """获取日志文件大小（字节）"""
        try:
            return self.log_file.stat().st_size if self.log_file.exists() else 0
        except:
            return 0

    def count_errors(self):
        """统计错误数"""
        try:
            with open(self.log_file, 'r', encoding='utf-8', errors='ignore') as f:
                return sum(1 for line in f if 'error:' in line)
        except:
            return 0

    def count_obj_files(self):
        """统计容器内生成的.o文件数"""
        try:
            result = subprocess.run(
                ['docker', 'exec', self.container_id, 'ls', '/build/src_split/*.o'],
                capture_output=True, text=True, timeout=5
            )
            if result.returncode == 0:
                return len(result.stdout.strip().split('\n'))
            return 0
        except:
            return 0

    def is_container_running(self):
        """检查容器是否还在运行"""
        try:
            result = subprocess.run(
                ['docker', 'ps', '--format', '{{.ID}}'],
                capture_output=True, text=True, timeout=5
            )
            return self.container_id[:12] in result.stdout
        except:
            return False

    def stop_container(self):
        """停止容器"""
        print(f"\n[*] 正在停止容器 {self.container_id}...")
        try:
            subprocess.run(['docker', 'stop', self.container_id], timeout=30)
            print("[OK] 容器已停止")
            return True
        except Exception as e:
            print(f"[ERROR] 停止失败: {e}")
            return False

    def format_time(self, seconds):
        """格式化时间"""
        if seconds < 60:
            return f"{int(seconds)}秒"
        elif seconds < 3600:
            return f"{int(seconds/60)}分{int(seconds%60)}秒"
        else:
            return f"{int(seconds/3600)}时{int((seconds%3600)/60)}分"

    def check_stall(self, current_size):
        """检查1: 日志是否停止增长"""
        elapsed = time.time() - self.start_time
        size_mb = current_size / (1024 * 1024)

        if current_size == self.last_size:
            self.stall_count += 1
            print(f"[{time.strftime('%H:%M:%S')}] ⏸️  日志停止增长 ({self.stall_count}/{self.MAX_STALL_COUNT}) - {size_mb:.1f}MB")

            if self.stall_count >= self.MAX_STALL_COUNT:
                error_count = self.count_errors()
                obj_count = self.count_obj_files()

                print(f"\n❌ 检测到编译卡死（日志{self.MAX_STALL_COUNT}次检查无变化）")
                print(f"   - 最终错误数: {error_count:,}")
                print(f"   - 目标文件数: {obj_count}/24")
                print(f"   - 运行时长: {self.format_time(elapsed)}")
                return 'STALLED'
        else:
            self.stall_count = 0
            growth_kb = (current_size - self.last_size) / 1024
            print(f"[{time.strftime('%H:%M:%S')}] ✅ 编译进行中 - 日志{size_mb:.1f}MB (+{growth_kb:.0f}KB)")

        self.last_size = current_size
        return 'OK'

    def check_errors(self):
        """检查2: 错误数是否超标或雪崩"""
        error_count = self.count_errors()
        error_growth = error_count - self.last_error_count

        if error_count > 0:
            print(f"           错误数: {error_count:,} (+{error_growth:,})")

            # 2a: 错误数超过阈值
            if error_count > self.threshold:
                print(f"\n❌ 错误数超过阈值 ({error_count:,} > {self.threshold:,})")
                print("   - 这可能导致编译器效率极低")
                print("   - 继续编译可能无法生成目标文件")
                return 'THRESHOLD_EXCEEDED'

            # 2b: 错误增长过快（雪崩）
            if error_growth > self.ERROR_GROWTH_RATE and self.last_error_count > 0:
                print(f"           ⚠️  错误快速增长 (+{error_growth:,}/{self.check_interval}秒)")

        self.last_error_count = error_count
        return 'OK'

    def check_obj_files(self):
        """检查3: 是否生成了目标文件"""
        elapsed = time.time() - self.start_time
        elapsed_min = elapsed / 60

        if elapsed_min >= self.NO_OBJ_AFTER_MIN:
            obj_count = self.count_obj_files()

            if obj_count == 0:
                error_count = self.count_errors()
                print(f"\n❌ {self.NO_OBJ_AFTER_MIN}分钟后仍无目标文件生成")
                print(f"   - 错误数: {error_count:,}")
                print("   - 编译器可能已放弃生成.o文件")
                print("   - 只在输出错误信息")
                return 'NO_OBJ_FILES'
            else:
                print(f"           目标文件: {obj_count}/24")

        return 'OK'

    def print_final_stats(self):
        """打印最终统计"""
        error_count = self.count_errors()
        elapsed = time.time() - self.start_time
        size_mb = self.get_file_size() / (1024 * 1024)

        print("\n" + "=" * 50)
        print("编译完成统计")
        print("=" * 50)
        print(f"错误数: {error_count:,}")
        print(f"运行时长: {self.format_time(elapsed)}")
        print(f"日志大小: {size_mb:.1f}MB")
        print()

        if error_count == 0:
            print("🎉 编译成功！")
            return 0
        else:
            print(f"❌ 编译失败（{error_count:,} 错误）")
            return 1

    def run(self):
        """主监控循环"""
        print("=" * 50)
        print("编译监控启动")
        print("=" * 50)
        print(f"日志文件: {self.log_file}")
        print(f"容器ID: {self.container_id}")
        print(f"错误阈值: {self.threshold:,}")
        print(f"检查间隔: {self.check_interval}秒")
        print()

        while True:
            time.sleep(self.check_interval)

            # 检查日志文件是否存在
            if not self.log_file.exists():
                print(f"[{time.strftime('%H:%M:%S')}] ⚠️  日志文件不存在，等待...")
                continue

            # 检查1: 日志增长
            current_size = self.get_file_size()
            stall_result = self.check_stall(current_size)
            if stall_result == 'STALLED':
                print(f"\n🛑 建议操作: docker stop {self.container_id}")
                return 1

            # 检查2: 错误数
            error_result = self.check_errors()
            if error_result == 'THRESHOLD_EXCEEDED':
                print(f"\n🛑 建议操作: docker stop {self.container_id}")
                return 2

            # 检查3: 目标文件
            obj_result = self.check_obj_files()
            if obj_result == 'NO_OBJ_FILES':
                print(f"\n🛑 建议操作: docker stop {self.container_id}")
                return 3

            # 检查4: 容器状态
            if not self.is_container_running():
                print(f"\n✅ 容器已停止，编译完成")
                return self.print_final_stats()

            print()


def main():
    parser = argparse.ArgumentParser(description='监控编译过程并早期检测失败')
    parser.add_argument('log_file', help='编译日志文件路径')
    parser.add_argument('container_id', help='Docker容器ID')
    parser.add_argument('--threshold', type=int, default=20000,
                       help='错误数阈值（默认20000）')
    parser.add_argument('--interval', type=int, default=30,
                       help='检查间隔秒数（默认30）')
    parser.add_argument('--auto-stop', action='store_true',
                       help='检测到失败时自动停止容器')

    args = parser.parse_args()

    monitor = CompilationMonitor(
        args.log_file,
        args.container_id,
        threshold=args.threshold,
        check_interval=args.interval
    )

    try:
        exit_code = monitor.run()

        # 如果启用自动停止且检测到失败
        if args.auto_stop and exit_code > 0:
            monitor.stop_container()

        sys.exit(exit_code)

    except KeyboardInterrupt:
        print("\n\n[*] 监控已中断")
        sys.exit(0)


if __name__ == '__main__':
    main()
