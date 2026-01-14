#!/usr/bin/env python3
"""
网络层代码提取工具
从 df_game_r_v3.c 中提取网络相关代码
支持模板类和命名空间
"""

import re
import sys
from pathlib import Path

class NetworkExtractor:
    def __init__(self, source_file):
        self.source_file = Path(source_file)
        self.output_dir = Path("src/network")
        self.include_dir = Path("include/network")

        # 目标类（包含模板和命名空间）
        self.target_patterns = [
            'CNetwork<[^>]+>',           # Template: CNetwork<...>
            'exchange_server::CSession',  # Namespace: exchange_server::CSession
            'PacketBuf',
            'InterfacePacketBuf',
            'CmmChannelProxy',
            'tagPacketHeader'
        ]

        # 简化的类名（用于文件命名）
        self.class_map = {
            'CNetwork': 'CNetwork',
            'exchange_server::CSession': 'CSession',
            'PacketBuf': 'PacketBuf',
            'InterfacePacketBuf': 'InterfacePacketBuf',
            'CmmChannelProxy': 'CmmChannelProxy',
            'tagPacketHeader': 'tagPacketHeader'
        }

        # 存储提取的代码（按简化名存储）
        self.declarations = {name: [] for name in self.class_map.values()}
        self.implementations = {name: [] for name in self.class_map.values()}
        self.structures = []

    def get_simple_name(self, full_name):
        """从完整类名提取简化名"""
        if 'CNetwork<' in full_name:
            return 'CNetwork'
        elif 'exchange_server::CSession' in full_name:
            return 'CSession'
        elif full_name in self.class_map.values():
            return full_name
        return full_name

    def extract(self):
        """主提取流程"""
        print(f"[*] 读取源文件: {self.source_file}")
        print(f"    文件大小: {self.source_file.stat().st_size / 1024 / 1024:.1f} MB")

        # 第一遍：提取函数声明
        print(f"\n[*] 第一遍：提取函数声明...")
        self.extract_declarations()

        # 第二遍：提取函数实现
        print(f"\n[*] 第二遍：提取函数实现...")
        self.extract_implementations()

        # 第三遍：提取结构体定义
        print(f"\n[*] 第三遍：提取结构体定义...")
        self.extract_structures()

        # 生成输出文件
        print(f"\n[*] 生成输出文件...")
        self.generate_files()

        # 统计信息
        self.print_statistics()

    def extract_declarations(self):
        """提取函数声明"""
        # 构建匹配模式（支持模板和命名空间）
        pattern_str = r'^([a-zA-Z_][\w\s\*<>,]+?)\s+(__cdecl\s+)?('
        pattern_str += '|'.join(re.escape(p).replace(r'\[', '[').replace(r'\]', ']').replace(r'\+', '+') for p in self.target_patterns)
        pattern_str += r')::(\w+)\s*\([^)]*\)\s*;'

        pattern = re.compile(pattern_str, re.MULTILINE)

        with open(self.source_file, 'r', encoding='utf-8', errors='ignore') as f:
            line_num = 0
            for line in f:
                line_num += 1

                # 只处理声明部分
                if line_num > 100000:
                    break

                match = pattern.match(line)
                if match:
                    return_type = match.group(1).strip()
                    full_class_name = match.group(3)
                    func_name = match.group(4)

                    simple_name = self.get_simple_name(full_class_name)

                    self.declarations[simple_name].append({
                        'line': line_num,
                        'code': line.rstrip(),
                        'return_type': return_type,
                        'function': func_name,
                        'full_class': full_class_name
                    })

        # 打印统计
        for cls in self.class_map.values():
            count = len(self.declarations[cls])
            if count > 0:
                print(f"    {cls}: {count} 个声明")

    def extract_implementations(self):
        """提取函数实现"""
        print("    使用行扫描模式提取函数实现...")

        # 构建函数开始模式
        pattern_str = r'^([a-zA-Z_][\w\s\*<>,]+?)\s+(__cdecl\s+)?('
        pattern_str += '|'.join(re.escape(p).replace(r'\[', '[').replace(r'\]', ']').replace(r'\+', '+') for p in self.target_patterns)
        pattern_str += r')::(\w+)\s*\('

        func_start_pattern = re.compile(pattern_str)

        with open(self.source_file, 'r', encoding='utf-8', errors='ignore') as f:
            line_num = 0
            current_func = None
            brace_count = 0
            func_start_line = 0

            for line in f:
                line_num += 1

                # 跳过声明部分
                if line_num <= 100000:
                    continue

                # 跳过 Flex 表格部分
                if 97000 <= line_num <= 150000:
                    continue

                # 检查是否是函数开始
                if current_func is None:
                    match = func_start_pattern.match(line)
                    if match:
                        return_type = match.group(1).strip()
                        full_class_name = match.group(3)
                        func_name = match.group(4)

                        simple_name = self.get_simple_name(full_class_name)

                        current_func = {
                            'class': simple_name,
                            'function': func_name,
                            'return_type': return_type,
                            'full_class': full_class_name,
                            'lines': []
                        }
                        func_start_line = line_num
                        brace_count = 0

                # 如果正在提取函数
                if current_func:
                    current_func['lines'].append(line.rstrip())

                    # 计数大括号
                    brace_count += line.count('{')
                    brace_count -= line.count('}')

                    # 函数结束
                    if brace_count == 0 and '{' in ''.join(current_func['lines']):
                        # 保存函数实现
                        class_name = current_func['class']
                        self.implementations[class_name].append({
                            'line': func_start_line,
                            'code': '\n'.join(current_func['lines']),
                            'function': current_func['function'],
                            'return_type': current_func['return_type'],
                            'full_class': current_func['full_class']
                        })

                        current_func = None

                # 进度指示
                if line_num % 100000 == 0:
                    print(f"      进度: {line_num:,} 行...")

        # 打印统计
        for cls in self.class_map.values():
            count = len(self.implementations[cls])
            if count > 0:
                print(f"    {cls}: {count} 个实现")

    def extract_structures(self):
        """提取结构体定义"""
        # 简化：只查找非模板的结构体
        simple_classes = ['PacketBuf', 'InterfacePacketBuf', 'CmmChannelProxy', 'tagPacketHeader']

        struct_pattern = re.compile(
            r'^(struct|class)\s+(' + '|'.join(simple_classes) + r')\s*\{',
            re.MULTILINE
        )

        with open(self.source_file, 'r', encoding='utf-8', errors='ignore') as f:
            line_num = 0
            current_struct = None
            brace_count = 0

            for line in f:
                line_num += 1

                if line_num > 100000:
                    break

                if current_struct is None:
                    match = struct_pattern.match(line)
                    if match:
                        current_struct = {
                            'keyword': match.group(1),
                            'name': match.group(2),
                            'lines': [],
                            'start_line': line_num
                        }
                        brace_count = 0

                if current_struct:
                    current_struct['lines'].append(line.rstrip())
                    brace_count += line.count('{')
                    brace_count -= line.count('}')

                    if brace_count == 0 and '{' in ''.join(current_struct['lines']):
                        self.structures.append(current_struct)
                        print(f"    找到结构体: {current_struct['name']} (行 {current_struct['start_line']})")
                        current_struct = None

    def generate_files(self):
        """生成输出文件"""
        # 创建目录
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)

        # 为每个类生成文件
        for cls in self.class_map.values():
            if not self.declarations[cls] and not self.implementations[cls]:
                continue

            # 生成头文件
            self.generate_header(cls)

            # 生成源文件
            self.generate_source(cls)

    def generate_header(self, class_name):
        """生成头文件"""
        header_file = self.include_dir / f"{class_name}.h"

        print(f"    生成: {header_file}")

        with open(header_file, 'w', encoding='utf-8') as f:
            # 头文件保护
            guard = f"__{class_name.upper()}_H__"
            f.write(f"#ifndef {guard}\n")
            f.write(f"#define {guard}\n\n")

            # 包含依赖
            f.write("#include <common/defs.h>\n")
            f.write("#include <stdint.h>\n")
            f.write("#include <sys/socket.h>\n\n")

            # 结构体定义
            struct_found = False
            for struct in self.structures:
                if struct['name'] == class_name:
                    f.write("// Structure definition\n")
                    f.write('\n'.join(struct['lines']))
                    f.write("\n\n")
                    struct_found = True
                    break

            if not struct_found:
                # 前向声明
                if class_name == 'CNetwork':
                    f.write("// Template class forward declaration\n")
                    f.write("template<int BufferSize, int MaxConnections>\n")
                    f.write("class CNetwork;\n\n")
                elif class_name == 'CSession':
                    f.write("// Forward declaration\n")
                    f.write("namespace exchange_server {\n")
                    f.write("    class CSession;\n")
                    f.write("}\n\n")
                else:
                    f.write(f"// Forward declaration\n")
                    f.write(f"struct {class_name};\n\n")

            # 函数声明
            if self.declarations[class_name]:
                f.write(f"// Function declarations ({len(self.declarations[class_name])} functions)\n\n")

                for decl in self.declarations[class_name]:
                    f.write(f"{decl['code']}\n")

            # 结束头文件保护
            f.write(f"\n#endif // {guard}\n")

    def generate_source(self, class_name):
        """生成源文件"""
        if not self.implementations[class_name]:
            return

        source_file = self.output_dir / f"{class_name}.cpp"

        print(f"    生成: {source_file}")

        with open(source_file, 'w', encoding='utf-8') as f:
            # 包含头文件
            f.write(f"#include <network/{class_name}.h>\n")
            f.write("#include <common/error_codes.h>\n")
            f.write("#include <common/constants_improved.h>\n\n")

            # 函数实现
            f.write(f"// {class_name} Implementation ({len(self.implementations[class_name])} functions)\n\n")

            for impl in self.implementations[class_name]:
                f.write(f"// {impl['function']} (line {impl['line']})\n")
                f.write(f"{impl['code']}\n\n")

    def print_statistics(self):
        """打印统计信息"""
        print(f"\n{'='*60}")
        print(f"提取统计")
        print(f"{'='*60}")

        total_decl = 0
        total_impl = 0

        for cls in self.class_map.values():
            decl_count = len(self.declarations[cls])
            impl_count = len(self.implementations[cls])

            if decl_count > 0 or impl_count > 0:
                print(f"\n{cls}:")
                print(f"  声明: {decl_count:4d}")
                print(f"  实现: {impl_count:4d}")

                total_decl += decl_count
                total_impl += impl_count

        print(f"\n总计:")
        print(f"  声明: {total_decl:4d}")
        print(f"  实现: {total_impl:4d}")
        print(f"  结构体: {len(self.structures):4d}")

        print(f"\n输出目录:")
        print(f"  头文件: {self.include_dir}/")
        print(f"  源文件: {self.output_dir}/")
        print(f"{'='*60}\n")

def main():
    if len(sys.argv) > 1:
        source_file = sys.argv[1]
    else:
        source_file = "archive/source_versions/df_game_r_v3.c"

    print(f"网络层代码提取工具")
    print(f"{'='*60}\n")

    extractor = NetworkExtractor(source_file)
    extractor.extract()

    print("\n✓ 提取完成!")

if __name__ == "__main__":
    main()
