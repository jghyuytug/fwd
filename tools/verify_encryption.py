#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
验证Script.pvf索引是否真的加密了

突破固有思维：也许索引根本没加密，只是我们解析错了？
"""

import struct
import sys


def analyze_raw_index(pvf_path, output_samples=True):
    """分析原始索引数据"""
    print("=" * 80)
    print("Script.pvf索引原始数据分析")
    print("=" * 80)
    print()

    with open(pvf_path, 'rb') as f:
        # 读取头部
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"PVF头部信息:")
        print(f"  GUID: {tag_string}")
        print(f"  版本: {revision}")
        print(f"  文件数量: {index_count:,}")
        print(f"  索引大小: {index_header_size:,} 字节")
        print(f"  索引CRC32: 0x{index_crc32:08x}")
        print()

        # 读取索引原始数据
        index_start = f.tell()
        raw_index = f.read(min(10000, index_header_size))  # 读取前10KB

        print("=" * 80)
        print("原始索引数据分析（前10KB）")
        print("=" * 80)
        print()

        # 1. 十六进制显示
        print("1. 十六进制数据（前256字节）:")
        print(raw_index[:256].hex())
        print()

        # 2. 字节值分布
        byte_freq = [0] * 256
        for byte in raw_index:
            byte_freq[byte] += 1

        print("2. 字节值分布分析:")
        printable_count = sum(byte_freq[32:127])
        total_count = len(raw_index)
        print(f"  可打印字符 (0x20-0x7E): {printable_count}/{total_count} ({printable_count*100/total_count:.1f}%)")
        print(f"  NULL字节 (0x00): {byte_freq[0]} ({byte_freq[0]*100/total_count:.1f}%)")
        print(f"  高字节 (0x80-0xFF): {sum(byte_freq[128:256])} ({sum(byte_freq[128:256])*100/total_count:.1f}%)")
        print()

        # 3. 熵值计算
        import math
        entropy = 0.0
        for freq in byte_freq:
            if freq > 0:
                p = freq / total_count
                entropy -= p * math.log2(p)

        print(f"3. 数据熵值: {entropy:.3f}")
        print(f"   (0=完全有序, 8=完全随机)")
        print(f"   期望: 明文<6.0, 加密>7.5")
        print()

        # 4. 尝试不同编码解析
        print("4. 尝试直接解析（假设未加密）:")
        print()

        encodings = ['utf-8', 'gbk', 'latin1', 'ascii']
        for encoding in encodings:
            try:
                decoded = raw_index[:500].decode(encoding, errors='ignore')
                visible_chars = sum(1 for c in decoded if c.isprintable())
                print(f"  {encoding:8s}: 可见字符 {visible_chars}/{len(decoded)} ({visible_chars*100/len(decoded):.1f}%)")
                if visible_chars > len(decoded) * 0.3:
                    print(f"             预览: {decoded[:100]}")
            except:
                print(f"  {encoding:8s}: 解码失败")
        print()

        # 5. 查找可能的文件名模式
        print("5. 搜索文件名特征:")
        print()

        # PVF索引格式可能是：
        # [4字节哈希] [4字节文件名长度] [文件名] [其他数据]

        possible_filenames = []
        for i in range(0, len(raw_index) - 8, 4):
            # 假设这是文件名长度字段
            name_len = struct.unpack('<I', raw_index[i:i+4])[0]

            # 文件名长度应该在合理范围内
            if 5 <= name_len <= 200:
                # 尝试读取文件名
                if i + 4 + name_len <= len(raw_index):
                    try:
                        filename = raw_index[i+4:i+4+name_len].decode('utf-8', errors='strict')
                        # 检查是否像文件路径
                        if '/' in filename or '.' in filename:
                            if all(c.isprintable() or c in ['\r', '\n'] for c in filename):
                                possible_filenames.append((i, name_len, filename))
                    except:
                        pass

        if possible_filenames:
            print(f"  找到 {len(possible_filenames)} 个可能的文件名:")
            for offset, length, filename in possible_filenames[:10]:
                print(f"    偏移 {offset:6d}: 长度={length:3d}, 路径={filename}")
            if len(possible_filenames) > 10:
                print(f"    ... 还有 {len(possible_filenames)-10} 个")
        else:
            print("  [X] 未找到任何文件名模式")
            print("  -> 索引很可能是加密的")
        print()

        # 6. 尝试按索引条目格式解析
        print("6. 尝试按PVF索引格式解析:")
        print()
        print("   假设格式: [4B哈希][4B名称长度][NB名称][8B位置+大小]")
        print()

        offset = 0
        parsed_count = 0
        for attempt in range(10):  # 尝试解析前10个条目
            if offset + 8 > len(raw_index):
                break

            hash_value = struct.unpack('<I', raw_index[offset:offset+4])[0]
            name_length = struct.unpack('<I', raw_index[offset+4:offset+8])[0]

            print(f"   条目 {attempt+1}:")
            print(f"     偏移: {offset}")
            print(f"     哈希: 0x{hash_value:08x}")
            print(f"     名称长度: {name_length}")

            if name_length > 1000 or name_length == 0:
                print(f"     [X] 名称长度异常")
                print()
                break

            if offset + 8 + name_length > len(raw_index):
                print(f"     [X] 数据不足")
                print()
                break

            try:
                filename = raw_index[offset+8:offset+8+name_length].decode('utf-8', errors='strict')
                if all(c.isprintable() or c in ['/', '\\', '\r', '\n'] for c in filename):
                    print(f"     [+] 文件名: {filename}")
                    parsed_count += 1
                else:
                    print(f"     [X] 文件名包含非法字符: {repr(filename[:50])}")
                    break
            except:
                print(f"     [X] 文件名解码失败")
                break

            # 移动到下一个条目
            # 假设每个条目是: 哈希(4) + 长度(4) + 名称(N) + 位置大小(8)
            offset += 8 + name_length + 8
            print()

        print()
        print("=" * 80)
        print("分析结论:")
        print("=" * 80)
        print()

        if parsed_count > 0:
            print(f"[+] 成功解析 {parsed_count} 个索引条目（假设未加密）")
            print(f"   -> 索引可能没有加密，或使用了简单的编码")
            return True
        else:
            print(f"[X] 无法解析任何索引条目")
            print(f"   -> 索引很可能使用了加密")
            print()
            print(f"   证据:")
            print(f"   - 熵值: {entropy:.3f} ({'高' if entropy > 7.0 else '中' if entropy > 6.0 else '低'})")
            print(f"   - 可打印字符: {printable_count*100/total_count:.1f}%")
            print(f"   - 未找到文件名模式")
            return False


if __name__ == '__main__':
    pvf_path = "C:\\Users\\waw\\Desktop\\game\\Script.pvf"
    is_plaintext = analyze_raw_index(pvf_path)
    sys.exit(0 if is_plaintext else 1)
