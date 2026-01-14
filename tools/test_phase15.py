#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_phase15.py - Phase 15 Protocol Test Client

Tests Phase 15 systems:
- msg_no=534: SIGN_IN_INFO
- msg_no=535: SIGN_IN_DO
- msg_no=382: WEEKLY_QUEST_GET
- msg_no=390: EVENT_LIST
- msg_no=512: CASH_BALANCE
- msg_no=550: CROSS_MATCH_QUEUE
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    """Create packet with 13-byte header (DNF format)"""
    total_len = PACKET_HEADER_SIZE + len(data)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0  # classification
    struct.pack_into('<I', header, 1, total_len)  # length
    # msg_no can be > 255, use 2 bytes
    struct.pack_into('<H', header, 5, msg_no)  # msg_no (2 bytes)
    struct.pack_into('<I', header, 7, 0)  # checksum
    struct.pack_into('<H', header, 11, seq)  # sequence
    return bytes(header) + data

def parse_response(data):
    """Parse response packet"""
    if len(data) < PACKET_HEADER_SIZE:
        return None, None

    header = {
        'classification': data[0],
        'length': struct.unpack('<I', data[1:5])[0],
        'msg_no': struct.unpack('<H', data[5:7])[0],
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

    # Server sends multiple init packets after START_GAME
    # Read all responses to clear the buffer
    init_packets = 0
    try:
        while True:
            sock.settimeout(0.5)
            response = sock.recv(4096)
            if not response:
                break
            init_packets += 1
            header, payload = parse_response(response)
            if header:
                print(f"  Init packet {init_packets}: msg_no={header['msg_no']}, len={header['length']}")
    except socket.timeout:
        pass

    sock.settimeout(5)
    print(f"  Started game (received {init_packets} init packets)")
    return True

# ============ Phase 15 Tests ============

def test_sign_in_info(sock, seq):
    """Test SIGN_IN_INFO (msg_no=534)"""
    print("\n=== Testing SIGN_IN_INFO (msg_no=534) ===")

    sock.send(create_packet(534, b'', seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 8:
                result = struct.unpack('<I', payload[0:4])[0]
                streak = struct.unpack('<I', payload[4:8])[0]
                print(f"  Result={result}, Streak={streak}")
                return True
            elif payload:
                print(f"  Payload: {payload.hex()}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_sign_in_do(sock, seq):
    """Test SIGN_IN_DO (msg_no=535)"""
    print("\n=== Testing SIGN_IN_DO (msg_no=535) ===")

    sock.send(create_packet(535, b'', seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result={result}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_weekly_quest_get(sock, seq):
    """Test WEEKLY_QUEST_GET (msg_no=382)"""
    print("\n=== Testing WEEKLY_QUEST_GET (msg_no=382) ===")

    sock.send(create_packet(382, b'', seq))

    try:
        response = sock.recv(2048)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result={result}")
                if len(payload) >= 8:
                    quest_count = struct.unpack('<I', payload[4:8])[0]
                    print(f"  Quest count={quest_count}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_event_list(sock, seq):
    """Test EVENT_LIST (msg_no=390)"""
    print("\n=== Testing EVENT_LIST (msg_no=390) ===")

    sock.send(create_packet(390, b'', seq))

    try:
        response = sock.recv(2048)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result={result}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_cash_balance(sock, seq):
    """Test CASH_BALANCE (msg_no=512)"""
    print("\n=== Testing CASH_BALANCE (msg_no=512) ===")

    sock.send(create_packet(512, b'', seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 12:
                result = struct.unpack('<I', payload[0:4])[0]
                cash = struct.unpack('<I', payload[4:8])[0]
                bind_cash = struct.unpack('<I', payload[8:12])[0]
                print(f"  Result={result}, Cash={cash}, BindCash={bind_cash}")
                return True
            elif payload:
                print(f"  Payload({len(payload)}): {payload.hex()}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_cross_match_queue(sock, seq, mode=1):
    """Test CROSS_MATCH_QUEUE (msg_no=550)"""
    print(f"\n=== Testing CROSS_MATCH_QUEUE (msg_no=550, mode={mode}) ===")

    data = struct.pack('<B', mode)
    sock.send(create_packet(550, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result={result}")
                return True
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_phase15.py <host:port> [account_id]")
        print("Example: python test_phase15.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print("DNF Phase 15 Protocol Test Client")
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

        print("\n" + "=" * 50)
        print("Phase 15 Protocol Tests")
        print("=" * 50)

        # Test Phase 15 protocols
        tests = [
            (test_sign_in_info, [sock, seq]),
            (test_sign_in_do, [sock, seq + 1]),
            (test_weekly_quest_get, [sock, seq + 2]),
            (test_event_list, [sock, seq + 3]),
            (test_cash_balance, [sock, seq + 4]),
            (test_cross_match_queue, [sock, seq + 5, 1]),
        ]

        for test_func, args in tests:
            time.sleep(0.3)
            total += 1
            if test_func(*args):
                passed += 1

    finally:
        sock.close()
        print("\n" + "=" * 50)
        print(f"Phase 15 Test Results: {passed}/{total} passed")
        print("=" * 50)

if __name__ == '__main__':
    main()
