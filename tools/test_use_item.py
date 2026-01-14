#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_use_item.py - Item Use System Test Client

Tests item use protocol:
- msg_no=26: USE_ITEM (with HP/MP restoration effects)
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
    }

    payload = data[PACKET_HEADER_SIZE:header['length']] if header['length'] > PACKET_HEADER_SIZE else b''
    return header, payload

def test_login(sock, seq, account_id=1):
    """Login"""
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)

    sock.send(create_packet(1, bytes(data), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)
    return header and payload and struct.unpack('<I', payload[0:4])[0] == 0

def test_select_character(sock, seq, char_index=0):
    """Select character"""
    data = struct.pack('<I', char_index)
    sock.send(create_packet(4, data, seq))
    response = sock.recv(4096)
    header, payload = parse_response(response)
    return header and payload and struct.unpack('<I', payload[0:4])[0] == 0

def test_start_game(sock, seq):
    """Start game"""
    data = struct.pack('<II', 1, 0)
    sock.send(create_packet(15, data, seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)
    return header is not None

def use_item(sock, seq, slot, item_id):
    """Use an item (msg_no=26)"""
    data = struct.pack('<HI', slot, item_id)
    sock.send(create_packet(26, data, seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 19:
        result = struct.unpack('<I', payload[0:4])[0]
        resp_slot = struct.unpack('<H', payload[4:6])[0]
        resp_item = struct.unpack('<I', payload[6:10])[0]
        effect_type = payload[10]
        current_hp = struct.unpack('<I', payload[11:15])[0]
        current_mp = struct.unpack('<I', payload[15:19])[0]

        effect_names = {
            0: 'None',
            1: 'HP Restore',
            2: 'MP Restore',
            3: 'HP+MP Restore',
            4: 'Buff'
        }

        return {
            'result': result,
            'slot': resp_slot,
            'item_id': resp_item,
            'effect_type': effect_type,
            'effect_name': effect_names.get(effect_type, 'Unknown'),
            'current_hp': current_hp,
            'current_mp': current_mp
        }
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_use_item.py <host:port> [account_id]")
        print("Example: python test_use_item.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Item Use System Test Client")
    print(f"Target: {host}:{port}")
    print(f"Account ID: {account_id}")
    print("=" * 50)

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print(f"[OK] Connected")
    except Exception as e:
        print(f"[FAIL] Connection failed: {e}")
        sys.exit(1)

    seq = 0

    try:
        # Login flow
        print("\n=== Login Flow ===")
        if not test_login(sock, seq, account_id):
            print("[FAIL] Login failed")
            return
        seq += 1
        time.sleep(0.2)

        if not test_select_character(sock, seq, 0):
            print("[FAIL] Select character failed")
            return
        seq += 1
        time.sleep(0.2)

        test_start_game(sock, seq)
        seq += 1
        time.sleep(0.2)

        # Test HP Potion (item_id 10100 = 1000 HP restore)
        print("\n=== Use HP Potion (item_id=10100) ===")
        result = use_item(sock, seq, slot=0, item_id=10100)
        seq += 1
        if result:
            print(f"  [OK] Result: {result['result']}")
            print(f"    Effect: {result['effect_name']}")
            print(f"    Current HP: {result['current_hp']:,}")
            print(f"    Current MP: {result['current_mp']:,}")
        else:
            print("  [FAIL] No response")
        time.sleep(0.2)

        # Test MP Potion (item_id 20050 = 500 MP restore)
        print("\n=== Use MP Potion (item_id=20050) ===")
        result = use_item(sock, seq, slot=1, item_id=20050)
        seq += 1
        if result:
            print(f"  [OK] Result: {result['result']}")
            print(f"    Effect: {result['effect_name']}")
            print(f"    Current HP: {result['current_hp']:,}")
            print(f"    Current MP: {result['current_mp']:,}")
        else:
            print("  [FAIL] No response")
        time.sleep(0.2)

        # Test HP+MP Potion (item_id 30200 = 2000 HP+MP restore)
        print("\n=== Use HP+MP Potion (item_id=30200) ===")
        result = use_item(sock, seq, slot=2, item_id=30200)
        seq += 1
        if result:
            print(f"  [OK] Result: {result['result']}")
            print(f"    Effect: {result['effect_name']}")
            print(f"    Current HP: {result['current_hp']:,}")
            print(f"    Current MP: {result['current_mp']:,}")
        else:
            print("  [FAIL] No response")
        time.sleep(0.2)

        # Test Generic item
        print("\n=== Use Generic Item (item_id=99999) ===")
        result = use_item(sock, seq, slot=3, item_id=99999)
        seq += 1
        if result:
            print(f"  [OK] Result: {result['result']}")
            print(f"    Effect: {result['effect_name']}")
            print(f"    Current HP: {result['current_hp']:,}")
            print(f"    Current MP: {result['current_mp']:,}")
        else:
            print("  [FAIL] No response")

        print("\n" + "=" * 50)
        print("Item use test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
