#!/usr/bin/env python3
"""
DNF Dungeon System Test Client
Tests the complete dungeon flow: enter -> attack -> next room -> clear -> reward
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

def build_packet(msg_no, data=b''):
    total_len = PACKET_HEADER_SIZE + len(data)
    # Header: class(1) + length(4) + msg_no(1) + reserved(1) + checksum(4) + seq(2) = 13 bytes
    header = struct.pack('<BIBBIH', 0, total_len, msg_no, 0, 0, 0)
    return header + data

def recv_packet(sock, timeout=5):
    sock.settimeout(timeout)
    try:
        # Receive header
        header = b''
        while len(header) < PACKET_HEADER_SIZE:
            chunk = sock.recv(PACKET_HEADER_SIZE - len(header))
            if not chunk:
                return None, None, None
            header += chunk

        # Header format: class(1) + length(4) + msg_no(1) + reserved(1) + checksum(4) + seq(2) = 13 bytes
        classification = header[0]
        length = struct.unpack('<I', header[1:5])[0]
        msg_no = header[5]

        payload_len = length - PACKET_HEADER_SIZE
        payload = b''
        while len(payload) < payload_len:
            chunk = sock.recv(payload_len - len(payload))
            if not chunk:
                break
            payload += chunk

        return msg_no, length, payload
    except socket.timeout:
        return None, None, None

def login_and_start(sock, account_id):
    """Login -> Select Character -> Start Game"""
    # LOGIN - 40 bytes: account_id(4) + session_key(32) + version(4)
    login_data = bytearray(40)
    struct.pack_into('<I', login_data, 0, account_id)
    login_data[4:36] = b'TESTSESSIONKEY00' * 2  # 32 bytes session key
    struct.pack_into('<I', login_data, 36, 1)  # version
    sock.send(build_packet(1, bytes(login_data)))
    msg_no, length, payload = recv_packet(sock)
    # Server returns msg_no=2 for login response
    if msg_no != 2 or (payload and struct.unpack('<I', payload[:4])[0] != 0):
        print(f"[FAIL] Login failed (msg_no={msg_no})")
        return False
    print(f"[OK] Login success")

    # SELECT CHARACTER
    sock.send(build_packet(4, struct.pack('<I', 0)))
    msg_no, length, payload = recv_packet(sock)
    if msg_no != 4:
        print(f"[FAIL] Select character failed")
        return False
    print(f"[OK] Character selected")

    # START GAME
    sock.send(build_packet(15, b''))
    msg_no, length, payload = recv_packet(sock)
    # Server may return msg_no=27 (COMPOUND_ITEM response) or 15
    if msg_no not in [15, 27]:
        print(f"[FAIL] Start game failed (msg_no={msg_no})")
        return False
    print(f"[OK] Game started")

    return True

def test_dungeon_enter(sock, dungeon_id=1, difficulty=0):
    """Test DUNGEON_ENTER (msg_no=60)"""
    print(f"\n=== DUNGEON_ENTER (id={dungeon_id}, diff={difficulty}) ===")

    data = struct.pack('<IB', dungeon_id, difficulty)
    sock.send(build_packet(60, data))

    msg_no, length, payload = recv_packet(sock)
    print(f"  Response: msg_no={msg_no}, length={length}, payload_len={len(payload) if payload else 0}")
    if payload:
        print(f"  Raw payload: {payload[:50].hex() if len(payload) > 50 else payload.hex()}")

    if msg_no != 60:
        print(f"  [FAIL] Unexpected msg_no={msg_no}")
        return None

    if not payload or len(payload) < 4:
        print(f"  [FAIL] Payload too short")
        return None

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        print(f"  [FAIL] Result={result}")
        return None

    if len(payload) < 12:
        print(f"  [OK] Dungeon entered (minimal response)")
        return []

    d_id = struct.unpack('<I', payload[4:8])[0]
    diff = payload[8]
    total_rooms = payload[9]
    current_room = payload[10]
    monster_count = payload[11]

    print(f"  Dungeon ID: {d_id}")
    print(f"  Difficulty: {diff}")
    print(f"  Rooms: {current_room}/{total_rooms}")
    print(f"  Monsters: {monster_count}")

    monsters = []
    offset = 12
    for i in range(monster_count):
        if offset + 14 > len(payload):
            break
        m_id = struct.unpack('<I', payload[offset:offset+4])[0]
        m_type = struct.unpack('<H', payload[offset+4:offset+6])[0]
        m_x = struct.unpack('<H', payload[offset+6:offset+8])[0]
        m_y = struct.unpack('<H', payload[offset+8:offset+10])[0]
        m_hp = struct.unpack('<I', payload[offset+10:offset+14])[0]
        monsters.append({'id': m_id, 'type': m_type, 'x': m_x, 'y': m_y, 'hp': m_hp})
        print(f"    Monster {i}: id={m_id} type={m_type} pos=({m_x},{m_y}) hp={m_hp}")
        offset += 14

    return monsters

def test_dungeon_attack(sock, monster_id, damage):
    """Test DUNGEON_ATTACK (msg_no=65)"""
    data = struct.pack('<II', monster_id, damage)
    sock.send(build_packet(65, data))

    msg_no, length, payload = recv_packet(sock)
    if msg_no != 65:
        return None, False, False

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        return None, False, False

    m_id = struct.unpack('<I', payload[4:8])[0]
    remaining_hp = struct.unpack('<I', payload[8:12])[0]
    killed = payload[12] == 1
    room_cleared = payload[13] == 1

    return remaining_hp, killed, room_cleared

def test_dungeon_next_room(sock):
    """Test DUNGEON_NEXT_ROOM (msg_no=66)"""
    print(f"\n=== DUNGEON_NEXT_ROOM ===")

    sock.send(build_packet(66, b''))

    msg_no, length, payload = recv_packet(sock)
    if msg_no != 66:
        print(f"  [FAIL] No response")
        return None

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        print(f"  [FAIL] Result={result}")
        return None

    current_room = payload[4]
    total_rooms = payload[5]
    monster_count = payload[6]

    print(f"  Room: {current_room}/{total_rooms}")
    print(f"  Monsters: {monster_count}")

    monsters = []
    offset = 7
    for i in range(monster_count):
        if offset + 14 > len(payload):
            break
        m_id = struct.unpack('<I', payload[offset:offset+4])[0]
        m_type = struct.unpack('<H', payload[offset+4:offset+6])[0]
        m_x = struct.unpack('<H', payload[offset+6:offset+8])[0]
        m_y = struct.unpack('<H', payload[offset+8:offset+10])[0]
        m_hp = struct.unpack('<I', payload[offset+10:offset+14])[0]
        monsters.append({'id': m_id, 'type': m_type, 'x': m_x, 'y': m_y, 'hp': m_hp})
        print(f"    Monster {i}: id={m_id} hp={m_hp}")
        offset += 14

    return monsters

def test_dungeon_clear(sock):
    """Test DUNGEON_CLEAR (msg_no=70)"""
    print(f"\n=== DUNGEON_CLEAR ===")

    sock.send(build_packet(70, b''))

    msg_no, length, payload = recv_packet(sock)
    if msg_no != 70:
        print(f"  [FAIL] No response")
        return False

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        print(f"  [FAIL] Result={result}")
        return False

    clear_time = struct.unpack('<I', payload[4:8])[0]
    rank = payload[8]
    kills = struct.unpack('<I', payload[9:13])[0]
    exp_reward = struct.unpack('<I', payload[13:17])[0]
    gold_reward = struct.unpack('<I', payload[17:21])[0]

    rank_chars = "SABCD"
    print(f"  Clear Time: {clear_time} sec")
    print(f"  Rank: {rank_chars[rank] if rank < 5 else '?'}")
    print(f"  Kills: {kills}")
    print(f"  EXP Reward: {exp_reward}")
    print(f"  Gold Reward: {gold_reward}")

    return True

def test_dungeon_info(sock):
    """Test DUNGEON_INFO (msg_no=62)"""
    print(f"\n=== DUNGEON_INFO ===")

    sock.send(build_packet(62, b''))

    msg_no, length, payload = recv_packet(sock)
    if msg_no != 62:
        print(f"  [FAIL] No response")
        return

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        print(f"  Not in dungeon")
        return

    d_id = struct.unpack('<I', payload[4:8])[0]
    diff = payload[8]
    current = payload[9]
    total = payload[10]
    remaining = struct.unpack('<I', payload[11:15])[0]
    alive = payload[15]
    kills = struct.unpack('<I', payload[16:20])[0]

    print(f"  Dungeon: {d_id}, Diff: {diff}")
    print(f"  Room: {current}/{total}")
    print(f"  Time Remaining: {remaining} sec")
    print(f"  Monsters Alive: {alive}")
    print(f"  Total Kills: {kills}")

def test_dungeon_exit(sock):
    """Test DUNGEON_EXIT (msg_no=61)"""
    print(f"\n=== DUNGEON_EXIT ===")

    sock.send(build_packet(61, b''))

    msg_no, length, payload = recv_packet(sock)
    if msg_no != 61:
        print(f"  [FAIL] No response")
        return False

    result = struct.unpack('<I', payload[:4])[0]
    if result != 0:
        print(f"  [FAIL] Result={result}")
        return False

    kills = struct.unpack('<I', payload[4:8])[0]
    print(f"  Exited with {kills} kills")
    return True

def run_full_dungeon_test(host, port, account_id):
    """Run complete dungeon flow test"""
    print("=" * 60)
    print("DNF Dungeon System Test")
    print(f"Target: {host}:{port}")
    print("=" * 60)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    print(f"[OK] Connected")

    # Login and start game
    if not login_and_start(sock, account_id):
        sock.close()
        return

    # Enter dungeon (Normal difficulty)
    monsters = test_dungeon_enter(sock, dungeon_id=1, difficulty=0)
    if not monsters:
        sock.close()
        return

    # Check dungeon info
    test_dungeon_info(sock)

    total_rooms = 3  # Normal difficulty = 3 rooms

    for room in range(total_rooms):
        print(f"\n--- Room {room} ---")

        # Attack all monsters until room is cleared
        room_cleared = False
        attack_count = 0
        while not room_cleared and attack_count < 50:
            for m in monsters:
                if m['hp'] <= 0:
                    continue

                # Attack with high damage to kill quickly
                damage = 50000
                remaining_hp, killed, room_cleared = test_dungeon_attack(sock, m['id'], damage)

                if remaining_hp is not None:
                    m['hp'] = remaining_hp
                    status = "KILLED" if killed else f"HP={remaining_hp}"
                    print(f"  Attack monster {m['id']}: {status}")

                if room_cleared:
                    print(f"  [ROOM CLEARED]")
                    break

                attack_count += 1

        # Move to next room (except for last room)
        if room < total_rooms - 1:
            monsters = test_dungeon_next_room(sock)
            if not monsters:
                print("[FAIL] Failed to move to next room")
                break

    # Clear dungeon (after final room)
    success = test_dungeon_clear(sock)

    # Final dungeon info (should show not in dungeon)
    test_dungeon_info(sock)

    sock.close()
    print("\n" + "=" * 60)
    print("Dungeon test completed!")
    print("=" * 60)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <host:port> [account_id]")
        print(f"Example: {sys.argv[0]} 192.168.200.131:20011 1")
        sys.exit(1)

    target = sys.argv[1]
    if ':' in target:
        host, port = target.split(':')
        port = int(port)
    else:
        host = target
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    run_full_dungeon_test(host, port, account_id)
