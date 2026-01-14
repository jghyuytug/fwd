#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Test new features: Party, Auction, Friend/Guild Online, Storage"""

import socket
import struct
import sys

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
        'msg_no': data[5],
        'length': struct.unpack('<I', data[1:5])[0]
    }
    payload = data[PACKET_HEADER_SIZE:header['length']] if header['length'] > PACKET_HEADER_SIZE else b''
    return header, payload

def login_and_start(sock, account_id):
    """Login, select character, start game"""
    seq = 0

    # LOGIN (msg_no=1)
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)
    sock.send(create_packet(1, bytes(data), seq))
    seq += 1

    response = sock.recv(2048)
    header, payload = parse_response(response)
    if not payload or payload[0] != 0:
        return False, 0

    # SELECT_CHARACTER (msg_no=4)
    data = struct.pack('<I', 0)  # char_index=0
    sock.send(create_packet(4, data, seq))
    seq += 1

    response = sock.recv(4096)
    header, payload = parse_response(response)
    if not payload or payload[0] != 0:
        return False, 0

    # START_GAME (msg_no=15)
    data = struct.pack('<II', 0, 0)
    sock.send(create_packet(15, data, seq))
    seq += 1

    response = sock.recv(2048)
    header, payload = parse_response(response)

    return True, seq

def test_party(host, port, account_id):
    """Test party system"""
    print("\n=== PARTY SYSTEM TEST ===")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(5)

    success, seq = login_and_start(sock, account_id)
    if not success:
        print("  [FAIL] Login failed")
        sock.close()
        return False

    passed = True

    # Create party (msg_no=120)
    sock.send(create_packet(120, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 120 and payload and payload[0] == 0:
        party_id = struct.unpack('<I', payload[4:8])[0]
        print(f"  [OK] Party created: party_id={party_id}")
    else:
        print("  [FAIL] Party create failed")
        passed = False

    # Get party info (msg_no=125)
    sock.send(create_packet(125, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 125 and payload and payload[0] == 0:
        ret_party_id = struct.unpack('<I', payload[4:8])[0]
        member_count = payload[8] if len(payload) > 8 else 0
        print(f"  [OK] Party info: party_id={ret_party_id}, members={member_count}")
    else:
        print("  [FAIL] Party info failed")
        passed = False

    # Leave party (msg_no=123)
    sock.send(create_packet(123, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 123 and payload and payload[0] == 0:
        print("  [OK] Left party")
    else:
        print("  [FAIL] Leave party failed")
        passed = False

    sock.close()
    return passed

def test_auction(host, port, account_id):
    """Test auction system"""
    print("\n=== AUCTION SYSTEM TEST ===")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(5)

    success, seq = login_and_start(sock, account_id)
    if not success:
        print("  [FAIL] Login failed")
        sock.close()
        return False

    passed = True

    # Search auction (msg_no=170)
    search_data = struct.pack('<III', 0, 0, 0)  # item_id=0, min=0, max=0
    sock.send(create_packet(170, search_data, seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 170 and payload:
        result = struct.unpack('<I', payload[:4])[0]
        count = payload[4] if len(payload) > 4 else 0
        print(f"  [OK] Auction search: result={result}, listings={count}")
    else:
        print("  [FAIL] Auction search failed")
        passed = False

    # Get my listings (msg_no=174)
    sock.send(create_packet(174, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 174 and payload:
        result = struct.unpack('<I', payload[:4])[0]
        count = payload[4] if len(payload) > 4 else 0
        print(f"  [OK] My listings: result={result}, count={count}")
    else:
        print("  [FAIL] Get listings failed")
        passed = False

    sock.close()
    return passed

def test_friend(host, port, account_id):
    """Test friend list with online status"""
    print("\n=== FRIEND ONLINE STATUS TEST ===")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(5)

    success, seq = login_and_start(sock, account_id)
    if not success:
        print("  [FAIL] Login failed")
        sock.close()
        return False

    # Get friend list (msg_no=130)
    sock.send(create_packet(130, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 130 and payload:
        result = struct.unpack('<I', payload[:4])[0]
        count = payload[4] if len(payload) > 4 else 0
        print(f"  [OK] Friend list: result={result}, friends={count}")
        if count > 0 and len(payload) >= 33:  # 5 + 28 (friend entry)
            offset = 5
            friend_id = struct.unpack('<I', payload[offset:offset+4])[0]
            name_bytes = payload[offset+4:offset+24]
            name = name_bytes.split(b'\x00')[0].decode('utf-8', errors='ignore')
            online = payload[offset+24] if len(payload) > offset+24 else 0
            print(f"  [OK] First friend: id={friend_id}, name='{name}', online={online}")
    else:
        print("  [FAIL] Friend list failed")
        sock.close()
        return False

    sock.close()
    return True

def test_guild(host, port, account_id):
    """Test guild with online status"""
    print("\n=== GUILD MEMBER ONLINE TEST ===")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(5)

    success, seq = login_and_start(sock, account_id)
    if not success:
        print("  [FAIL] Login failed")
        sock.close()
        return False

    # Get guild info (msg_no=140)
    sock.send(create_packet(140, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 140 and payload:
        result = struct.unpack('<I', payload[:4])[0]
        if result == 0 and len(payload) >= 8:
            guild_id = struct.unpack('<I', payload[4:8])[0]
            print(f"  [OK] Guild info: guild_id={guild_id}")
        else:
            print(f"  [OK] No guild (result={result})")
    else:
        print("  [FAIL] Guild info failed")
        sock.close()
        return False

    sock.close()
    return True

def test_storage(host, port, account_id):
    """Test storage system"""
    print("\n=== STORAGE SYSTEM TEST ===")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(5)

    success, seq = login_and_start(sock, account_id)
    if not success:
        print("  [FAIL] Login failed")
        sock.close()
        return False

    # Get storage (msg_no=100)
    sock.send(create_packet(100, b'', seq))
    seq += 1
    response = sock.recv(2048)
    header, payload = parse_response(response)
    if header and header['msg_no'] == 100 and payload:
        result = struct.unpack('<I', payload[:4])[0]
        if result == 0 and len(payload) >= 12:
            gold = struct.unpack('<I', payload[4:8])[0]
            capacity = struct.unpack('<I', payload[8:12])[0]
            items = payload[12] if len(payload) > 12 else 0
            print(f"  [OK] Storage: gold={gold}, capacity={capacity}, items={items}")
        else:
            print(f"  [OK] Storage result={result}")
    else:
        print("  [FAIL] Storage get failed")
        sock.close()
        return False

    sock.close()
    return True

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} host:port account_id")
        sys.exit(1)

    addr = sys.argv[1].split(':')
    host = addr[0]
    port = int(addr[1])
    account_id = int(sys.argv[2])

    print("=" * 60)
    print("DNF New Features Test")
    print(f"Target: {host}:{port}")
    print("=" * 60)

    results = []
    results.append(("Party System", test_party(host, port, account_id)))
    results.append(("Auction System", test_auction(host, port, account_id)))
    results.append(("Friend Online", test_friend(host, port, account_id)))
    results.append(("Guild Online", test_guild(host, port, account_id)))
    results.append(("Storage System", test_storage(host, port, account_id)))

    print("\n" + "=" * 60)
    print("Test Results:")
    print("=" * 60)
    passed = 0
    for name, result in results:
        status = "[PASS]" if result else "[FAIL]"
        print(f"  {status} {name}")
        if result:
            passed += 1

    print(f"\nTotal: {passed}/{len(results)} passed")
    print("=" * 60)

if __name__ == '__main__':
    main()
