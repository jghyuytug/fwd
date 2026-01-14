#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_equipment.py - Equipment System Test Client

Tests equipment protocols:
- msg_no=32: GET_EQUIPMENT
- msg_no=33: EQUIP_ITEM
- msg_no=34: UNEQUIP_ITEM
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

def test_get_equipment(sock, seq):
    """Test GET_EQUIPMENT (msg_no=32)"""
    print("\n=== Testing GET_EQUIPMENT (msg_no=32) ===")

    sock.send(create_packet(32, b'', seq))

    try:
        response = sock.recv(2048)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 5:
                result = struct.unpack('<I', payload[0:4])[0]
                equip_count = payload[4]
                print(f"  Result={result}, Equipment Count={equip_count}")

                # Parse equipment items
                offset = 5
                items = []
                for i in range(equip_count):
                    if offset + 8 > len(payload):
                        break
                    slot = payload[offset]
                    item_id = struct.unpack('<I', payload[offset+1:offset+5])[0]
                    stat = payload[offset+5]
                    hidden = struct.unpack('<H', payload[offset+6:offset+8])[0]
                    items.append((slot, item_id, stat, hidden))
                    print(f"    Slot {slot}: item_id={item_id}, stat={stat}, hidden={hidden}")
                    offset += 8

                return items
    except socket.timeout:
        print("  [TIMEOUT]")
    return None

def test_equip_item(sock, seq, slot=0, item_id=50601):
    """Test EQUIP_ITEM (msg_no=33)"""
    print(f"\n=== Testing EQUIP_ITEM (msg_no=33): slot={slot}, item_id={item_id} ===")

    data = struct.pack('<BI', slot, item_id)
    sock.send(create_packet(33, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 9:
                result = struct.unpack('<I', payload[0:4])[0]
                resp_slot = payload[4]
                resp_item = struct.unpack('<I', payload[5:9])[0]
                print(f"  Result={result}, Slot={resp_slot}, Item={resp_item}")
                return result == 0
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def test_unequip_item(sock, seq, slot=0):
    """Test UNEQUIP_ITEM (msg_no=34)"""
    print(f"\n=== Testing UNEQUIP_ITEM (msg_no=34): slot={slot} ===")

    data = struct.pack('<B', slot)
    sock.send(create_packet(34, data, seq))

    try:
        response = sock.recv(1024)
        header, payload = parse_response(response)
        if header:
            print(f"  Response: msg_no={header['msg_no']}, len={header['length']}")
            if payload and len(payload) >= 9:
                result = struct.unpack('<I', payload[0:4])[0]
                resp_slot = payload[4]
                resp_item = struct.unpack('<I', payload[5:9])[0]
                print(f"  Result={result}, Slot={resp_slot}, Unequipped Item={resp_item}")
                return result == 0
    except socket.timeout:
        print("  [TIMEOUT]")
    return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_equipment.py <host:port> [account_id]")
        print("Example: python test_equipment.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Equipment System Test Client")
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

        # Test equipment protocols
        time.sleep(0.3)
        total += 1
        items = test_get_equipment(sock, seq)
        if items is not None:
            passed += 1
            print(f"\n  [OK] Found {len(items)} equipped items from DB!")
        seq += 1

        # Test equip item
        time.sleep(0.3)
        total += 1
        if test_equip_item(sock, seq, slot=0, item_id=99999):
            passed += 1
        seq += 1

        # Test unequip item
        time.sleep(0.3)
        total += 1
        if test_unequip_item(sock, seq, slot=0):
            passed += 1
        seq += 1

        # Get equipment again to verify changes
        time.sleep(0.3)
        total += 1
        items2 = test_get_equipment(sock, seq)
        if items2 is not None:
            passed += 1

    finally:
        sock.close()
        print("\n" + "=" * 50)
        print(f"Test Results: {passed}/{total} passed")
        print("=" * 50)

if __name__ == '__main__':
    main()
