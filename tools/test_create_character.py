#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_create_character.py - Character Creation Test Client

Tests character creation protocol:
- msg_no=5: CREATE_CHARACTER (with job selection and initial stats)
"""

import socket
import struct
import sys
import time
import random

PACKET_HEADER_SIZE = 13

# Job types
JOB_SLAYER = 0
JOB_FIGHTER = 1
JOB_GUNNER = 2
JOB_MAGE = 3
JOB_PRIEST = 4
JOB_THIEF = 5

JOB_NAMES = {
    0: "Slayer",
    1: "Fighter",
    2: "Gunner",
    3: "Mage",
    4: "Priest",
    5: "Thief"
}

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

def login(sock, seq, account_id):
    """Login only (no character select)"""
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)
    sock.send(create_packet(1, bytes(data), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and payload and len(payload) >= 4:
        return struct.unpack('<I', payload[0:4])[0] == 0
    return False

def create_character(sock, seq, name, job, grow_type=0, sex=0):
    """Create a new character (msg_no=5)"""
    data = bytearray(24)

    # Name (16 bytes)
    name_bytes = name.encode('utf-8')[:15]
    data[0:len(name_bytes)] = name_bytes

    # Job, grow_type, sex
    data[16] = job
    data[17] = grow_type
    data[18] = sex

    sock.send(create_packet(5, bytes(data), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 8:
        result = struct.unpack('<I', payload[0:4])[0]
        charac_no = struct.unpack('<I', payload[4:8])[0]
        return result, charac_no
    return None, None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_create_character.py <host:port> [account_id]")
        sys.exit(1)

    addr = sys.argv[1]
    host, port = addr.split(':') if ':' in addr else (addr, 20011)
    port = int(port)
    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Character Creation Test Client")
    print(f"Target: {host}:{port}")
    print("=" * 50)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect((host, port))
    print("[OK] Connected")

    seq = 0

    try:
        # Login first
        print("\n=== Login ===")
        if not login(sock, seq, account_id):
            print("[FAIL] Login failed")
            return
        seq += 1
        print("[OK] Login successful")
        time.sleep(0.2)

        # Generate unique test name
        test_name = f"Test{random.randint(1000, 9999)}"

        # Test creating character with each job
        for job in range(6):
            print(f"\n=== Create {JOB_NAMES[job]} Character ===")
            char_name = f"{test_name}_{JOB_NAMES[job][:3]}"

            result, charac_no = create_character(sock, seq, char_name, job, sex=job % 2)
            seq += 1

            if result == 0:
                print(f"  [OK] Created: {char_name}")
                print(f"    Job: {JOB_NAMES[job]}")
                print(f"    Character No: {charac_no}")
            elif result == 3:
                print(f"  [INFO] Name already exists: {char_name}")
            else:
                print(f"  [FAIL] Result: {result}")

            time.sleep(0.2)

        # Try duplicate name
        print(f"\n=== Test Duplicate Name ===")
        result, charac_no = create_character(sock, seq, f"{test_name}_Sla", JOB_SLAYER)
        seq += 1
        if result == 3:
            print(f"  [OK] Correctly rejected duplicate name")
        elif result == 0:
            print(f"  [INFO] Name was unique, created: {charac_no}")
        else:
            print(f"  Result: {result}")

        print("\n" + "=" * 50)
        print("Character creation test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
