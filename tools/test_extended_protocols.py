#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_extended_protocols.py - Extended Game Protocol Test Client

Tests additional protocols:
- msg_no=9: HEARTBEAT
- msg_no=10: CHAT
- msg_no=20: MOVE
- msg_no=25: GET_INVENTORY
- msg_no=30: USE_SKILL
- msg_no=31: GET_SKILL_LIST
- msg_no=35: NPC_DIALOG
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    """Create packet with 13-byte header"""
    total_len = PACKET_HEADER_SIZE + len(data)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0
    struct.pack_into('<I', header, 1, total_len)
    header[5] = msg_no
    header[6] = 0
    struct.pack_into('<I', header, 7, 0)
    struct.pack_into('<H', header, 11, seq)
    return bytes(header) + data

def parse_response(data):
    """Parse response packet"""
    if len(data) < PACKET_HEADER_SIZE:
        return None, None

    header = {
        'classification': data[0],
        'length': struct.unpack('<I', data[1:5])[0],
        'msg_no': data[5],
        'reserved': data[6],
        'checksum': struct.unpack('<I', data[7:11])[0],
        'sequence': struct.unpack('<H', data[11:13])[0]
    }

    payload = data[PACKET_HEADER_SIZE:header['length']] if header['length'] > PACKET_HEADER_SIZE else b''
    return header, payload

def test_login(sock, seq, account_id=1):
    """Login to get in STATE_IN_GAME"""
    print("\n=== LOGIN (msg_no=1) ===")

    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)

    sock.send(create_packet(1, bytes(data), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 8:
        result = struct.unpack('<I', payload[0:4])[0]
        char_count = struct.unpack('<I', payload[4:8])[0]
        print(f"  Result={result}, Characters={char_count}")
        return char_count > 0
    return False

def test_select_character(sock, seq, char_index=0):
    """Select character to enter game"""
    print("\n=== SELECT_CHARACTER (msg_no=4) ===")

    data = struct.pack('<I', char_index)
    sock.send(create_packet(4, data, seq))
    response = sock.recv(4096)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 4:
        result = struct.unpack('<I', payload[0:4])[0]
        print(f"  Result={result}")
        return result == 0
    return False

def test_start_game(sock, seq):
    """Start game to be in STATE_IN_GAME"""
    print("\n=== START_GAME (msg_no=15) ===")

    data = struct.pack('<II', 1, 0)
    sock.send(create_packet(15, data, seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header:
        print(f"  Started game (payload len={len(payload)})")
        return True
    return False

def test_heartbeat(sock, seq):
    """Test HEARTBEAT (msg_no=9)"""
    print("\n=== Testing HEARTBEAT (msg_no=9) ===")

    sock.send(create_packet(9, b'', seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 8:
                result = struct.unpack('<I', payload[0:4])[0]
                server_time = struct.unpack('<I', payload[4:8])[0]
                print(f"  Result={result}, Server Time={server_time}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_chat(sock, seq, message="Hello World"):
    """Test CHAT (msg_no=10)"""
    print(f"\n=== Testing CHAT (msg_no=10): '{message}' ===")

    msg_bytes = message.encode('utf-8')
    data = bytearray(2 + len(msg_bytes))
    data[0] = 0  # chat_type (0=normal)
    data[1] = len(msg_bytes)
    data[2:] = msg_bytes

    sock.send(create_packet(10, bytes(data), seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload:
                result = payload[0]
                chat_type = payload[1]
                sender_raw = payload[2:22].split(b'\x00')[0]
                try:
                    sender_name = sender_raw.decode('utf-8')
                except:
                    sender_name = sender_raw.hex()
                print(f"  Result={result}, Type={chat_type}, Sender={sender_name}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_move(sock, seq, x=100, y=200):
    """Test MOVE (msg_no=20)"""
    print(f"\n=== Testing MOVE (msg_no=20): ({x}, {y}) ===")

    data = struct.pack('<HHbb', x, y, 0, 0)  # x, y, direction, move_type
    sock.send(create_packet(20, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 4:
                rx, ry = struct.unpack('<HH', payload[0:4])
                print(f"  Echoed position: ({rx}, {ry})")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_get_inventory(sock, seq):
    """Test GET_INVENTORY (msg_no=25)"""
    print("\n=== Testing GET_INVENTORY (msg_no=25) ===")

    sock.send(create_packet(25, b'', seq))

    try:
        response = sock.recv(2048)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 12:
                result = struct.unpack('<I', payload[0:4])[0]
                gold = struct.unpack('<I', payload[4:8])[0]
                capacity = struct.unpack('<H', payload[8:10])[0]
                item_count = struct.unpack('<H', payload[10:12])[0]
                print(f"  Result={result}, Gold={gold}, Capacity={capacity}, Items={item_count}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_get_skill_list(sock, seq):
    """Test GET_SKILL_LIST (msg_no=31)"""
    print("\n=== Testing GET_SKILL_LIST (msg_no=31) ===")

    sock.send(create_packet(31, b'', seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 10:
                result = struct.unpack('<I', payload[0:4])[0]
                sp = struct.unpack('<I', payload[4:8])[0]
                skill_count = struct.unpack('<H', payload[8:10])[0]
                print(f"  Result={result}, SP={sp}, Skills={skill_count}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_npc_dialog(sock, seq, npc_id=1001):
    """Test NPC_DIALOG (msg_no=35)"""
    print(f"\n=== Testing NPC_DIALOG (msg_no=35): npc_id={npc_id} ===")

    data = struct.pack('<I', npc_id)
    sock.send(create_packet(35, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 6:
                result = struct.unpack('<I', payload[0:4])[0]
                npc_type = payload[4]
                option_count = payload[5]
                print(f"  Result={result}, NPC Type={npc_type}, Options={option_count}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_use_skill(sock, seq, skill_id=1, x=150, y=250):
    """Test USE_SKILL (msg_no=30)"""
    print(f"\n=== Testing USE_SKILL (msg_no=30): skill={skill_id} at ({x},{y}) ===")

    data = struct.pack('<HHH', skill_id, x, y)
    sock.send(create_packet(30, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 6:
                result = struct.unpack('<I', payload[0:4])[0]
                cooldown = struct.unpack('<H', payload[4:6])[0]
                print(f"  Result={result}, Cooldown={cooldown}ms")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_extended_protocols.py <host:port> [account_id]")
        print("Example: python test_extended_protocols.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Extended Protocol Test Client")
    print(f"Target: {host}:{port}")
    print(f"Account ID: {account_id}")
    print("=" * 50)

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print(f"[OK] Connected to {host}:{port}")
    except Exception as e:
        print(f"[FAIL] Connection failed: {e}")
        sys.exit(1)

    seq = 0
    passed = 0
    total = 0

    try:
        # Login flow
        time.sleep(0.2)
        if test_login(sock, seq, account_id):
            seq += 1
            time.sleep(0.2)
            if test_select_character(sock, seq, 0):
                seq += 1
                time.sleep(0.2)
                test_start_game(sock, seq)
                seq += 1

        # Test extended protocols
        tests = [
            (test_heartbeat, [sock, seq]),
            (test_chat, [sock, seq + 1, "Hello DNF"]),
            (test_move, [sock, seq + 2, 100, 200]),
            (test_get_inventory, [sock, seq + 3]),
            (test_get_skill_list, [sock, seq + 4]),
            (test_use_skill, [sock, seq + 5, 1, 150, 250]),
            (test_npc_dialog, [sock, seq + 6, 1001]),
        ]

        for test_func, args in tests:
            time.sleep(0.3)
            total += 1
            if test_func(*args):
                passed += 1

    finally:
        sock.close()
        print("\n" + "=" * 50)
        print(f"Test Results: {passed}/{total} passed")
        print("=" * 50)

if __name__ == '__main__':
    main()
