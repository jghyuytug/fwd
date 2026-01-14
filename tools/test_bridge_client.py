#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_bridge_client.py - Bridge Protocol Test Client

测试 Bridge 服务的基本协议：
1. TCP 连接
2. 发送/接收数据包
3. 协议验证
"""

import socket
import struct
import sys
import time

# 数据包头格式 (13 bytes)
# classification(1) + length(4) + msg_no(1) + reserved(1) + checksum(4) + sequence(2)
PACKET_HEADER_SIZE = 13

def create_packet_header(msg_no, data_len, seq=0):
    """创建数据包头"""
    total_len = PACKET_HEADER_SIZE + data_len
    # Pack: classification(B) + length(I) + msg_no(B) + reserved(B) + checksum(I) + sequence(H)
    header = struct.pack('<BIBBIIH',
        0,          # classification
        total_len,  # length (little endian)
        msg_no,     # msg_no
        0,          # reserved
        0,          # checksum
        seq         # sequence
    )
    # 注意: 实际头部是13字节，上面pack是14字节，需要调整
    # 正确格式: B(1) + I(4) + B(1) + B(1) + I(4) + H(2) = 13 bytes
    header = struct.pack('<BIBBIIH',
        0,          # classification (1)
        total_len,  # length (4)
        msg_no,     # msg_no (1)
        0,          # reserved (1)
        0,          # checksum (4)
        seq         # sequence (2)
    )
    # Actually 1+4+1+1+4+2 = 13
    return header[:PACKET_HEADER_SIZE]


def parse_packet_header(data):
    """解析数据包头"""
    if len(data) < PACKET_HEADER_SIZE:
        return None

    # 手动解析
    classification = data[0]
    length = struct.unpack('<I', data[1:5])[0]
    msg_no = data[5]
    reserved = data[6]
    checksum = struct.unpack('<I', data[7:11])[0]
    sequence = struct.unpack('<H', data[11:13])[0]

    return {
        'classification': classification,
        'length': length,
        'msg_no': msg_no,
        'reserved': reserved,
        'checksum': checksum,
        'sequence': sequence
    }


def test_connection(host, port):
    """测试基本连接"""
    print(f"\n=== Testing connection to {host}:{port} ===")

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print(f"[OK] Connected to {host}:{port}")
        return sock
    except Exception as e:
        print(f"[FAIL] Connection failed: {e}")
        return None


def test_script_version(sock):
    """测试脚本版本检查协议 (msg_no=2)"""
    print("\n=== Testing CS_CHECK_SCRIPT_VERSION ===")

    # 构建请求: header + version(4)
    version = 1
    data = struct.pack('>I', version)  # network byte order

    # 创建头部
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0  # classification
    struct.pack_into('<I', header, 1, PACKET_HEADER_SIZE + len(data))  # length
    header[5] = 2  # msg_no = 2 (CHECK_SCRIPT_VERSION)
    header[6] = 0  # reserved
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, 0)  # sequence

    packet = bytes(header) + data
    print(f"Sending: {packet.hex()}")
    print(f"  Header: classification=0 length={PACKET_HEADER_SIZE + len(data)} msg_no=2")
    print(f"  Data: version={version}")

    sock.send(packet)

    # 接收响应
    try:
        response = sock.recv(1024)
        print(f"Received: {response.hex()}")

        if len(response) >= PACKET_HEADER_SIZE:
            resp_header = parse_packet_header(response)
            print(f"  Response header: {resp_header}")

            if resp_header['length'] > PACKET_HEADER_SIZE:
                resp_data = response[PACKET_HEADER_SIZE:resp_header['length']]
                print(f"  Response data: {resp_data.hex()}")

                if len(resp_data) >= 5:
                    server_version = struct.unpack('>I', resp_data[0:4])[0]
                    need_update = resp_data[4]
                    print(f"  Server version: {server_version}, Need update: {need_update}")
                    return True

        return False
    except socket.timeout:
        print("[TIMEOUT] No response received")
        return False


def test_channel_list(sock):
    """测试请求频道列表 (msg_no=3)"""
    print("\n=== Testing CS_NOTICE_CHANNEL_SERVER ===")

    # 创建头部 (无数据)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0  # classification
    struct.pack_into('<I', header, 1, PACKET_HEADER_SIZE)  # length
    header[5] = 3  # msg_no = 3 (NOTICE_CHANNEL_SERVER)
    header[6] = 0  # reserved
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, 0)  # sequence

    packet = bytes(header)
    print(f"Sending: {packet.hex()}")
    print(f"  Header: classification=0 length={PACKET_HEADER_SIZE} msg_no=3")

    sock.send(packet)

    # 接收响应
    try:
        response = sock.recv(2048)
        print(f"Received: {response[:50].hex()}... (total {len(response)} bytes)")

        if len(response) >= PACKET_HEADER_SIZE:
            resp_header = parse_packet_header(response)
            print(f"  Response header: {resp_header}")

            if resp_header['length'] > PACKET_HEADER_SIZE:
                resp_data = response[PACKET_HEADER_SIZE:resp_header['length']]
                print(f"  Response data length: {len(resp_data)}")

                if len(resp_data) >= 4:
                    count = struct.unpack('>I', resp_data[0:4])[0]
                    print(f"  Channel count: {count}")

                    # 解析每个频道
                    offset = 4
                    for i in range(count):
                        if offset + 64 <= len(resp_data):
                            ch_id = struct.unpack('>I', resp_data[offset:offset+4])[0]
                            ch_name = resp_data[offset+4:offset+36].decode('utf-8', errors='ignore').rstrip('\x00')
                            ch_ip = resp_data[offset+36:offset+52].decode('utf-8', errors='ignore').rstrip('\x00')
                            ch_port = struct.unpack('>I', resp_data[offset+52:offset+56])[0]
                            ch_users = struct.unpack('>I', resp_data[offset+56:offset+60])[0]
                            ch_max = struct.unpack('>I', resp_data[offset+60:offset+64])[0]
                            print(f"  Channel {i+1}: id={ch_id} name='{ch_name}' ip={ch_ip} port={ch_port} users={ch_users}/{ch_max}")
                            offset += 64
                    return True

        return False
    except socket.timeout:
        print("[TIMEOUT] No response received")
        return False


def test_channel_register(sock, channel_id=1, channel_ip="127.0.0.1", channel_port=7001):
    """测试 Channel 注册协议 (msg_no=0 via registerHandlerExtra)"""
    print("\n=== Testing CS_UPDATE_CHANNEL_INFO ===")

    # 构建注册数据: channel_id(4) + channel_ip(64) + channel_port(4) = 72 bytes
    data = bytearray(72)
    struct.pack_into('>I', data, 0, channel_id)  # channel_id (network order)
    ip_bytes = channel_ip.encode('utf-8')[:63]
    data[4:4+len(ip_bytes)] = ip_bytes  # channel_ip
    struct.pack_into('>I', data, 68, channel_port)  # channel_port (network order)

    # 创建头部 (注意: 这是 Extra handler, classification 可能不同)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 1  # classification = 1 (for Extra handlers)
    struct.pack_into('<I', header, 1, PACKET_HEADER_SIZE + len(data))  # length
    header[5] = 0  # msg_no = 0 (UPDATE_CHANNEL_INFO)
    header[6] = 0  # reserved
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, 0)  # sequence

    packet = bytes(header) + bytes(data)
    print(f"Sending: {packet[:30].hex()}... (total {len(packet)} bytes)")
    print(f"  Header: classification=1 length={PACKET_HEADER_SIZE + len(data)} msg_no=0")
    print(f"  Data: channel_id={channel_id} ip={channel_ip} port={channel_port}")

    sock.send(packet)

    # 接收响应
    try:
        response = sock.recv(1024)
        print(f"Received: {response.hex()}")

        if len(response) >= PACKET_HEADER_SIZE:
            resp_header = parse_packet_header(response)
            print(f"  Response header: {resp_header}")
            return True

        return False
    except socket.timeout:
        print("[TIMEOUT] No response received (may be expected)")
        return True  # Registration might not send response


def main():
    if len(sys.argv) < 2:
        print("Usage: python test_bridge_client.py <host:port>")
        print("Example: python test_bridge_client.py 192.168.200.128:17000")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 7000

    print(f"DNF Bridge Protocol Test Client")
    print(f"Target: {host}:{port}")
    print("=" * 50)

    # 测试连接
    sock = test_connection(host, port)
    if not sock:
        sys.exit(1)

    try:
        # 等待一下让服务器准备好
        time.sleep(0.5)

        # 测试脚本版本检查
        test_script_version(sock)

        time.sleep(0.5)

        # 测试请求频道列表
        test_channel_list(sock)

        time.sleep(0.5)

        # 测试 Channel 注册 (需要 Extra handler)
        # test_channel_register(sock)

    finally:
        sock.close()
        print("\n=== Test completed ===")


if __name__ == '__main__':
    main()
