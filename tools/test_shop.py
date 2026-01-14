#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_shop.py - Shop System Test Client

Tests shop protocols with real DB operations:
- msg_no=36: NPC_SHOP_BUY (with gold deduction)
- msg_no=25: GET_INVENTORY (to verify gold change)
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

def get_inventory(sock, seq):
    """Get current inventory/gold"""
    sock.send(create_packet(25, b'', seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and payload and len(payload) >= 12:
        result = struct.unpack('<I', payload[0:4])[0]
        gold = struct.unpack('<I', payload[4:8])[0]
        return gold if result == 0 else None
    return None

def shop_buy(sock, seq, npc_id=1001, item_id=50001, count=1):
    """Buy item from NPC shop"""
    data = struct.pack('<IIH', npc_id, item_id, count)
    sock.send(create_packet(36, data, seq))
    response = sock.recv(1024)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 4:
        result = struct.unpack('<I', payload[0:4])[0]
        if result == 0 and len(payload) >= 14:
            new_gold = struct.unpack('<I', payload[4:8])[0]
            slot = struct.unpack('<H', payload[8:10])[0]
            bought_item = struct.unpack('<I', payload[10:14])[0]
            return {'result': result, 'new_gold': new_gold, 'slot': slot, 'item_id': bought_item}
        else:
            return {'result': result, 'error': True}
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_shop.py <host:port> [account_id]")
        print("Example: python test_shop.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Shop System Test Client")
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

        # Get initial gold
        print("\n=== Check Initial Gold ===")
        initial_gold = get_inventory(sock, seq)
        seq += 1
        print(f"  Initial Gold: {initial_gold:,}")
        time.sleep(0.2)

        # Test buying an item
        print("\n=== Buy Item (item_id=50001, count=1) ===")
        # Price = (50001 % 10000) * 10 = 10 gold
        result = shop_buy(sock, seq, npc_id=1001, item_id=50001, count=1)
        seq += 1

        if result:
            if result.get('error'):
                print(f"  [FAIL] Purchase failed, result={result['result']}")
            else:
                print(f"  [OK] Purchase successful!")
                print(f"    New Gold: {result['new_gold']:,}")
                print(f"    Item placed in slot: {result['slot']}")
                print(f"    Item ID: {result['item_id']}")
                gold_spent = initial_gold - result['new_gold'] if initial_gold else 0
                print(f"    Gold spent: {gold_spent:,}")
        else:
            print("  [FAIL] No response")
        time.sleep(0.2)

        # Verify gold change
        print("\n=== Verify Gold After Purchase ===")
        final_gold = get_inventory(sock, seq)
        seq += 1
        print(f"  Final Gold: {final_gold:,}")

        if initial_gold and final_gold:
            diff = initial_gold - final_gold
            print(f"  Gold Difference: {diff:,}")

        # Buy another item with higher price
        print("\n=== Buy Higher Price Item (item_id=51234, count=2) ===")
        # Price = (51234 % 10000) * 10 * 2 = 24680 gold
        result2 = shop_buy(sock, seq, npc_id=1001, item_id=51234, count=2)
        seq += 1

        if result2:
            if result2.get('error'):
                print(f"  [FAIL] Purchase failed, result={result2['result']}")
            else:
                print(f"  [OK] Purchase successful!")
                print(f"    New Gold: {result2['new_gold']:,}")
                print(f"    Slot: {result2['slot']}")
                gold_spent = final_gold - result2['new_gold'] if final_gold else 0
                print(f"    Gold spent: {gold_spent:,}")
        else:
            print("  [FAIL] No response")

        print("\n" + "=" * 50)
        print("Shop test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
