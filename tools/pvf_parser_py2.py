#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Script.pvf Parser - Python 2.4 Compatible Version

Simplified version for CentOS 5 (Python 2.4.3) validation
"""

import struct
import sys
import zlib

class PVFHeader:
    """PVF文件头部结构"""
    def __init__(self):
        self.tag_length = 0
        self.tag_string = ""
        self.revision = 0
        self.index_header_size = 0
        self.index_crc32 = 0
        self.index_count = 0

class IndexEntry:
    """索引条目结构"""
    def __init__(self):
        self.name = ""
        self.offset = 0
        self.size = 0
        self.decrypt_key = 0
        self.hash_value = 0

class PVFParser:
    """Script.pvf解析器 - Python 2兼容"""

    EXPECTED_GUID = "fa08bf71-4395-6a4b-a3e3-2617c9fee116"

    def __init__(self, filepath):
        self.filepath = filepath
        self.header = None
        self.header_size = 0
        self.indexes = []
        self.fp = None

    def open_pvf(self):
        """打开PVF文件并解析头部"""
        try:
            self.fp = open(self.filepath, 'rb')

            # Step 1: 读取GUID标签长度
            tag_length_bytes = self.fp.read(4)
            if len(tag_length_bytes) != 4:
                print "[-] 无法读取标签长度"
                return False

            tag_length = struct.unpack('<I', tag_length_bytes)[0]
            print "[+] 标签长度: %d 字节" % tag_length

            # Step 2: 读取GUID字符串
            tag_string_bytes = self.fp.read(tag_length)
            if len(tag_string_bytes) != tag_length:
                print "[-] 无法读取完整标签 (期望 %d 字节)" % tag_length
                return False

            tag_string = tag_string_bytes.decode('utf-8', 'ignore')
            print "[+] GUID: %s" % tag_string

            # 验证GUID
            if tag_string != self.EXPECTED_GUID:
                print "[!] 警告: GUID不匹配"
                print "   期望: %s" % self.EXPECTED_GUID
                print "   实际: %s" % tag_string

            # Step 3: 读取版本号
            revision_bytes = self.fp.read(4)
            revision = struct.unpack('<I', revision_bytes)[0]
            print "[+] 版本号: %d" % revision

            # Step 4: 读取索引头部大小
            index_header_size_bytes = self.fp.read(4)
            index_header_size = struct.unpack('<I', index_header_size_bytes)[0]
            print "[+] 索引头部大小: %d 字节" % index_header_size

            # Step 5: 读取索引CRC32
            index_crc32_bytes = self.fp.read(4)
            index_crc32 = struct.unpack('<I', index_crc32_bytes)[0]
            print "[+] 索引CRC32: 0x%08x" % index_crc32

            # Step 6: 读取索引数量
            index_count_bytes = self.fp.read(4)
            index_count = struct.unpack('<I', index_count_bytes)[0]
            print "[+] 文件数量: %d" % index_count

            # 保存头部信息
            self.header = PVFHeader()
            self.header.tag_length = tag_length
            self.header.tag_string = tag_string
            self.header.revision = revision
            self.header.index_header_size = index_header_size
            self.header.index_crc32 = index_crc32
            self.header.index_count = index_count

            # 计算头部结束位置
            self.header_size = 4 + tag_length + 4 + 4 + 4 + 4
            print "[+] 头部结束位置: 0x%08x (%d 字节)" % (self.header_size, self.header_size)

            return True

        except Exception, e:
            print "[-] 打开PVF失败: %s" % str(e)
            return False

    def _ror32(self, value, shift):
        """32位右旋转"""
        return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF

    def _decrypt_index(self, encrypted_data, crc_init, checksum, key):
        """解密索引数据"""
        size = len(encrypted_data)

        # 检查对齐（必须是4的倍数）
        if (size & 3) != 0:
            print "[!] 索引数据大小 %d 不是4的倍数" % size
            return None

        # 生成CRC32表（标准多项式0xEDB88320）
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
        crc = (~crc_init) & 0xFFFFFFFF

        # 关键参数
        XOR_KEY = 0
        g_AdditionalKey = 0x81A79011

        # 修改密钥
        if XOR_KEY:
            key ^= XOR_KEY

        # 解密
        decrypted = list(encrypted_data)  # Python 2.4 needs list

        for i in range(0, size, 4):
            # 读取4字节
            dword = struct.unpack('<I', ''.join(decrypted[i:i+4]))[0]

            # 步骤1: XOR g_AdditionalKey
            if g_AdditionalKey:
                dword ^= g_AdditionalKey

            # 步骤2: ROR(key ^ data, 6) 解密
            dword = self._ror32(key ^ dword, 6)

            # 写回
            packed = struct.pack('<I', dword)
            for j in range(4):
                decrypted[i + j] = packed[j]

            # 更新CRC32
            for j in range(4):
                byte = ord(decrypted[i + j])
                crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF]

        # 验证CRC32
        final_crc = (~crc) & 0xFFFFFFFF

        if final_crc != checksum:
            print "[!] 索引CRC32不匹配:"
            print "    期望: 0x%08x" % checksum
            print "    实际: 0x%08x" % final_crc
            return None

        print "[+] 索引CRC32验证通过: 0x%08x" % final_crc
        return ''.join(decrypted)

    def parse_index(self, max_entries=10):
        """解析索引表（只解析前N个条目用于验证）"""
        if not self.header:
            print "[-] 头部未解析"
            return False

        try:
            # Step 1: 读取加密的索引数据
            print "\n[*] 读取加密索引数据 (%d 字节)..." % self.header.index_header_size
            encrypted_index = self.fp.read(self.header.index_header_size)

            if len(encrypted_index) != self.header.index_header_size:
                print "[-] 索引数据不完整"
                return False

            # Step 2: 解密索引
            print "[*] 解密索引数据..."
            decrypted_index = self._decrypt_index(
                encrypted_index,
                self.header.index_count,  # CRC初始值 = 文件数量
                self.header.index_crc32,  # 期望的CRC32
                self.header.index_crc32   # 解密密钥 = index_crc32
            )

            if decrypted_index is None:
                print "[-] 索引解密失败"
                return False

            print "[+] 索引解密成功"

            # Step 3: 解析前N个索引条目（用于验证）
            offset = 0
            parse_count = min(max_entries, self.header.index_count)

            print "\n[*] 解析前 %d 个索引条目..." % parse_count

            for i in range(parse_count):
                try:
                    # 读取哈希值 (4 bytes)
                    if offset + 4 > len(decrypted_index):
                        print "[!] 索引 %d: 数据不足，无法读取哈希值" % i
                        break

                    hash_value = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 读取文件名长度
                    if offset + 4 > len(decrypted_index):
                        print "[!] 索引 %d: 数据不足，无法读取文件名长度" % i
                        break

                    name_length = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 读取文件名
                    if offset + name_length > len(decrypted_index):
                        print "[!] 索引 %d: 数据不足，无法读取文件名" % i
                        break

                    name_bytes = decrypted_index[offset:offset+name_length]
                    offset += name_length

                    # 解码文件名
                    name = name_bytes.decode('utf-8', 'ignore')

                    # 读取文件信息 (12 bytes)
                    if offset + 12 > len(decrypted_index):
                        print "[!] 索引 %d: 数据不足，无法读取文件信息" % i
                        break

                    file_size = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4
                    decrypt_key = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4
                    file_offset = struct.unpack('<I', decrypted_index[offset:offset+4])[0]
                    offset += 4

                    # 创建索引条目
                    entry = IndexEntry()
                    entry.hash_value = hash_value
                    entry.name = name
                    entry.offset = file_offset
                    entry.size = file_size
                    entry.decrypt_key = decrypt_key

                    self.indexes.append(entry)

                    # 打印条目信息
                    print "[%d] %s" % (i, name)
                    print "    Hash: 0x%08x, Size: %d, DecryptKey: 0x%08x, Offset: %d" % (
                        hash_value, file_size, decrypt_key, file_offset
                    )

                except Exception, e:
                    print "[!] 解析索引 %d 失败: %s" % (i, str(e))
                    break

            print "\n[+] 成功解析 %d/%d 个索引条目" % (len(self.indexes), self.header.index_count)
            return True

        except Exception, e:
            print "[-] 解析索引失败: %s" % str(e)
            return False

    def close(self):
        """关闭文件"""
        if self.fp:
            self.fp.close()
            self.fp = None

def main():
    if len(sys.argv) < 2:
        print "Usage: %s <path_to_Script.pvf>" % sys.argv[0]
        print ""
        print "Example:"
        print "    python %s /home/neople/game/Script.pvf" % sys.argv[0]
        return 1

    pvf_path = sys.argv[1]

    print "=" * 80
    print "Script.pvf Parser - Python 2.4 Compatible"
    print "=" * 80
    print ""

    parser = PVFParser(pvf_path)

    # 解析头部
    if not parser.open_pvf():
        return 1

    # 解析索引（只解析前10个条目用于验证）
    if not parser.parse_index(max_entries=10):
        return 1

    parser.close()

    print ""
    print "=" * 80
    print "[SUCCESS] PVF解析验证通过！"
    print "=" * 80

    return 0

if __name__ == '__main__':
    sys.exit(main())
