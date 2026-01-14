#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
分析pvfUtility使用的解密参数

策略：通过API获取已解密的索引数据，对比Script.pvf中的加密索引，反推参数
"""

import struct
import requests
import sys


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def get_api_file_list(base_url="http://localhost:27000"):
    """通过API获取文件列表（解密后的索引数据）"""
    try:
        # 获取根目录
        response = requests.get(f"{base_url}/Api/PvfUtiltiy/getPvfRootDirectory")
        if response.status_code != 200:
            return None

        data = response.json()
        if data.get('IsError'):
            return None

        root_dirs = data.get('Data', [])
        print(f"[+] API返回 {len(root_dirs)} 个根目录")

        # 获取第一个目录的文件列表
        if root_dirs:
            dir_name = root_dirs[0]
            response = requests.get(
                f"{base_url}/Api/PvfUtiltiy/GetFileList",
                params={'dirName': dir_name, 'returnType': 1}
            )

            if response.status_code == 200:
                data = response.json()
                if not data.get('IsError'):
                    # 返回的是\r\n分隔的文件列表
                    file_list_str = data.get('Data', '')
                    files = [f for f in file_list_str.split('\r\n') if f.strip()]
                    print(f"[+] 目录'{dir_name}'包含 {len(files)} 个文件")
                    print(f"[+] 第一个文件: {files[0] if files else 'NONE'}")
                    return files[:10]  # 返回前10个文件

        return []
    except Exception as e:
        print(f"[!] API请求失败: {e}")
        return None


def read_script_pvf_index(pvf_path):
    """读取Script.pvf的索引数据（加密）"""
    try:
        with open(pvf_path, 'rb') as f:
            # 读取头部
            tag_length = struct.unpack('<I', f.read(4))[0]
            tag_string = f.read(tag_length).decode('utf-8')
            revision = struct.unpack('<I', f.read(4))[0]
            index_header_size = struct.unpack('<I', f.read(4))[0]
            index_crc32 = struct.unpack('<I', f.read(4))[0]
            index_count = struct.unpack('<I', f.read(4))[0]

            print()
            print(f"[*] Script.pvf信息:")
            print(f"    文件数量: {index_count:,}")
            print(f"    索引大小: {index_header_size:,} 字节")
            print(f"    索引CRC32: 0x{index_crc32:08x}")
            print(f"    版本: {revision}")
            print()

            # 读取加密的索引（优化：只读取前100KB用于测试）
            test_size = min(102400, index_header_size)  # 100KB或全部
            encrypted_index = f.read(test_size)

            print(f"[*] 优化：仅测试前{test_size:,}字节（加快速度）")

            return {
                'index_count': index_count,
                'index_crc32': index_crc32,
                'revision': revision,
                'index_header_size': index_header_size,
                'encrypted_index': encrypted_index,
                'test_size': test_size
            }
    except Exception as e:
        print(f"[!] 读取Script.pvf失败: {e}")
        return None


def decrypt_index_with_params(encrypted_data, crc_init, checksum, key,
                                g_additional_key, xor_key, ror_shift=6):
    """用指定参数解密索引"""
    size = len(encrypted_data)

    if (size & 3) != 0:
        return None, 0

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

    # 应用XOR_KEY
    if xor_key:
        key ^= xor_key

    decrypted = bytearray(encrypted_data)

    for i in range(0, size, 4):
        dword = struct.unpack('<I', decrypted[i:i+4])[0]

        # XOR g_AdditionalKey
        if g_additional_key:
            dword ^= g_additional_key

        # ROR解密
        dword = ror32(key ^ dword, ror_shift)

        struct.pack_into('<I', decrypted, i, dword)

        # 更新CRC32
        for j in range(4):
            byte = decrypted[i + j]
            crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

    final_crc = ~crc & 0xFFFFFFFF

    return bytes(decrypted), final_crc


def check_if_contains_filenames(decrypted_data, known_filenames):
    """检查解密数据是否包含已知的文件名"""
    decrypted_str = decrypted_data[:10000].decode('utf-8', errors='ignore')

    matches = 0
    for filename in known_filenames:
        if filename in decrypted_str:
            matches += 1

    return matches


def bruteforce_params(pvf_info, known_filenames):
    """暴力搜索参数"""
    print("[*] 开始参数搜索...")
    print(f"[*] 已知文件名样本: {len(known_filenames)} 个")
    print()

    encrypted_index = pvf_info['encrypted_index']
    index_count = pvf_info['index_count']
    index_crc32 = pvf_info['index_crc32']
    revision = pvf_info['revision']

    # 生成候选参数
    g_add_candidates = [
        ("0", 0),
        ("反编译值", 0x81A12D11),
        ("crc32", index_crc32),
        ("~crc32", ~index_crc32 & 0xFFFFFFFF),
        ("revision", revision),
        ("~revision", ~revision & 0xFFFFFFFF),
    ]

    xor_key_candidates = [
        ("0", 0),
        ("反编译值", 0x81A12D11),
        ("crc32", index_crc32),
        ("revision", revision),
    ]

    ror_shifts = [6, 13, 17, 26]  # 尝试不同的ROR偏移

    print(f"[*] 测试组合: {len(g_add_candidates)} g_add × {len(xor_key_candidates)} xor × {len(ror_shifts)} ror = {len(g_add_candidates) * len(xor_key_candidates) * len(ror_shifts)} 种")
    print()

    results = []

    for g_name, g_add in g_add_candidates:
        for x_name, xor_key in xor_key_candidates:
            for ror_shift in ror_shifts:
                # 解密
                decrypted, actual_crc = decrypt_index_with_params(
                    encrypted_index,
                    index_count,
                    index_crc32,
                    index_crc32,
                    g_add,
                    xor_key,
                    ror_shift
                )

                if decrypted is None:
                    continue

                # CRC32匹配
                crc_match = (actual_crc == index_crc32)

                # 文件名匹配数量
                filename_matches = check_if_contains_filenames(decrypted, known_filenames)

                # 计算得分
                score = 0
                if crc_match:
                    score += 100
                score += filename_matches * 10

                results.append({
                    'g_name': g_name,
                    'g_add': g_add,
                    'x_name': x_name,
                    'xor_key': xor_key,
                    'ror_shift': ror_shift,
                    'crc_match': crc_match,
                    'actual_crc': actual_crc,
                    'filename_matches': filename_matches,
                    'score': score,
                    'decrypted': decrypted
                })

    # 按分数排序
    results.sort(key=lambda x: x['score'], reverse=True)

    # 显示Top 10
    print("=" * 80)
    print("Top 10 参数组合（按得分排序）")
    print("=" * 80)
    print()
    print(f"{'排名':<4} {'分数':<6} {'CRC':<8} {'文件名':<6} {'参数组合'}")
    print("-" * 80)

    for i, result in enumerate(results[:10], 1):
        crc_status = "[+]" if result['crc_match'] else "[.]"

        print(f"{i:<4} {result['score']:<6} {crc_status:<8} "
              f"{result['filename_matches']:<6} "
              f"g_add={result['g_name']} (0x{result['g_add']:08x}), "
              f"xor={result['x_name']} (0x{result['xor_key']:08x}), "
              f"ror={result['ror_shift']}")

    print()

    # 详细分析最佳候选
    if results:
        best = results[0]
        print("=" * 80)
        print("最佳候选详细分析")
        print("=" * 80)
        print()
        print(f"参数组合:")
        print(f"  g_AdditionalKey: 0x{best['g_add']:08x} ({best['g_name']})")
        print(f"  XOR_KEY: 0x{best['xor_key']:08x} ({best['x_name']})")
        print(f"  ROR偏移: {best['ror_shift']}")
        print()
        print(f"验证结果:")
        print(f"  CRC32匹配: {'[+] 是' if best['crc_match'] else '[.] 否'}")
        print(f"  期望CRC32: 0x{index_crc32:08x}")
        print(f"  实际CRC32: 0x{best['actual_crc']:08x}")
        print(f"  文件名匹配: {best['filename_matches']} / {len(known_filenames)}")
        print()

        # 显示解密数据预览
        preview = best['decrypted'][:500]
        print(f"解密数据预览（前500字节）:")
        print(f"  十六进制: {preview.hex()[:100]}...")
        try:
            text_preview = preview.decode('utf-8', errors='ignore')
            print(f"  文本: {text_preview[:200]}")
        except:
            pass

        # 如果CRC32匹配，保存解密数据
        if best['crc_match']:
            print()
            print("[+] CRC32完美匹配！保存解密数据...")
            with open('decrypted_index_found.bin', 'wb') as f:
                f.write(best['decrypted'])
            print("[+] 已保存到: decrypted_index_found.bin")

            return True

    return False


def main():
    print("=" * 80)
    print("pvfUtility参数分析工具")
    print("=" * 80)
    print()

    # 1. 通过API获取已知文件名
    print("[*] 步骤1: 通过API获取已知文件名...")
    known_filenames = get_api_file_list()

    if not known_filenames:
        print("[!] 无法从API获取文件列表")
        print("[!] 请确保pvfUtility正在运行并且已打开Script.pvf")
        return False

    # 2. 读取Script.pvf索引
    print("[*] 步骤2: 读取Script.pvf索引...")
    pvf_path = "C:\\Users\\waw\\Desktop\\game\\Script.pvf"
    pvf_info = read_script_pvf_index(pvf_path)

    if not pvf_info:
        print("[!] 无法读取Script.pvf")
        return False

    # 3. 暴力搜索参数
    print("[*] 步骤3: 暴力搜索参数...")
    print()
    success = bruteforce_params(pvf_info, known_filenames)

    return success


if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
