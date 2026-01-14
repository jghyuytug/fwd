#!/usr/bin/env python3
"""分析 Bridge 响应数据"""

import struct

# 收到的响应
response = bytes.fromhex('7C 06 1F 00 00 00 08 0C 87 28 08 00 00 00 00 35 39 00 A4 0D 00 00 00 0C 87 28 08 06 00 00 00'.replace(' ', ''))

print(f"Response length: {len(response)} bytes")
print(f"Raw hex: {response.hex()}")
print()

# 尝试不同的解析方式

print("=== Parsing Attempt 1: tagPacketHeader (6-byte) ===")
if len(response) >= 6:
    category = response[0]
    packet_id = response[1]
    size = struct.unpack('<I', response[2:6])[0]
    print(f"  category:  {category} (0x{category:02X})")
    print(f"  packet_id: {packet_id} (0x{packet_id:02X})")
    print(f"  size:      {size}")
    print(f"  size match: {size == len(response)}")
    if size == len(response):
        print(f"  payload:   {response[6:].hex()}")
print()

print("=== Parsing Attempt 2: Response with marker ===")
# 假设第一个字节是标记，后面是正常结构
if len(response) >= 6:
    marker = response[0]
    category = response[1]
    size = struct.unpack('<I', response[2:6])[0]
    print(f"  marker:    {marker} (0x{marker:02X}) = '|' (ASCII)")
    print(f"  category:  {category} (0x{category:02X})")
    print(f"  size:      {size}")
    print(f"  size match: {size == len(response)}")
print()

print("=== Parsing Attempt 3: Check for script version response ===")
# 查看 payload 内容
print(f"  Bytes 6-10:  {response[6:10].hex()}")
print(f"  Bytes 10-14: {response[10:14].hex()}")
print(f"  Bytes 14-18: {response[14:18].hex()}")
print(f"  Bytes 18-22: {response[18:22].hex()}")
print(f"  Bytes 22-26: {response[22:26].hex()}")
print(f"  Bytes 26-31: {response[26:31].hex()}")
print()

# 检查是否包含可打印字符串
print("=== Check for strings ===")
for i in range(len(response)):
    if 0x30 <= response[i] <= 0x39 or 0x41 <= response[i] <= 0x5A or 0x61 <= response[i] <= 0x7A:
        print(f"  Offset {i}: 0x{response[i]:02X} = '{chr(response[i])}'")

print()
print("=== Key finding ===")
print(f"  Byte 0: 0x7C = '|' (124) - likely a marker or special byte")
print(f"  Byte 1: 0x06 = 6 - packet_id for response")
print(f"  Bytes 2-5: 0x{struct.unpack('<I', response[2:6])[0]:08X} = {struct.unpack('<I', response[2:6])[0]} - SIZE MATCHES RESPONSE LENGTH!")
