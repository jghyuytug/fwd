#!/usr/bin/env python3
import re

def extract_function(filename, start_line):
    with open(filename, 'r') as f:
        lines = f.readlines()

    # 找到函数定义开始
    while start_line < len(lines):
        if lines[start_line-1].strip().startswith('int __cdecl main('):
            break
        start_line += 1

    # 提取函数体
    func_lines = []
    brace_count = 0
    in_function = False

    for line in lines[start_line-1:]:
        # 计算大括号数量
        brace_count += line.count('{') - line.count('}')

        # 开始记录函数
        if '{' in line and not in_function:
            in_function = True

        # 记录函数内容
        if in_function:
            func_lines.append(line)

        # 函数结束
        if in_function and brace_count == 0:
            break

    return ''.join(func_lines)

def extract_nearby_funcs(filename, start_line):
    main_func = extract_function(filename, start_line)

    # 提取 ShowLogo
    showlogo_func = extract_function(filename, start_line + 10)

    return main_func, showlogo_func

def main():
    filename = 'df_game_r.c'
    start_line = 1017148  # main 函数起始行

    main_func, showlogo_func = extract_nearby_funcs(filename, start_line)

    # 输出到文件
    with open('extracted_main_funcs.txt', 'w') as f:
        f.write("=== main() ===\n")
        f.write(main_func)
        f.write("\n\n=== ShowLogo() ===\n")
        f.write(showlogo_func)

if __name__ == '__main__':
    main()