#!/usr/bin/env python3
"""
test_protocol_format.py - 测试不同协议格式

尝试多种协议头格式来确定真实的协议结构
"""

import socket
import struct
import sys
import time

def hexdump(data, prefix=""):
    """格式化 hex dump"""
    result = []
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_str = ' '.join(f'{b:02X}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        result.append(f"{prefix}{i:04X}  {hex_str:<48}  {ascii_str}")
    return '\n'.join(result)


def create_packet_6byte(category, packet_id, payload=b''):
    """
    创建 6 字节 tagPacketHeader 格式数据包

    结构:
      [0:1] category   - 路由分类
      [1:1] packet_id  - 消息号 (0-255)
      [2:4] size       - 包大小 (小端序, 含header)
    """
    total_len = 6 + len(payload)
    header = bytearray(6)
    header[0] = category
    header[1] = packet_id
    struct.pack_into('<I', header, 2, total_len)
    return bytes(header) + payload


def create_packet_10byte(packet_id, payload=b''):
    """
    创建 10 字节 PacketHeader 格式数据包 (服务器内部)

    结构:
      [0:2] packet_id  - 消息号 (0-65535, 小端序)
      [2:2] size       - 包大小 (小端序, 含header)
      [4:2] reserved
      [6:4] unknown
    """
    total_len = 10 + len(payload)
    header = bytearray(10)
    struct.pack_into('<H', header, 0, packet_id)
    struct.pack_into('<H', header, 2, total_len)
    struct.pack_into('<H', header, 4, 0)  # reserved
    struct.pack_into('<I', header, 6, 0)  # unknown
    return bytes(header) + payload


def create_packet_13byte(msg_no, payload=b'', seq=0):
    """
    创建 13 字节格式数据包 (当前开发版)

    结构:
      [0:1]  classification
      [1:4]  length
      [5:1]  msg_no
      [6:1]  reserved
      [7:4]  checksum
      [11:2] sequence
    """
    total_len = 13 + len(payload)
    header = bytearray(13)
    header[0] = 0  # classification
    struct.pack_into('<I', header, 1, total_len)
    header[5] = msg_no
    header[6] = 0  # reserved
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, seq)
    return bytes(header) + payload


def test_format(host, port, name, packet):
    """测试单个格式"""
    print(f"\n{'='*60}")
    print(f"Testing: {name}")
    print(f"{'='*60}")
    print(f"Packet ({len(packet)} bytes):")
    print(hexdump(packet, "  "))

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print(f"\n[OK] Connected to {host}:{port}")

        sock.send(packet)
        print(f"[OK] Sent {len(packet)} bytes")

        # 等待响应
        time.sleep(0.5)
        try:
            response = sock.recv(4096)
            if response:
                print(f"\n[OK] Received {len(response)} bytes:")
                print(hexdump(response, "  "))
                return True
            else:
                print(f"\n[!] Connection closed by server (no data)")
        except socket.timeout:
            print(f"\n[!] Timeout - no response")

    except Exception as e:
        print(f"\n[ERROR] {e}")
    finally:
        sock.close()

    return False


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <host:port>")
        print(f"Example: {sys.argv[0]} 192.168.200.131:7000")
        sys.exit(1)

    target = sys.argv[1]
    host, port = target.split(':')
    port = int(port)

    print(f"Testing protocol formats against {host}:{port}")
    print(f"="*60)

    # 测试不同格式
    tests = [
        # 6字节格式 - category=0 (内部), packet_id=0 (心跳?)
        ("6-byte: category=0, packet_id=0", create_packet_6byte(0, 0)),

        # 6字节格式 - category=1 (Channel), packet_id=0
        ("6-byte: category=1, packet_id=0", create_packet_6byte(1, 0)),

        # 6字节格式 - category=1, packet_id=1 (可能是登录)
        ("6-byte: category=1, packet_id=1", create_packet_6byte(1, 1)),

        # 6字节格式 - category=1, packet_id=2 (CS_UPDATE_CHANNEL_INFO)
        ("6-byte: category=1, packet_id=2", create_packet_6byte(1, 2)),

        # 6字节格式 - category=1, packet_id=5 (CS_CHECK_SCRIPT_VERSION)
        ("6-byte: category=1, packet_id=5", create_packet_6byte(1, 5)),

        # 10字节格式 - packet_id=0
        ("10-byte: packet_id=0", create_packet_10byte(0)),

        # 10字节格式 - packet_id=1
        ("10-byte: packet_id=1", create_packet_10byte(1)),

        # 13字节格式 - msg_no=0
        ("13-byte: msg_no=0", create_packet_13byte(0)),
    ]

    results = []
    for name, packet in tests:
        success = test_format(host, port, name, packet)
        results.append((name, success))
        time.sleep(0.5)  # 避免太快

    # 汇总结果
    print(f"\n{'='*60}")
    print("SUMMARY")
    print(f"{'='*60}")
    for name, success in results:
        status = "✓ Got Response" if success else "✗ No Response/Disconnected"
        print(f"  {status}: {name}")


if __name__ == '__main__':
    main()
