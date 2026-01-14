#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_game_client.py - Game Protocol Test Client

测试 Game 服务的协议：
1. CHECK_CONNECTION (msg_no=0)
2. LOGIN (msg_no=1)
3. SELECT_CHARACTER (msg_no=4)
4. START_GAME (msg_no=15)
"""

import socket
import struct
import sys
import time

# 数据包头格式 (13 bytes)
PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    """创建数据包"""
    total_len = PACKET_HEADER_SIZE + len(data)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0  # classification
    struct.pack_into('<I', header, 1, total_len)  # length (little-endian)
    header[5] = msg_no  # msg_no
    header[6] = 0  # reserved
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, seq)  # sequence
    return bytes(header) + data


def parse_response(data):
    """解析响应"""
    if len(data) < PACKET_HEADER_SIZE:
        return None, None

    classification = data[0]
    length = struct.unpack('<I', data[1:5])[0]
    msg_no = data[5]
    reserved = data[6]
    checksum = struct.unpack('<I', data[7:11])[0]
    sequence = struct.unpack('<H', data[11:13])[0]

    header = {
        'classification': classification,
        'length': length,
        'msg_no': msg_no,
        'reserved': reserved,
        'checksum': checksum,
        'sequence': sequence
    }

    payload = data[PACKET_HEADER_SIZE:length] if length > PACKET_HEADER_SIZE else b''
    return header, payload


def test_check_connection(sock, seq):
    """测试 CHECK_CONNECTION (msg_no=0)"""
    print("\n=== Testing CHECK_CONNECTION (msg_no=0) ===")

    packet = create_packet(0, b'', seq)
    print(f"Sending: {packet.hex()}")
    sock.send(packet)

    try:
        response = sock.recv(1024)
        print(f"Received: {response.hex()}")
        header, payload = parse_response(response)
        if header:
            print(f"  Header: msg_no={header['msg_no']} length={header['length']}")
            if payload:
                print(f"  Payload: {payload.hex()}")
            return True
    except socket.timeout:
        print("[TIMEOUT] No response")
    return False


def test_login(sock, seq, account_id=1001):
    """测试 LOGIN (msg_no=1)"""
    print("\n=== Testing LOGIN (msg_no=1) ===")

    # Login 数据: account_id(4) + session_key(32) + version(4)
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)  # account_id
    session_key = b'TESTSESSIONKEY00' * 2  # 32 bytes
    data[4:36] = session_key
    struct.pack_into('<I', data, 36, 1)  # version

    packet = create_packet(1, bytes(data), seq)
    print(f"Sending login packet (account_id={account_id})")
    print(f"  Packet: {packet[:30].hex()}... ({len(packet)} bytes)")
    sock.send(packet)

    try:
        response = sock.recv(2048)
        print(f"Received: {len(response)} bytes")
        print(f"  Raw: {response[:100].hex()}...")

        header, payload = parse_response(response)
        if header:
            print(f"  Header: msg_no={header['msg_no']} length={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result code: {result}")

                # 解析角色列表
                if len(payload) > 8:
                    char_count = struct.unpack('<I', payload[4:8])[0]
                    print(f"  Character count: {char_count}")

                    # 简单解析角色名
                    offset = 8
                    for i in range(min(char_count, 5)):
                        if offset + 64 <= len(payload):
                            char_name = payload[offset:offset+32]
                            # 尝试解码
                            try:
                                name = char_name.split(b'\x00')[0].decode('utf-8')
                                print(f"  Character {i+1}: {name}")
                            except:
                                print(f"  Character {i+1}: (raw) {char_name[:20].hex()}")
                            offset += 64
                return True
    except socket.timeout:
        print("[TIMEOUT] No response")
    return False


def test_select_character(sock, seq, char_index=0):
    """测试 SELECT_CHARACTER (msg_no=4)"""
    print("\n=== Testing SELECT_CHARACTER (msg_no=4) ===")

    # Select character data: char_index(4)
    data = struct.pack('<I', char_index)

    packet = create_packet(4, data, seq)
    print(f"Sending select character (index={char_index})")
    sock.send(packet)

    try:
        response = sock.recv(4096)
        print(f"Received: {len(response)} bytes")

        header, payload = parse_response(response)
        if header:
            print(f"  Header: msg_no={header['msg_no']} length={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result code: {result}")

                if len(payload) > 100:
                    print(f"  Payload size: {len(payload)} bytes (contains character data)")
            return True
    except socket.timeout:
        print("[TIMEOUT] No response")
    return False


def test_start_game(sock, seq):
    """测试 START_GAME (msg_no=15)"""
    print("\n=== Testing START_GAME (msg_no=15) ===")

    # Start game data: dungeon_id(4) + difficulty(4)
    data = struct.pack('<II', 1, 0)  # dungeon 1, difficulty 0

    packet = create_packet(15, data, seq)
    print(f"Sending start game")
    sock.send(packet)

    try:
        response = sock.recv(2048)
        print(f"Received: {len(response)} bytes")

        header, payload = parse_response(response)
        if header:
            print(f"  Header: msg_no={header['msg_no']} length={header['length']}")
            if payload:
                print(f"  Payload: {payload[:50].hex()}...")
            return True
    except socket.timeout:
        print("[TIMEOUT] No response")
    return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python test_game_client.py <host:port> [account_id]")
        print("Example: python test_game_client.py 192.168.200.131:20011 1001")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 10011  # default game port

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1001

    print(f"DNF Game Protocol Test Client")
    print(f"Target: {host}:{port}")
    print(f"Account ID: {account_id}")
    print("=" * 50)

    # 连接
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print(f"[OK] Connected to {host}:{port}")
    except Exception as e:
        print(f"[FAIL] Connection failed: {e}")
        sys.exit(1)

    seq = 0

    try:
        # 测试 CHECK_CONNECTION
        time.sleep(0.3)
        test_check_connection(sock, seq)
        seq += 1

        # 测试 LOGIN
        time.sleep(0.3)
        test_login(sock, seq, account_id)
        seq += 1

        # 测试 SELECT_CHARACTER
        time.sleep(0.3)
        test_select_character(sock, seq, 0)
        seq += 1

        # 测试 START_GAME
        time.sleep(0.3)
        test_start_game(sock, seq)
        seq += 1

    finally:
        sock.close()
        print("\n=== Test completed ===")


if __name__ == '__main__':
    main()
