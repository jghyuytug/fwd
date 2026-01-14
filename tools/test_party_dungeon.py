#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_party_dungeon.py - Party Dungeon Sync Test Client

Tests the party dungeon synchronization logic:
1. Two players create/join party
2. Leader enters dungeon (creates shared instance)
3. Member joins dungeon (joins shared instance)
4. Both attack monsters (damage synced)
5. Room cleared and advanced

Protocol Numbers (from server):
- 12 = PARTY_CREATE (SET_PARTY_INFO)
- 16 = DUNGEON_ENTER (SELECT_DUNGEON)
- 211 = DUNGEON_EXIT
- 212 = DUNGEON_ATTACK
- 221 = PARTY_JOIN
"""

import socket
import struct
import sys
import time
import threading

PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    """Create packet"""
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
    """Parse response"""
    if len(data) < PACKET_HEADER_SIZE:
        return None, None

    classification = data[0]
    length = struct.unpack('<I', data[1:5])[0]
    msg_no = data[5]
    reserved = data[6]
    checksum = struct.unpack('<I', data[7:11])[0]
    sequence = struct.unpack('<H', data[11:13])[0]

    header = {
        'classification': classification,
        'length': length,
        'msg_no': msg_no,
        'reserved': reserved,
        'checksum': checksum,
        'sequence': sequence
    }

    payload = data[PACKET_HEADER_SIZE:length] if length > PACKET_HEADER_SIZE else b''
    return header, payload

def recv_full(sock, timeout=5):
    """Receive full response"""
    sock.settimeout(timeout)
    try:
        data = sock.recv(4096)
        return data
    except socket.timeout:
        return None

def recv_until_msg(sock, expected_msg_no, timeout=5):
    """Receive packets until we get the expected msg_no"""
    sock.settimeout(timeout)
    all_data = b''
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            data = sock.recv(4096)
            if not data:
                break
            all_data += data
            # Parse all packets in the buffer
            offset = 0
            while offset + PACKET_HEADER_SIZE <= len(all_data):
                pkt_len = struct.unpack('<I', all_data[offset+1:offset+5])[0]
                if offset + pkt_len > len(all_data):
                    break  # Incomplete packet
                msg_no = all_data[offset + 5]
                if msg_no == expected_msg_no:
                    return all_data[offset:offset+pkt_len]
                offset += pkt_len
        except socket.timeout:
            continue
    return all_data if all_data else None

class GameClient:
    def __init__(self, host, port, account_id, name):
        self.host = host
        self.port = port
        self.account_id = account_id
        self.name = name
        self.sock = None
        self.seq = 0
        self.party_id = 0
        self.charac_no = 0
        self.in_dungeon = False

    def connect(self):
        """Connect to server"""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(10)
        self.sock.connect((self.host, self.port))
        print(f"[{self.name}] Connected to {self.host}:{self.port}")
        return True

    def login(self):
        """Login"""
        print(f"\n[{self.name}] === LOGIN (msg_no=1) ===")
        data = bytearray(40)
        struct.pack_into('<I', data, 0, self.account_id)
        session_key = b'TESTSESSIONKEY00' * 2
        data[4:36] = session_key
        struct.pack_into('<I', data, 36, 1)

        packet = create_packet(1, bytes(data), self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            if header and payload:
                result = struct.unpack('<I', payload[0:4])[0]
                char_count = struct.unpack('<I', payload[4:8])[0] if len(payload) >= 8 else 0
                print(f"  Result: {result}, Characters: {char_count}")
                return result == 0
        return False

    def select_character(self, char_index=0):
        """Select character"""
        print(f"\n[{self.name}] === SELECT_CHARACTER (msg_no=4) ===")
        data = struct.pack('<I', char_index)
        packet = create_packet(4, data, self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            if header and payload:
                result = struct.unpack('<I', payload[0:4])[0]
                # Extract charac_no if available (offset varies by response format)
                if len(payload) >= 8:
                    self.charac_no = struct.unpack('<I', payload[4:8])[0]
                print(f"  Result: {result}, charac_no: {self.charac_no}")
                return result == 0
        return False

    def create_party(self):
        """Create party (msg_no=12) - SET_PARTY_INFO in production"""
        print(f"\n[{self.name}] === PARTY_CREATE (msg_no=12) ===")
        data = struct.pack('<I', 0)  # loot_mode = free
        packet = create_packet(12, data, self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            print(f"  Response: msg_no={header['msg_no'] if header else 'N/A'} len={len(payload) if payload else 0}")
            if header and payload and len(payload) >= 8:
                result = struct.unpack('<I', payload[0:4])[0]
                self.party_id = struct.unpack('<I', payload[4:8])[0]
                print(f"  Result: {result}, Party ID: {self.party_id}")
                return result == 0
            elif header and payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                # If no party_id returned, use charac_no as fallback party identifier
                self.party_id = self.charac_no if self.charac_no > 0 else 1
                print(f"  Result: {result}, Using Party ID: {self.party_id}")
                return result == 0
        return False

    def join_party(self, party_id):
        """Join party (msg_no=221) - Custom PARTY_JOIN"""
        print(f"\n[{self.name}] === PARTY_JOIN (msg_no=221) party={party_id} ===")
        self.party_id = party_id
        data = struct.pack('<I', party_id)
        packet = create_packet(221, data, self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            print(f"  Response: msg_no={header['msg_no'] if header else 'N/A'} len={len(payload) if payload else 0}")
            if header and payload:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result: {result}")
                return result == 0
        return False

    def enter_dungeon(self, dungeon_id=1, difficulty=1):
        """Enter dungeon (msg_no=16) - SELECT_DUNGEON with party support"""
        print(f"\n[{self.name}] === DUNGEON_ENTER (msg_no=16) dungeon={dungeon_id} diff={difficulty} ===")
        data = struct.pack('<II', dungeon_id, difficulty)
        packet = create_packet(16, data, self.seq)
        self.sock.send(packet)
        self.seq += 1

        # Use recv_until_msg to find msg_no=60 (DUNGEON_ENTER response)
        response = recv_until_msg(self.sock, 60, timeout=3)
        if response:
            header, payload = parse_response(response)
            if header:
                print(f"  Response msg_no={header['msg_no']} len={header['length']}")
                if payload and len(payload) >= 4:
                    result = struct.unpack('<I', payload[0:4])[0]
                    print(f"  Result: {result}")

                    if result == 0 and len(payload) >= 12:
                        offset = 4
                        d_id = struct.unpack('<I', payload[offset:offset+4])[0]
                        offset += 4
                        difficulty = payload[offset]
                        offset += 1
                        total_rooms = payload[offset]
                        offset += 1
                        current_room = payload[offset]
                        offset += 1
                        monster_count = payload[offset]
                        offset += 1

                        print(f"  Dungeon ID: {d_id}")
                        print(f"  Difficulty: {difficulty}")
                        print(f"  Total Rooms: {total_rooms}")
                        print(f"  Current Room: {current_room}")
                        print(f"  Monster Count: {monster_count}")

                        # Check for party dungeon fields if present
                        if len(payload) >= offset + 2:
                            is_party = payload[offset] if offset < len(payload) else 0
                            member_count = payload[offset+1] if offset+1 < len(payload) else 0
                            print(f"  Is Party Dungeon: {is_party}")
                            print(f"  Party Members: {member_count}")
                            offset += 2

                        for i in range(monster_count):
                            if offset + 14 <= len(payload):
                                m_id = struct.unpack('<I', payload[offset:offset+4])[0]
                                m_type = struct.unpack('<H', payload[offset+4:offset+6])[0]
                                m_x = struct.unpack('<H', payload[offset+6:offset+8])[0]
                                m_y = struct.unpack('<H', payload[offset+8:offset+10])[0]
                                m_hp = struct.unpack('<I', payload[offset+10:offset+14])[0]
                                offset += 14
                                print(f"    Monster {i+1}: id={m_id} type={m_type} pos=({m_x},{m_y}) hp={m_hp}")

                        self.in_dungeon = True
                    return result == 0
        return False

    def attack_monster(self, monster_id, skill_id=100, skill_level=1):
        """Attack monster (msg_no=212) - Custom DUNGEON_ATTACK"""
        print(f"\n[{self.name}] === DUNGEON_ATTACK (msg_no=212) monster={monster_id} skill={skill_id} ===")
        data = struct.pack('<IHB', monster_id, skill_id, skill_level)
        packet = create_packet(212, data, self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            if header:
                print(f"  Response msg_no={header['msg_no']} len={header['length']}")
                if payload and len(payload) >= 4:
                    result = struct.unpack('<I', payload[0:4])[0]
                    print(f"  Result: {result}")

                    if len(payload) >= 20:
                        m_id = struct.unpack('<I', payload[4:8])[0]
                        damage = struct.unpack('<I', payload[8:12])[0]
                        remaining_hp = struct.unpack('<I', payload[12:16])[0]
                        killed = payload[16]
                        is_crit = payload[17]
                        hit_count = payload[18]
                        room_cleared = payload[19]
                        attacker_id = struct.unpack('<I', payload[20:24])[0] if len(payload) >= 24 else 0
                        print(f"  Monster ID: {m_id}")
                        print(f"  Damage dealt: {damage}" + (" (CRIT!)" if is_crit else ""))
                        print(f"  Hit count: {hit_count}")
                        print(f"  Remaining HP: {remaining_hp}")
                        print(f"  Killed: {killed}")
                        print(f"  Room cleared: {room_cleared}")
                        print(f"  Attacker ID: {attacker_id}")
                    return result == 0
        return False

    def exit_dungeon(self):
        """Exit dungeon (msg_no=211) - Custom DUNGEON_EXIT"""
        print(f"\n[{self.name}] === DUNGEON_EXIT (msg_no=211) ===")
        packet = create_packet(211, b'', self.seq)
        self.sock.send(packet)
        self.seq += 1

        response = recv_full(self.sock)
        if response:
            header, payload = parse_response(response)
            if header and payload:
                result = struct.unpack('<I', payload[0:4])[0]
                kills = struct.unpack('<I', payload[4:8])[0] if len(payload) >= 8 else 0
                print(f"  Result: {result}, Kills: {kills}")
                self.in_dungeon = False
                return result == 0
        return False

    def close(self):
        if self.sock:
            self.sock.close()

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_party_dungeon.py <host:port> [account_id1] [account_id2]")
        print("Example: python test_party_dungeon.py 192.168.200.131:20011 1 2")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id1 = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    account_id2 = int(sys.argv[3]) if len(sys.argv) > 3 else 2

    print("=== Party Dungeon Sync Test ===")
    print(f"Target: {host}:{port}")
    print(f"Leader Account: {account_id1}")
    print(f"Member Account: {account_id2}")
    print("=" * 50)

    # Create two clients
    leader = GameClient(host, port, account_id1, "LEADER")
    member = GameClient(host, port, account_id2, "MEMBER")

    try:
        # Connect both clients
        if not leader.connect() or not member.connect():
            print("[FAIL] Failed to connect")
            return

        # Login both clients
        if not leader.login() or not member.login():
            print("[FAIL] Login failed")
            return
        time.sleep(0.3)

        # Select characters
        if not leader.select_character(0) or not member.select_character(0):
            print("[FAIL] Select character failed")
            return
        time.sleep(0.3)

        # Leader creates party
        if not leader.create_party():
            print("[WARN] Create party returned error, continuing anyway...")
        time.sleep(0.3)

        # Member joins party
        if leader.party_id > 0:
            if not member.join_party(leader.party_id):
                print("[WARN] Join party returned error, continuing anyway...")
            time.sleep(0.3)

        # Leader enters dungeon first
        print("\n" + "="*50)
        print("LEADER ENTERING DUNGEON")
        print("="*50)
        if not leader.enter_dungeon(1, 1):
            print("[FAIL] Leader enter dungeon failed")
            return
        time.sleep(0.5)

        # Member enters dungeon (should join shared instance)
        print("\n" + "="*50)
        print("MEMBER ENTERING DUNGEON (should join shared instance)")
        print("="*50)
        if not member.enter_dungeon(1, 1):
            print("[FAIL] Member enter dungeon failed")
            return
        time.sleep(0.5)

        # Leader attacks monster 1
        print("\n" + "="*50)
        print("LEADER ATTACKING MONSTER 1")
        print("="*50)
        leader.attack_monster(1, skill_id=100, skill_level=1)
        time.sleep(0.3)

        # Member attacks monster 2
        print("\n" + "="*50)
        print("MEMBER ATTACKING MONSTER 2")
        print("="*50)
        member.attack_monster(2, skill_id=303, skill_level=10)
        time.sleep(0.3)

        # Leader attacks monster 1 again (should see updated state from shared dungeon)
        print("\n" + "="*50)
        print("LEADER ATTACKING MONSTER 1 AGAIN (damage should be synced)")
        print("="*50)
        leader.attack_monster(1, skill_id=401, skill_level=5)
        time.sleep(0.3)

        # Both exit dungeon
        print("\n" + "="*50)
        print("BOTH EXITING DUNGEON")
        print("="*50)
        leader.exit_dungeon()
        time.sleep(0.2)
        member.exit_dungeon()

    finally:
        leader.close()
        member.close()
        print("\n=== Party Dungeon Test completed ===")

if __name__ == '__main__':
    main()
