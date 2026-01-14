#!/usr/bin/env python3
"""
游戏逻辑模块提取工具 (Phase 5 Stage 5)

从 df_game_r_v3.c 中提取游戏逻辑相关的类和函数，
包括：dungeon, party, skill, events, security 五大系统

用法:
    python3 extract_game.py [source_file]

默认源文件: archive/source_versions/df_game_r_v3.c
"""

import re
import sys
from pathlib import Path
from collections import defaultdict

class GameExtractor:
    """游戏逻辑提取器"""

    def __init__(self, source_file="archive/source_versions/df_game_r_v3.c"):
        self.source_file = Path(source_file)

        # 目标类和命名空间 - 按系统分组
        self.target_patterns = {
            # Dungeon 系统
            'dungeon': [
                'CDungeon',
                'CBattle_Field',
            ],
            # Party 系统
            'party': [
                'CParty',
                'QuickParty::[A-Za-z0-9_]+',  # QuickParty 命名空间
            ],
            # Skill 系统
            'skill': [
                'CSkill',
                'CSkillList',
                'CSkillTreeParam',
                'ComboSkill::[A-Za-z0-9_]+',
            ],
            # Events 系统
            'events': [
                'CEventBase',
                'EventClassify::[A-Za-z0-9_]+',  # EventClassify 命名空间
                'BingoEvent',
                'AttendanceEvent',
                'BlueMarbleDungeonEvent',
                'CFatigueBuffEvent',
            ],
            # Security 反作弊系统
            'security': [
                'WongWork::CHackAnalyzer',
                'WongWork::CAutoPunishRuleBlackIPMgr',
                'WongWork::CGMAccounts',
            ],
        }

        # 类名到简化名的映射（用于文件命名）
        self.class_map = {
            'CDungeon': 'CDungeon',
            'CBattle_Field': 'CBattle_Field',
            'CParty': 'CParty',
            'QuickParty': 'QuickParty',
            'CSkill': 'CSkill',
            'CSkillList': 'CSkillList',
            'CSkillTreeParam': 'CSkillTreeParam',
            'ComboSkill': 'ComboSkill',
            'CEventBase': 'CEventBase',
            'EventClassify': 'EventClassify',
            'BingoEvent': 'BingoEvent',
            'AttendanceEvent': 'AttendanceEvent',
            'BlueMarbleDungeonEvent': 'BlueMarbleDungeonEvent',
            'CFatigueBuffEvent': 'CFatigueBuffEvent',
            'WongWork::CHackAnalyzer': 'CHackAnalyzer',
            'WongWork::CAutoPunishRuleBlackIPMgr': 'CAutoPunishRuleBlackIPMgr',
            'WongWork::CGMAccounts': 'CGMAccounts',
        }

        # 存储提取的内容 - 按系统分组
        self.declarations = defaultdict(lambda: defaultdict(list))
        self.implementations = defaultdict(lambda: defaultdict(list))
        self.structures = defaultdict(list)

        # Flex 表区域 (需要跳过)
        self.flex_start = 97000
        self.flex_end = 150000

    def get_system_dir(self, system):
        """获取系统对应的目录"""
        if system in ['dungeon', 'party', 'skill']:
            return Path(f"src/game/{system}"), Path(f"include/game/{system}")
        else:
            return Path(f"src/{system}"), Path(f"include/{system}")

    def matches_pattern(self, line, pattern):
        """检查行是否匹配类模式"""
        # 匹配函数声明: type __cdecl ClassName::method(
        match = re.search(rf'\b{pattern}::\w+\s*\(', line)
        return match is not None

    def extract_declarations(self):
        """第一遍：提取函数声明（前10万行）"""
        print("第一遍：提取函数声明...")

        with open(self.source_file, 'r', encoding='utf-8') as f:
            for line_num, line in enumerate(f, 1):
                if line_num > 100000:
                    break

                # 跳过空行和注释
                if not line.strip() or line.strip().startswith('//'):
                    continue

                # 检查每个系统的每个模式
                for system, patterns in self.target_patterns.items():
                    for pattern in patterns:
                        if self.matches_pattern(line, pattern):
                            # 提取类名
                            match = re.search(rf'({pattern})::', line)
                            if match:
                                class_name = match.group(1)
                                simple_name = self.class_map.get(class_name, class_name.split('::')[-1])
                                self.declarations[system][simple_name].append(line.rstrip())
                            break

        # 打印统计
        total = 0
        for system, classes in self.declarations.items():
            system_total = sum(len(decls) for decls in classes.values())
            total += system_total
            print(f"  {system}: {system_total} 个声明")
        print(f"  总计: {total} 个函数声明\n")

    def extract_implementations(self):
        """第二遍：提取函数实现（10万行到文件末尾，跳过Flex区域）"""
        print("第二遍：提取函数实现...")

        with open(self.source_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        in_function = False
        current_function = []
        current_system = None
        current_class = None
        brace_count = 0

        for line_num, line in enumerate(lines, 1):
            # 跳过前10万行和Flex区域
            if line_num < 100000:
                continue
            if self.flex_start <= line_num <= self.flex_end:
                continue

            if not in_function:
                # 查找函数开始
                for system, patterns in self.target_patterns.items():
                    for pattern in patterns:
                        if self.matches_pattern(line, pattern):
                            # 找到函数开始
                            match = re.search(rf'({pattern})::', line)
                            if match:
                                class_name = match.group(1)
                                simple_name = self.class_map.get(class_name, class_name.split('::')[-1])
                                in_function = True
                                current_system = system
                                current_class = simple_name
                                current_function = [line.rstrip()]
                                brace_count = line.count('{') - line.count('}')
                                break
                    if in_function:
                        break
            else:
                # 继续收集函数体
                current_function.append(line.rstrip())
                brace_count += line.count('{') - line.count('}')

                # 检查函数是否结束
                if brace_count == 0:
                    self.implementations[current_system][current_class].append('\n'.join(current_function))
                    in_function = False
                    current_function = []
                    current_system = None
                    current_class = None

        # 打印统计
        total = 0
        for system, classes in self.implementations.items():
            system_total = sum(len(impls) for impls in classes.values())
            total += system_total
            print(f"  {system}: {system_total} 个实现")
        print(f"  总计: {total} 个函数实现\n")

    def extract_structures(self):
        """第三遍：提取结构体定义（前10万行）"""
        print("第三遍：提取结构体定义...")

        # 为每个系统查找相关的结构体
        struct_patterns = {
            'dungeon': [r'struct\s+(ST)?Dungeon', r'struct\s+.*Battle'],
            'party': [r'struct\s+(ST)?Party'],
            'skill': [r'struct\s+(ST)?Skill', r'struct\s+.*Combo'],
            'events': [r'struct\s+(ST)?Event', r'struct\s+.*Bingo', r'struct\s+.*Attendance'],
            'security': [r'struct\s+.*Hack', r'struct\s+.*Punish'],
        }

        with open(self.source_file, 'r', encoding='utf-8') as f:
            in_struct = False
            current_struct = []
            current_system = None
            brace_count = 0

            for line_num, line in enumerate(f, 1):
                if line_num > 100000:
                    break

                if not in_struct:
                    # 查找结构体开始
                    for system, patterns in struct_patterns.items():
                        for pattern in patterns:
                            if re.search(pattern, line):
                                in_struct = True
                                current_system = system
                                current_struct = [line.rstrip()]
                                brace_count = line.count('{') - line.count('}')
                                break
                        if in_struct:
                            break
                else:
                    # 继续收集结构体
                    current_struct.append(line.rstrip())
                    brace_count += line.count('{') - line.count('}')

                    # 检查结构体是否结束
                    if brace_count == 0 and ';' in line:
                        self.structures[current_system].append('\n'.join(current_struct))
                        in_struct = False
                        current_struct = []
                        current_system = None

        # 打印统计
        total = sum(len(structs) for structs in self.structures.values())
        for system, structs in self.structures.items():
            print(f"  {system}: {len(structs)} 个结构体")
        print(f"  总计: {total} 个结构体定义\n")

    def generate_files(self):
        """生成头文件和源文件"""
        print("生成模块文件...")

        total_files = 0
        total_size = 0

        for system in self.target_patterns.keys():
            src_dir, inc_dir = self.get_system_dir(system)
            src_dir.mkdir(parents=True, exist_ok=True)
            inc_dir.mkdir(parents=True, exist_ok=True)

            # 获取该系统的所有类
            all_classes = set(self.declarations[system].keys()) | set(self.implementations[system].keys())

            for class_name in sorted(all_classes):
                # 生成头文件
                header_file = inc_dir / f"{class_name}.h"
                with open(header_file, 'w', encoding='utf-8') as f:
                    f.write(f"#ifndef {class_name.upper()}_H\n")
                    f.write(f"#define {class_name.upper()}_H\n\n")
                    f.write("#include <common/defs.h>\n\n")

                    # 写入结构体定义
                    if self.structures[system]:
                        f.write("// Structure definitions\n")
                        for struct in self.structures[system]:
                            f.write(struct + "\n\n")

                    # 写入函数声明
                    if class_name in self.declarations[system]:
                        f.write(f"// {class_name} declarations\n")
                        for decl in self.declarations[system][class_name]:
                            f.write(decl + "\n")

                    f.write(f"\n#endif // {class_name.upper()}_H\n")

                total_files += 1
                total_size += header_file.stat().st_size
                print(f"  生成: {header_file} ({header_file.stat().st_size:,} bytes)")

                # 生成源文件
                if class_name in self.implementations[system]:
                    source_file = src_dir / f"{class_name}.cpp"
                    with open(source_file, 'w', encoding='utf-8') as f:
                        f.write(f"#include <{system}/{class_name}.h>\n")
                        if system in ['dungeon', 'party', 'skill']:
                            f.write(f"#include <game/{system}/{class_name}.h>\n")
                        f.write("\n")

                        for impl in self.implementations[system][class_name]:
                            f.write(impl + "\n\n")

                    total_files += 1
                    total_size += source_file.stat().st_size
                    print(f"  生成: {source_file} ({source_file.stat().st_size:,} bytes)")

        print(f"\n总计生成 {total_files} 个文件，共 {total_size:,} bytes ({total_size/1024:.1f} KB)\n")

    def extract(self):
        """执行完整提取流程"""
        print(f"=== 游戏逻辑模块提取工具 ===\n")
        print(f"源文件: {self.source_file}\n")

        if not self.source_file.exists():
            print(f"错误: 源文件不存在: {self.source_file}")
            sys.exit(1)

        self.extract_declarations()
        self.extract_implementations()
        self.extract_structures()
        self.generate_files()

        print("✓ 提取完成!")

def main():
    source_file = sys.argv[1] if len(sys.argv) > 1 else "archive/source_versions/df_game_r_v3.c"
    extractor = GameExtractor(source_file)
    extractor.extract()

if __name__ == "__main__":
    main()
