#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Test 128 Protocol Game Server"""
import socket
import struct
import sys

def build_packet(msg_no, data=b'', seq=1):
    length = 13 + len(data)
    # Format: class(1) + length(4) + msg_no(1) + reserved(5) + seq(2) = 13 bytes
    header = struct.pack('<BI', 0, length)  # class + length = 5 bytes
    header += struct.pack('<B', msg_no)      # msg_no = 1 byte
    header += b'\x00' * 5                    # reserved = 5 bytes
    header += struct.pack('<H', seq)         # seq = 2 bytes
    return header + data

g_seq = 0

def send_recv(sock, msg_no, data=b'', timeout=3):
    global g_seq
    g_seq += 1
    sock.settimeout(timeout)
    pkt = build_packet(msg_no, data, g_seq)
    sock.send(pkt)
    try:
        resp = sock.recv(4096)
        if len(resp) >= 13:
            r_msg = resp[5]
            r_data = resp[13:] if len(resp) > 13 else b''
            return r_msg, r_data
    except socket.timeout:
        return -1, b''
    except Exception as e:
        return -2, str(e).encode()
    return None, None

def main():
    host = '192.168.200.131'
    port = 20011
    if len(sys.argv) > 1:
        parts = sys.argv[1].split(':')
        host = parts[0]
        if len(parts) > 1:
            port = int(parts[1])

    print('Testing 128 Protocol Game Server')
    print('Target: %s:%d' % (host, port))
    print('='*60)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    print('[OK] Connected')

    # Login with account_id=1
    login_data = struct.pack('<I', 1)  # account_id
    login_data += struct.pack('<I', 1)  # some flag
    login_data += b'TESTSESSIONKEY' + b'\x00' * 18  # 32 bytes session
    send_recv(sock, 1, login_data, 5)
    print('[OK] Login sent')

    # Select character (uint32_t - slot index or charac_no)
    select_data = struct.pack('<I', 0)  # character slot index 0
    send_recv(sock, 4, select_data, 3)
    print('[OK] Character selected')

    # Start game
    send_recv(sock, 15, b'', 3)
    print('[OK] Game started')

    print('='*60)
    print('Testing new protocols:')
    print('='*60)

    # Test all new protocol categories
    tests = [
        # Trade (75-83)
        (75, struct.pack('<I', 99999), 'TRADE_REQUEST'),
        (83, b'', 'TRADE_CANCEL'),

        # Mail (85-91)
        (85, b'', 'MAIL_GET_LIST'),
        (90, b'', 'MAIL_GET_UNREAD_COUNT'),

        # Friend (95-102)
        (95, b'', 'FRIEND_GET_LIST'),
        (102, b'', 'FRIEND_GET_BLOCKED_LIST'),

        # Guild (105-117)
        (107, b'', 'GUILD_GET_INFO'),
        (114, b'', 'GUILD_GET_MEMBERS'),
        (116, b'', 'GUILD_GET_APPLICATIONS'),

        # Party (120-128)
        (120, b'', 'PARTY_CREATE'),
        (125, b'', 'PARTY_GET_INFO'),

        # PVP (130-138)
        (130, b'', 'PVP_GET_INFO'),
        (138, b'', 'PVP_GET_RANKING'),

        # Ranking (140-145)
        (140, b'', 'RANKING_GET_LEVEL'),
        (141, b'', 'RANKING_GET_WEALTH'),
        (143, struct.pack('<B', 0), 'RANKING_GET_MY_RANK'),
        (144, b'', 'RANKING_GET_GUILD'),
        (145, b'', 'RANKING_GET_ACHIEVEMENT'),

        # Achievement (150-156)
        (150, b'', 'ACHIEVEMENT_GET_LIST'),
        (154, b'', 'ACHIEVEMENT_GET_CATEGORIES'),
        (156, b'', 'ACHIEVEMENT_GET_RECENT'),

        # Title (160-165)
        (160, b'', 'TITLE_GET_LIST'),
        (164, b'', 'TITLE_GET_CURRENT'),
        (165, b'', 'TITLE_GET_UNLOCKABLE'),

        # Auction (170-178)
        (170, struct.pack('<III', 0, 0, 70), 'AUCTION_SEARCH'),
        (174, b'', 'AUCTION_GET_MY_LISTINGS'),
        (175, b'', 'AUCTION_GET_HISTORY'),

        # Storage (180-185)
        (180, b'', 'STORAGE_GET_LIST'),

        # Pet (190-198)
        (190, b'', 'PET_GET_LIST'),
        (195, struct.pack('<I', 80000), 'PET_GET_INFO'),
    ]

    passed = 0
    failed = 0

    for msg_no, data, name in tests:
        r_msg, r_data = send_recv(sock, msg_no, data, 2)
        if r_msg == msg_no:
            print('[OK] %s (msg=%d): %d bytes response' % (name, msg_no, len(r_data)))
            passed += 1
        elif r_msg == -1:
            print('[TIMEOUT] %s (msg=%d)' % (name, msg_no))
            failed += 1
        else:
            print('[FAIL] %s (msg=%d): got msg=%s' % (name, msg_no, r_msg))
            failed += 1

    sock.close()

    print('='*60)
    print('Result: %d passed, %d failed out of %d tests' % (passed, failed, len(tests)))
    print('='*60)

    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
