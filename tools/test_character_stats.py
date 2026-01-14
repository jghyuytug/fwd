#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_character_stats.py - Character Stats Test Client

Tests character stats protocol:
- msg_no=38: GET_CHARACTER_STATS (returns 16 combat stats)
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

def get_character_stats(sock, seq):
    """Get character combat stats (msg_no=38)"""
    sock.send(create_packet(38, b'', seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload and len(payload) >= 68:
        result = struct.unpack('<I', payload[0:4])[0]
        if result != 0:
            return None

        stats = {}
        offset = 4
        stat_names = [
            'STR', 'INT', 'VIT', 'SPR',
            'PhysAtk', 'MagAtk', 'PhysDef', 'MagDef',
            'MaxHP', 'MaxMP', 'AtkSpd', 'MoveSpd',
            'CastSpd', 'CritRate', 'HitRate', 'Evasion'
        ]

        for name in stat_names:
            value = struct.unpack('<I', payload[offset:offset+4])[0]
            stats[name] = value
            offset += 4

        return stats
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_character_stats.py <host:port> [account_id]")
        print("Example: python test_character_stats.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Character Stats Test Client")
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

        # Get character stats
        print("\n=== Character Combat Stats (msg_no=38) ===")
        stats = get_character_stats(sock, seq)
        seq += 1

        if stats:
            print("[OK] Stats received!")
            print("\n  Base Stats:")
            print(f"    STR: {stats['STR']:,}")
            print(f"    INT: {stats['INT']:,}")
            print(f"    VIT: {stats['VIT']:,}")
            print(f"    SPR: {stats['SPR']:,}")

            print("\n  Attack Stats:")
            print(f"    Physical Attack: {stats['PhysAtk']:,}")
            print(f"    Magic Attack: {stats['MagAtk']:,}")

            print("\n  Defense Stats:")
            print(f"    Physical Defense: {stats['PhysDef']:,}")
            print(f"    Magic Defense: {stats['MagDef']:,}")

            print("\n  Vitals:")
            print(f"    Max HP: {stats['MaxHP']:,}")
            print(f"    Max MP: {stats['MaxMP']:,}")

            print("\n  Speed Stats:")
            print(f"    Attack Speed: {stats['AtkSpd']:,}")
            print(f"    Move Speed: {stats['MoveSpd']:,}")
            print(f"    Cast Speed: {stats['CastSpd']:,}")

            print("\n  Combat Stats:")
            print(f"    Critical Rate: {stats['CritRate']/100:.2f}%")
            print(f"    Hit Rate: {stats['HitRate']:,}")
            print(f"    Evasion: {stats['Evasion']:,}")
        else:
            print("[FAIL] Could not get stats")

        print("\n" + "=" * 50)
        print("Character stats test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
