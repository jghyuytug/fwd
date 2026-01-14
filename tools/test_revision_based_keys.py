#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试基于revision的参数变换

假设: g_AdditionalKey 可能是 revision 的某种变换
"""

import struct
import sys


def make_crc_table():
    """生成CRC32表"""
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


def ror32(value, shift):
    """32位右旋转"""
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF


def decrypt_index_with_params(encrypted_data, crc_init, checksum, key,
                                g_additional_key, xor_key, ror_shift=6):
    """用指定参数解密索引"""
    size = len(encrypted_data)

    if (size & 3) != 0:
        return None, 0

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


def check_if_plaintext(decrypted_data):
    """检查解密数据是否像明文索引"""
    # 检查前100个字节
    if len(decrypted_data) < 100:
        return False, 0

    # 尝试解析第一个索引条目
    try:
        offset = 0
        hash_value = struct.unpack('<I', decrypted_data[offset:offset+4])[0]
        name_length = struct.unpack('<I', decrypted_data[offset+4:offset+8])[0]

        # 文件名长度应该在合理范围内
        if not (5 <= name_length <= 200):
            return False, 0

        # 尝试解码文件名
        filename = decrypted_data[offset+8:offset+8+name_length].decode('utf-8', errors='strict')

        # 检查是否像文件路径
        if ('/' in filename or '\\' in filename) and '.' in filename:
            if all(c.isprintable() or c in ['\r', '\n'] for c in filename):
                return True, name_length

    except:
        pass

    return False, 0


def test_revision_based_keys(pvf_path, revision):
    """测试基于revision的参数"""
    print("=" * 80)
    print(f"测试基于 revision={revision} 的参数")
    print("=" * 80)
    print()

    # 读取加密索引
    with open(pvf_path, 'rb') as f:
        tag_length = struct.unpack('<I', f.read(4))[0]
        tag_string = f.read(tag_length).decode('utf-8')
        file_revision = struct.unpack('<I', f.read(4))[0]
        index_header_size = struct.unpack('<I', f.read(4))[0]
        index_crc32 = struct.unpack('<I', f.read(4))[0]
        index_count = struct.unpack('<I', f.read(4))[0]

        print(f"PVF信息:")
        print(f"  Revision: {file_revision}")
        print(f"  文件数量: {index_count:,}")
        print(f"  索引CRC32: 0x{index_crc32:08x}")
        print()

        # 只读取前10KB用于测试
        test_size = min(10240, index_header_size)
        encrypted_index = f.read(test_size)

    # 生成候选参数
    candidates = []

    # 1. Revision的直接变换
    candidates.append(("revision", revision))
    candidates.append(("~revision", ~revision & 0xFFFFFFFF))
    candidates.append(("revision << 8", (revision << 8) & 0xFFFFFFFF))
    candidates.append(("revision << 16", (revision << 16) & 0xFFFFFFFF))
    candidates.append(("revision * 2", (revision * 2) & 0xFFFFFFFF))
    candidates.append(("revision * 33", (revision * 33) & 0xFFFFFFFF))  # DJB2乘数
    candidates.append(("revision * 5381", (revision * 5381) & 0xFFFFFFFF))  # DJB2初始值

    # 2. Revision与固定值的组合
    base_key = 0x81A12D11  # 反编译代码中的值
    candidates.append(("base ^ revision", base_key ^ revision))
    candidates.append(("base + revision", (base_key + revision) & 0xFFFFFFFF))
    candidates.append(("base - revision", (base_key - revision) & 0xFFFFFFFF))
    candidates.append(("base * revision", (base_key * revision) & 0xFFFFFFFF))

    # 3. Revision与CRC32的组合
    candidates.append(("crc32 ^ revision", index_crc32 ^ revision))
    candidates.append(("crc32 + revision", (index_crc32 + revision) & 0xFFFFFFFF))
    candidates.append(("crc32 - revision", (index_crc32 - revision) & 0xFFFFFFFF))
    candidates.append(("crc32 * revision", (index_crc32 * revision) & 0xFFFFFFFF))

    # 4. 复杂变换
    candidates.append(("ROR(revision, 6)", ror32(revision, 6)))
    candidates.append(("ROR(revision, 13)", ror32(revision, 13)))
    candidates.append(("ROR(base, revision&31)", ror32(base_key, revision & 31)))

    # 5. 基于CRC32(revision)
    crc_table = make_crc_table()
    revision_bytes = struct.pack('<I', revision)
    crc = 0xFFFFFFFF
    for byte in revision_bytes:
        crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]
    crc32_of_revision = ~crc & 0xFFFFFFFF
    candidates.append(("CRC32(revision)", crc32_of_revision))
    candidates.append(("~CRC32(revision)", ~crc32_of_revision & 0xFFFFFFFF))

    print(f"生成 {len(candidates)} 个候选参数")
    print()

    # 测试每个候选
    results = []

    for name, g_add in candidates:
        # 测试不同的ROR偏移
        for ror_shift in [6, 13, 17, 26]:
            # 解密
            decrypted, actual_crc = decrypt_index_with_params(
                encrypted_index,
                index_count,
                index_crc32,
                index_crc32,
                g_add,
                0,  # XOR_KEY = 0
                ror_shift
            )

            if decrypted is None:
                continue

            # 检查CRC32
            crc_match = (actual_crc == index_crc32)

            # 检查是否像明文
            is_plaintext, name_len = check_if_plaintext(decrypted)

            # 计算得分
            score = 0
            if crc_match:
                score += 100
            if is_plaintext:
                score += 50

            if score > 0:
                results.append({
                    'name': name,
                    'g_add': g_add,
                    'ror_shift': ror_shift,
                    'crc_match': crc_match,
                    'is_plaintext': is_plaintext,
                    'name_len': name_len,
                    'score': score,
                    'decrypted': decrypted
                })

    # 按分数排序
    results.sort(key=lambda x: x['score'], reverse=True)

    # 显示结果
    print("=" * 80)
    print("测试结果")
    print("=" * 80)
    print()

    if results:
        print(f"找到 {len(results)} 个可能的参数组合:")
        print()
        print(f"{'排名':<4} {'得分':<6} {'CRC':<8} {'明文':<8} {'参数名称':<30} {'ROR'}")
        print("-" * 80)

        for i, r in enumerate(results[:20], 1):
            crc_s = "[+]" if r['crc_match'] else "[.]"
            plain_s = "[+]" if r['is_plaintext'] else "[.]"

            print(f"{i:<4} {r['score']:<6} {crc_s:<8} {plain_s:<8} "
                  f"{r['name']:<30} {r['ror_shift']}")

        print()
        print("=" * 80)
        print("最佳候选详细信息")
        print("=" * 80)
        print()

        best = results[0]
        print(f"参数:")
        print(f"  名称: {best['name']}")
        print(f"  g_AdditionalKey: 0x{best['g_add']:08x}")
        print(f"  ROR偏移: {best['ror_shift']}")
        print()
        print(f"验证:")
        print(f"  CRC32匹配: {'[+] 是' if best['crc_match'] else '[.] 否'}")
        print(f"  明文验证: {'[+] 是' if best['is_plaintext'] else '[.] 否'}")
        if best['is_plaintext']:
            print(f"  第一个文件名长度: {best['name_len']}")
        print()

        # 显示解密数据预览
        preview = best['decrypted'][:200]
        print(f"解密数据预览（前200字节）:")
        print(f"  十六进制: {preview.hex()}")
        try:
            # 尝试解析第一个文件名
            name_len = struct.unpack('<I', preview[4:8])[0]
            if 5 <= name_len <= 200:
                filename = preview[8:8+name_len].decode('utf-8', errors='ignore')
                print(f"  第一个文件名: {filename}")
        except:
            pass

        # 如果CRC32匹配，保存
        if best['crc_match']:
            print()
            print("[+] CRC32完美匹配！这可能是正确的参数！")
            with open('decrypted_index_by_revision.bin', 'wb') as f:
                # 解密完整索引
                with open(pvf_path, 'rb') as pf:
                    pf.seek(56)  # 跳过头部
                    full_encrypted = pf.read(index_header_size)

                full_decrypted, _ = decrypt_index_with_params(
                    full_encrypted,
                    index_count,
                    index_crc32,
                    index_crc32,
                    best['g_add'],
                    0,
                    best['ror_shift']
                )
                f.write(full_decrypted)
            print("[+] 已保存完整解密索引到: decrypted_index_by_revision.bin")

            return True

    else:
        print("[-] 未找到任何匹配的参数组合")

    return False


def main():
    pvf_path = "C:\\Users\\waw\\Desktop\\game\\Script.pvf"
    revision = 48984

    success = test_revision_based_keys(pvf_path, revision)
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
