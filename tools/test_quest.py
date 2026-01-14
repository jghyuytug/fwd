#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_quest.py - Quest System Test Client

Tests quest protocols:
- msg_no=50: GET_QUEST_LIST
- msg_no=51: ACCEPT_QUEST
- msg_no=52: COMPLETE_QUEST (with EXP/Gold rewards)
- msg_no=53: UPDATE_QUEST_PROGRESS
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    total_len = PACKET_HEADER_SIZE + len(data)
    header = bytearray(PACKET_HEADER_SIZE)
    header[0] = 0
    struct.pack_into('<I', header, 1, total_len)
    header[5] = msg_no
    struct.pack_into('<H', header, 11, seq)
    return bytes(header) + data

def parse_response(data):
    if len(data) < PACKET_HEADER_SIZE:
        return None, None
    header = {
        'length': struct.unpack('<I', data[1:5])[0],
        'msg_no': data[5],
    }
    payload = data[PACKET_HEADER_SIZE:header['length']] if header['length'] > PACKET_HEADER_SIZE else b''
    return header, payload

def login_flow(sock, account_id):
    """Login -> Select Character -> Start Game"""
    seq = 0

    # LOGIN
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)
    sock.send(create_packet(1, bytes(data), seq))
    sock.recv(2048)
    seq += 1
    time.sleep(0.1)

    # SELECT CHARACTER
    sock.send(create_packet(4, struct.pack('<I', 0), seq))
    sock.recv(4096)
    seq += 1
    time.sleep(0.1)

    # START GAME
    sock.send(create_packet(15, struct.pack('<II', 1, 0), seq))
    sock.recv(2048)
    seq += 1
    time.sleep(0.1)

    return seq

def get_quest_list(sock, seq):
    """Get quest list (msg_no=50)"""
    sock.send(create_packet(50, b'', seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 5:
        result = struct.unpack('<I', payload[0:4])[0]
        quest_count = payload[4]
        quests = []
        offset = 5
        for i in range(quest_count):
            if offset + 5 > len(payload):
                break
            quest_id = struct.unpack('<I', payload[offset:offset+4])[0]
            progress = payload[offset+4]
            quests.append((quest_id, progress))
            offset += 5
        return result, quests
    return None, []

def accept_quest(sock, seq, quest_id):
    """Accept quest (msg_no=51)"""
    sock.send(create_packet(51, struct.pack('<I', quest_id), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 4:
        result = struct.unpack('<I', payload[0:4])[0]
        return result
    return None

def complete_quest(sock, seq, quest_id):
    """Complete quest (msg_no=52)"""
    sock.send(create_packet(52, struct.pack('<I', quest_id), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 16:
        result = struct.unpack('<I', payload[0:4])[0]
        if result == 0:
            q_id = struct.unpack('<I', payload[4:8])[0]
            exp = struct.unpack('<I', payload[8:12])[0]
            gold = struct.unpack('<I', payload[12:16])[0]
            return result, exp, gold
        return result, 0, 0
    return None, 0, 0

def update_quest_progress(sock, seq, quest_id, progress):
    """Update quest progress (msg_no=53)"""
    data = struct.pack('<I', quest_id) + bytes([progress])
    sock.send(create_packet(53, data, seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 4:
        result = struct.unpack('<I', payload[0:4])[0]
        return result
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_quest.py <host:port> [account_id]")
        sys.exit(1)

    addr = sys.argv[1]
    host, port = addr.split(':') if ':' in addr else (addr, 20011)
    port = int(port)
    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Quest System Test Client")
    print(f"Target: {host}:{port}")
    print("=" * 50)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect((host, port))
    print("[OK] Connected")

    try:
        seq = login_flow(sock, account_id)
        time.sleep(0.2)

        # Get initial quest list
        print("\n=== Get Quest List (empty) ===")
        result, quests = get_quest_list(sock, seq)
        seq += 1
        print(f"  Result: {result}, Active Quests: {len(quests)}")
        time.sleep(0.1)

        # Accept quest 1001
        print("\n=== Accept Quest 1001 ===")
        result = accept_quest(sock, seq, 1001)
        seq += 1
        print(f"  Result: {result} ({'Success' if result == 0 else 'Failed'})")
        time.sleep(0.1)

        # Accept quest 1002
        print("\n=== Accept Quest 1002 ===")
        result = accept_quest(sock, seq, 1002)
        seq += 1
        print(f"  Result: {result} ({'Success' if result == 0 else 'Failed'})")
        time.sleep(0.1)

        # Get quest list again
        print("\n=== Get Quest List (after accept) ===")
        result, quests = get_quest_list(sock, seq)
        seq += 1
        print(f"  Active Quests: {len(quests)}")
        for qid, prog in quests:
            print(f"    Quest {qid}: {prog}%")
        time.sleep(0.1)

        # Update progress on quest 1001
        print("\n=== Update Quest 1001 Progress (50%) ===")
        result = update_quest_progress(sock, seq, 1001, 50)
        seq += 1
        print(f"  Result: {result}")
        time.sleep(0.1)

        # Update progress to 100%
        print("\n=== Update Quest 1001 Progress (100%) ===")
        result = update_quest_progress(sock, seq, 1001, 100)
        seq += 1
        print(f"  Result: {result}")
        time.sleep(0.1)

        # Verify progress
        print("\n=== Verify Progress ===")
        result, quests = get_quest_list(sock, seq)
        seq += 1
        for qid, prog in quests:
            print(f"    Quest {qid}: {prog}%")
        time.sleep(0.1)

        # Complete quest 1001
        print("\n=== Complete Quest 1001 ===")
        result, exp, gold = complete_quest(sock, seq, 1001)
        seq += 1
        if result == 0:
            print(f"  [OK] Quest completed!")
            print(f"    EXP Reward: {exp:,}")
            print(f"    Gold Reward: {gold:,}")
        else:
            print(f"  [FAIL] Result: {result}")
        time.sleep(0.1)

        # Final quest list
        print("\n=== Final Quest List ===")
        result, quests = get_quest_list(sock, seq)
        seq += 1
        print(f"  Active Quests: {len(quests)}")
        for qid, prog in quests:
            print(f"    Quest {qid}: {prog}%")

        print("\n" + "=" * 50)
        print("Quest system test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
