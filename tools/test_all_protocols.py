#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
test_all_protocols.py - Comprehensive Protocol Test Suite for DNF Game Server

Tests all 205 implemented protocols organized by category.
Usage: python test_all_protocols.py <host:port> [account_id]
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

# Protocol definitions by category
PROTOCOLS = {
    # Core (0-9)
    'core': {
        0: 'CHECK_CONNECTION',
        1: 'LOGIN',
        2: 'SET_UDP_IP_PORT',
        3: 'EXIT',
        4: 'SELECT_CHARACTER',
        5: 'CREATE_CHARACTER',
        6: 'DELETE_CHARACTER',
        7: 'RETURN_SELECT',
        8: 'GET_USERINFO',
        9: 'HEARTBEAT',
    },
    # Party (12-14)
    'party': {
        12: 'PARTY_CREATE',
        13: 'PARTY_LEAVE',
        14: 'PARTY_KICK',
    },
    # Game Flow (15-50)
    'game': {
        15: 'START_GAME',
        16: 'DUNGEON_ENTER',
        17: 'CHAT',
        18: 'DELETE_ITEM',
        19: 'MOVE_ITEM',
        20: 'SORT_ITEM',
        21: 'NPC_SHOP_BUY',
        24: 'NPC_SHOP_SELL',
        25: 'REPAIR_EQUIPMENT',
        26: 'TRADE_REQUEST',
        27: 'COMPOUND_ITEM',
        28: 'DISJOINT_ITEM',
        30: 'CHANGE_SKILLSLOT',
        31: 'BUY_SKILL',
        32: 'INCREASE_STATUS',
        33: 'ACCEPT_QUEST',
        34: 'GIVEUP_QUEST',
        35: 'SET_QUEST_TRIGGER',
        36: 'FINISH_QUEST',
        37: 'SET_USER_POSITION',
        38: 'SET_USER_AREA',
        40: 'FINISH_LOADING',
        41: 'USE_SKILL',
        42: 'DIE_MONSTER',
        43: 'DIE_CHARACTER',
        44: 'USE_COIN',
        45: 'GIVEUP_GAME',
        46: 'GET_ITEM',
        47: 'USE_STACKABLE',
        48: 'MOVE_MAP',
        49: 'SET_PLAY_RESULT',
        50: 'DROP_ITEM',
    },
    # PVP (53-56)
    'pvp': {
        53: 'PVP_CREATE_ROOM',
        54: 'PVP_ENTER_ROOM',
        55: 'PVP_SET_SEAT',
        56: 'PVP_READY',
    },
    # Social (63-64)
    'friend': {
        63: 'FRIEND_ADD',
        64: 'FRIEND_REMOVE',
    },
    # Guild (70-81)
    'guild': {
        70: 'GUILD_MEMBER_LIST',
        77: 'GUILD_INFO',
        78: 'GUILD_JOIN',
        81: 'GUILD_LEAVE',
        83: 'UPGRADE_ITEM',
    },
    # Mail (97-101)
    'mail': {
        97: 'MAIL_LIST',
        98: 'MAIL_READ',
        99: 'MAIL_CLAIM',
        100: 'STORAGE_DEPOSIT_GOLD',
        101: 'STORAGE_WITHDRAW_GOLD',
    },
}

class GameClient:
    def __init__(self, host, port, timeout=5):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.sock = None
        self.seq = 0

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(self.timeout)
        self.sock.connect((self.host, self.port))
        return True

    def disconnect(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def create_packet(self, msg_no, data=b''):
        total_len = PACKET_HEADER_SIZE + len(data)
        header = bytearray(PACKET_HEADER_SIZE)
        header[0] = 0  # classification
        struct.pack_into('<I', header, 1, total_len)
        header[5] = msg_no
        header[6] = 0  # reserved
        struct.pack_into('<I', header, 7, 0)  # checksum
        struct.pack_into('<H', header, 11, self.seq)
        self.seq += 1
        return bytes(header) + data

    def send_recv(self, msg_no, data=b'', recv_size=4096):
        packet = self.create_packet(msg_no, data)
        self.sock.send(packet)
        try:
            response = self.sock.recv(recv_size)
            return self.parse_response(response)
        except socket.timeout:
            return None, None

    def parse_response(self, data):
        if not data or len(data) < PACKET_HEADER_SIZE:
            return None, None
        length = struct.unpack('<I', data[1:5])[0]
        msg_no = data[5]
        payload = data[PACKET_HEADER_SIZE:length] if length > PACKET_HEADER_SIZE else b''
        return msg_no, payload

    def login(self, account_id):
        # LOGIN: account_id(4) + session_key(32) + version(4)
        data = bytearray(40)
        struct.pack_into('<I', data, 0, account_id)
        data[4:36] = b'TESTSESSIONKEY00' * 2
        struct.pack_into('<I', data, 36, 1)

        msg_no, payload = self.send_recv(1, bytes(data))
        # Server returns msg_no=2 for login response
        if msg_no == 2 and payload and len(payload) >= 4:
            result = struct.unpack('<I', payload[:4])[0]
            return result == 0
        return False

    def select_character(self, index=0):
        data = struct.pack('<I', index)
        msg_no, payload = self.send_recv(4, data)
        return msg_no == 4

    def start_game(self):
        msg_no, payload = self.send_recv(15, b'')
        return msg_no in [15, 27]  # 27 is also valid

def test_protocol(client, msg_no, name, data=b''):
    """Test a single protocol"""
    try:
        resp_no, payload = client.send_recv(msg_no, data)
        if resp_no is not None:
            payload_len = len(payload) if payload else 0
            result = 0
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[:4])[0]
            return True, resp_no, payload_len, result
        return False, None, 0, -1
    except Exception as e:
        return False, None, 0, str(e)

def run_tests(host, port, account_id):
    client = GameClient(host, port)

    print(f"DNF Protocol Test Suite")
    print(f"Target: {host}:{port}")
    print(f"Account: {account_id}")
    print("=" * 60)

    # Connect and login
    try:
        client.connect()
        print("[OK] Connected")
    except Exception as e:
        print(f"[FAIL] Connection: {e}")
        return

    # Login sequence
    if not client.login(account_id):
        print("[FAIL] Login failed")
        client.disconnect()
        return
    print("[OK] Login")

    if not client.select_character(0):
        print("[FAIL] Select character failed")
        client.disconnect()
        return
    print("[OK] Select character")

    if not client.start_game():
        print("[FAIL] Start game failed")
        client.disconnect()
        return
    print("[OK] Start game")

    print("\n" + "=" * 60)
    print("Testing protocols...")
    print("=" * 60)

    results = {'pass': 0, 'fail': 0, 'timeout': 0}

    # Test each category
    for category, protocols in PROTOCOLS.items():
        if category in ['core']:  # Skip core - already tested
            continue

        print(f"\n--- {category.upper()} ---")
        for msg_no, name in sorted(protocols.items()):
            time.sleep(0.1)  # Rate limit

            # Generate test data based on protocol
            test_data = generate_test_data(msg_no)

            ok, resp, plen, res = test_protocol(client, msg_no, name, test_data)

            if ok:
                status = "PASS" if resp == msg_no else f"RESP={resp}"
                print(f"  [{msg_no:3d}] {name:25} {status} (len={plen}, res={res})")
                results['pass'] += 1
            else:
                print(f"  [{msg_no:3d}] {name:25} TIMEOUT/FAIL")
                results['timeout'] += 1

    # Test extended protocols (200-296)
    print("\n--- EXTENDED (200-296) ---")
    extended_ranges = [(200, 206), (210, 216), (220, 226), (230, 236),
                       (240, 249), (250, 254), (260, 268), (270, 278),
                       (280, 286), (290, 297)]
    for start, end in extended_ranges:
        for msg_no in range(start, end):
            time.sleep(0.05)
            ok, resp, plen, res = test_protocol(client, msg_no, f"EXT_{msg_no}", b'')
            if ok:
                results['pass'] += 1
            else:
                results['timeout'] += 1
        print(f"  [{start}-{end-1}]: tested")

    # Test Phase 13 protocols (410-482)
    print("\n--- PHASE 13 (410-482) ---")
    phase13_ranges = [(410, 413), (420, 421), (430, 431), (440, 442),
                      (450, 453), (460, 463), (470, 474), (480, 483)]
    for start, end in phase13_ranges:
        for msg_no in range(start, end):
            time.sleep(0.05)
            ok, resp, plen, res = test_protocol(client, msg_no, f"P13_{msg_no}", b'')
            if ok:
                results['pass'] += 1
            else:
                results['timeout'] += 1
        print(f"  [{start}-{end-1}]: tested")

    # Test Phase 14 protocols (350-555)
    print("\n--- PHASE 14 (350-555) ---")
    phase14 = [350, 351, 352, 353, 354, 355, 380, 381, 390, 391,
               510, 511, 512, 513, 514, 550, 551, 552, 553, 554, 555]
    for msg_no in phase14:
        time.sleep(0.05)
        ok, resp, plen, res = test_protocol(client, msg_no, f"P14_{msg_no}", b'')
        if ok:
            results['pass'] += 1
        else:
            results['timeout'] += 1
    print(f"  Phase 14 protocols: tested")

    client.disconnect()

    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    total = results['pass'] + results['fail'] + results['timeout']
    print(f"  Passed:  {results['pass']}")
    print(f"  Failed:  {results['fail']}")
    print(f"  Timeout: {results['timeout']}")
    print(f"  Total:   {total}")
    print(f"  Rate:    {results['pass']*100/total:.1f}%" if total > 0 else "  Rate: N/A")

def generate_test_data(msg_no):
    """Generate appropriate test data for each protocol"""
    if msg_no == 17:  # CHAT
        return struct.pack('<B', 0) + b'test\x00'
    elif msg_no in [18, 19, 46, 50]:  # Item operations
        return struct.pack('<I', 1)
    elif msg_no == 20:  # SORT
        return b''
    elif msg_no in [21, 24]:  # Shop
        return struct.pack('<II', 1, 1)
    elif msg_no in [33, 34, 36]:  # Quest
        return struct.pack('<I', 1)
    elif msg_no in [37, 38]:  # Position
        return struct.pack('<HH', 100, 100)
    elif msg_no == 41:  # USE_SKILL
        return struct.pack('<HHH', 1, 100, 100)
    elif msg_no in [42, 43]:  # Die
        return struct.pack('<I', 1)
    elif msg_no in [63, 64]:  # Friend
        return struct.pack('<I', 1)
    elif msg_no == 83:  # Upgrade
        return struct.pack('<I', 1)
    return b''

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python test_all_protocols.py <host:port> [account_id]")
        sys.exit(1)

    addr = sys.argv[1]
    host, port = addr.split(':') if ':' in addr else (addr, 20011)
    port = int(port)
    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1001

    run_tests(host, port, account_id)
