#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能搜索g_AdditionalKey值 - 基于熵分析和质量评分

方案2: 扩展候选值，使用解密数据质量评估找到最可能的参数
"""

import struct
import sys
import math
from typing import List, Tuple


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def calculate_entropy(data: bytes, sample_size: int = 4096) -> float:
    """
    计算数据熵值

    熵越低，数据越有规律（更接近明文）
    熵越高，数据越随机（更接近密文）

    Args:
        data: 待计算的数据
        sample_size: 采样大小（避免大文件计算过慢）

    Returns:
        熵值 (0-8，0=完全有序，8=完全随机)
    """
    if len(data) == 0:
        return 0.0

    # 采样（如果数据太大）
    if len(data) > sample_size:
        data = data[:sample_size]

    # 统计字节频率
    freq = [0] * 256
    for byte in data:
        freq[byte] += 1

    # 计算熵
    entropy = 0.0
    data_len = len(data)

    for count in freq:
        if count == 0:
            continue
        p = count / data_len
        entropy -= p * math.log2(p)

    return entropy


def calculate_printable_ratio(data: bytes, sample_size: int = 4096) -> float:
    """
    计算可打印字符比例

    明文数据通常包含较高比例的可打印字符

    Args:
        data: 待计算的数据
        sample_size: 采样大小

    Returns:
        可打印字符比例 (0.0-1.0)
    """
    if len(data) == 0:
        return 0.0

    # 采样
    if len(data) > sample_size:
        data = data[:sample_size]

    printable_count = sum(1 for b in data if 32 <= b < 127)
    return printable_count / len(data)


def check_filename_validity(data: bytes) -> Tuple[bool, str]:
    """
    检查解密数据是否包含合理的文件名

    尝试解析第一个索引条目，判断文件名是否合理

    Returns:
        (是否有效, 文件名或错误信息)
    """
    try:
        offset = 0

        # 读取哈希值
        if len(data) < 8:
            return False, "数据太短"

        hash_value = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # 读取文件名长度
        name_length = struct.unpack('<I', data[offset:offset+4])[0]
        offset += 4

        # 检查文件名长度是否合理
        if name_length > 1000 or name_length == 0:
            return False, f"文件名长度异常: {name_length}"

        # 检查是否有足够数据
        if len(data) < offset + name_length:
            return False, f"数据不足以包含文件名 (需要{name_length}字节)"

        # 读取文件名
        filename = data[offset:offset+name_length].decode('utf-8', errors='strict')

        # 检查文件名是否合理（只包含可打印字符和路径分隔符）
        if not all(c.isprintable() or c in ['/', '\\'] for c in filename):
            return False, f"文件名包含非法字符: {repr(filename[:50])}"

        # 检查是否像文件路径
        if '/' not in filename and '\\' not in filename:
            # 可能是根目录文件，检查是否有扩展名
            if '.' not in filename:
                return False, f"可能不是有效路径: {filename}"

        return True, filename

    except UnicodeDecodeError:
        return False, "文件名包含非UTF-8字符"
    except Exception as e:
        return False, f"解析失败: {str(e)}"


def decrypt_index_with_key(encrypted_data: bytes,
                            crc_init: int,
                            checksum: int,
                            key: int,
                            g_additional_key: int) -> Tuple[bool, int, bytes]:
    """
    使用指定的g_AdditionalKey解密索引

    Returns:
        (是否CRC匹配, 实际CRC32值, 解密数据)
    """
    size = len(encrypted_data)

    if (size & 3) != 0:
        return False, 0, None

    # 生成CRC32表
    def make_crc_table():
        table = []
        for i in range(256):
            c = i
            for _ in range(8):
                if c & 1:
                    c = 0xEDB88320 ^ (c >> 1)
                else:
                    c = c >> 1
            table.append(c)
        return table

    crc_table = make_crc_table()
    crc = ~crc_init & 0xFFFFFFFF

    decrypted = bytearray(encrypted_data)

    for i in range(0, size, 4):
        dword = struct.unpack('<I', decrypted[i:i+4])[0]

        # 步骤1: XOR g_AdditionalKey
        if g_additional_key:
            dword ^= g_additional_key

        # 步骤2: ROR(key ^ data, 6)
        dword = ror32(key ^ dword, 6)

        struct.pack_into('<I', decrypted, i, dword)

        # 步骤3: 更新CRC32
        for j in range(4):
            byte = decrypted[i + j]
            crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

    final_crc = ~crc & 0xFFFFFFFF

    return final_crc == checksum, final_crc, bytes(decrypted)


def calculate_quality_score(decrypted: bytes, crc_matched: bool) -> float:
    """
    计算解密数据的质量分数

    分数越高，越可能是正确的解密

    Returns:
        质量分数 (0-100)
    """
    score = 0.0

    # 1. CRC32匹配 (60分 - 最重要)
    if crc_matched:
        score += 60.0

    # 2. 熵值评分 (20分)
    # 明文索引数据熵值应该在 4.0-6.5 之间
    entropy = calculate_entropy(decrypted)
    if 4.0 <= entropy <= 6.5:
        score += 20.0
    elif 3.0 <= entropy < 4.0 or 6.5 < entropy <= 7.0:
        score += 10.0
    elif entropy < 3.0 or entropy > 7.5:
        score += 0.0
    else:
        score += 5.0

    # 3. 可打印字符比例 (10分)
    printable_ratio = calculate_printable_ratio(decrypted)
    # 索引数据期望30-70%可打印字符
    if 0.3 <= printable_ratio <= 0.7:
        score += 10.0
    elif 0.2 <= printable_ratio < 0.3 or 0.7 < printable_ratio <= 0.8:
        score += 5.0

    # 4. 文件名有效性 (10分)
    filename_valid, _ = check_filename_validity(decrypted)
    if filename_valid:
        score += 10.0

    return score


def generate_candidates(index_crc32: int, index_count: int,
                        index_header_size: int, revision: int) -> List[Tuple[str, int]]:
    """
    生成候选g_AdditionalKey值

    包括:
    - 基础值（0, 反编译值）
    - 文件相关值
    - 常见魔数
    - 数学运算结果
    - 位操作结果
    """
    candidates = []

    # 1. 基础值
    candidates.append(("0 (禁用)", 0))
    candidates.append(("反编译值", 0x81A12D11))

    # 2. 文件相关值
    candidates.append(("index_crc32", index_crc32))
    candidates.append(("~index_crc32", ~index_crc32 & 0xFFFFFFFF))
    candidates.append(("index_count", index_count))
    candidates.append(("~index_count", ~index_count & 0xFFFFFFFF))
    candidates.append(("revision", revision))
    candidates.append(("~revision", ~revision & 0xFFFFFFFF))
    candidates.append(("index_header_size", index_header_size))
    candidates.append(("~index_header_size", ~index_header_size & 0xFFFFFFFF))

    # 3. 常见魔数
    magic_numbers = [
        ("DEADBEEF", 0xDEADBEEF),
        ("CAFEBABE", 0xCAFEBABE),
        ("FEEDFACE", 0xFEEDFACE),
        ("12345678", 0x12345678),
        ("FFFFFFFF", 0xFFFFFFFF),
        ("AAAAAAAA", 0xAAAAAAAA),
        ("55555555", 0x55555555),
        ("ABCDEF01", 0xABCDEF01),
    ]
    candidates.extend(magic_numbers)

    # 4. 数学运算
    candidates.append(("crc32 * 2", (index_crc32 * 2) & 0xFFFFFFFF))
    candidates.append(("crc32 + count", (index_crc32 + index_count) & 0xFFFFFFFF))
    candidates.append(("crc32 - count", (index_crc32 - index_count) & 0xFFFFFFFF))
    candidates.append(("crc32 ^ count", index_crc32 ^ index_count))
    candidates.append(("count * revision", (index_count * revision) & 0xFFFFFFFF))

    # 5. 位操作
    candidates.append(("crc32 << 1", (index_crc32 << 1) & 0xFFFFFFFF))
    candidates.append(("crc32 >> 1", index_crc32 >> 1))
    candidates.append(("crc32 << 8", (index_crc32 << 8) & 0xFFFFFFFF))
    candidates.append(("crc32 >> 8", index_crc32 >> 8))
    candidates.append(("ROR(crc32, 13)", ror32(index_crc32, 13)))
    candidates.append(("ROR(crc32, 17)", ror32(index_crc32, 17)))

    # 6. 组合运算
    candidates.append(("(crc32 ^ 0xFF) << 8", ((index_crc32 ^ 0xFF) << 8) & 0xFFFFFFFF))
    candidates.append(("~(crc32 << 1)", ~(index_crc32 << 1) & 0xFFFFFFFF))
    candidates.append(("(count << 16) | revision", ((index_count << 16) | revision) & 0xFFFFFFFF))

    return candidates


def main(pvf_path: str):
    print("=" * 80)
    print("智能搜索g_AdditionalKey - 基于熵分析和质量评分")
    print("=" * 80)
    print()

    # 读取Script.pvf头部和索引
    with open(pvf_path, 'rb') as f:
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"文件: {pvf_path}")
        print(f"文件数量: {index_count:,}")
        print(f"索引大小: {index_header_size:,} 字节")
        print(f"索引CRC32: 0x{index_crc32:08x}")
        print(f"版本号: {revision}")
        print()

        encrypted_index = f.read(index_header_size)

    # 生成候选值
    candidates = generate_candidates(index_crc32, index_count, index_header_size, revision)

    print(f"[*] 生成 {len(candidates)} 个候选值")
    print()
    print("[*] 测试所有候选值并评分...")
    print()

    # 测试所有候选值并收集结果
    results = []

    for name, g_add in candidates:
        matched, actual_crc, decrypted = decrypt_index_with_key(
            encrypted_index,
            index_count,
            index_crc32,
            index_crc32,
            g_add
        )

        if decrypted is None:
            continue

        # 计算质量分数
        score = calculate_quality_score(decrypted, matched)

        # 计算详细指标
        entropy = calculate_entropy(decrypted)
        printable_ratio = calculate_printable_ratio(decrypted)
        filename_valid, filename_info = check_filename_validity(decrypted)

        results.append({
            'name': name,
            'g_add': g_add,
            'matched': matched,
            'actual_crc': actual_crc,
            'score': score,
            'entropy': entropy,
            'printable': printable_ratio,
            'filename_valid': filename_valid,
            'filename_info': filename_info,
            'decrypted': decrypted
        })

        # 如果CRC32匹配，立即报告
        if matched:
            print(f"[+] CRC32匹配! {name} = 0x{g_add:08x}")
            print()

    # 按分数排序
    results.sort(key=lambda x: x['score'], reverse=True)

    print()
    print("=" * 80)
    print("Top 15 候选值（按质量分数排序）")
    print("=" * 80)
    print()
    print(f"{'排名':<4} {'分数':<6} {'CRC匹配':<8} {'熵值':<6} {'可打印%':<8} {'文件名':<8} {'候选值'}")
    print("-" * 80)

    for i, result in enumerate(results[:15], 1):
        crc_status = "[+]" if result['matched'] else "[.]"
        filename_status = "[+]" if result['filename_valid'] else "[.]"

        print(f"{i:<4} {result['score']:<6.1f} {crc_status:<8} "
              f"{result['entropy']:<6.2f} {result['printable']*100:<7.1f}% "
              f"{filename_status:<8} "
              f"{result['name']} = 0x{result['g_add']:08x}")

    print()
    print("=" * 80)
    print("详细分析 - Top 5")
    print("=" * 80)

    for i, result in enumerate(results[:5], 1):
        print()
        print(f"#{i} - {result['name']} = 0x{result['g_add']:08x}")
        print(f"  质量分数: {result['score']:.1f}/100")
        print(f"  CRC32: 0x{result['actual_crc']:08x} {'[+] 匹配' if result['matched'] else '[.] 期望 0x' + f'{index_crc32:08x}'}")
        print(f"  熵值: {result['entropy']:.3f} (期望 4.0-6.5)")
        print(f"  可打印字符: {result['printable']*100:.1f}% (期望 30-70%)")
        print(f"  文件名: {'[+] ' + result['filename_info'] if result['filename_valid'] else '[.] ' + result['filename_info']}")

        # 显示前128字节十六进制
        hex_preview = result['decrypted'][:128].hex()
        print(f"  数据预览: {hex_preview[:64]}...")

    print()
    print("=" * 80)

    # 检查是否有匹配的
    perfect_matches = [r for r in results if r['matched']]
    if perfect_matches:
        print()
        print(f"[+] 找到 {len(perfect_matches)} 个CRC32完全匹配的候选值!")
        print()

        best = perfect_matches[0]
        print(f"[+] 最佳匹配: {best['name']} = 0x{best['g_add']:08x}")
        print(f"    文件名: {best['filename_info']}")

        # 保存解密数据
        with open('decrypted_index.bin', 'wb') as out:
            out.write(best['decrypted'])
        print()
        print("[+] 解密数据已保存到 decrypted_index.bin")

        return True

    else:
        print()
        print("[!] 没有找到CRC32完全匹配的值")
        print()
        print("但是，质量分数最高的候选值可能接近正确答案：")
        print()

        best = results[0]
        print(f"  最佳候选: {best['name']} = 0x{best['g_add']:08x}")
        print(f"  质量分数: {best['score']:.1f}/100")
        print(f"  文件名: {best['filename_info']}")

        # 如果最佳候选的文件名有效，也保存
        if best['filename_valid']:
            print()
            print("[*] 最佳候选的文件名看起来合理，保存解密数据...")
            with open('decrypted_index_best_candidate.bin', 'wb') as out:
                out.write(best['decrypted'])
            print("[+] 已保存到 decrypted_index_best_candidate.bin")

        return False


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python intelligent_bruteforce.py <Script.pvf>")
        sys.exit(1)

    success = main(sys.argv[1])
    sys.exit(0 if success else 1)
