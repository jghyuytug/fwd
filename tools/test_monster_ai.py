#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_monster_ai.py - Monster AI Test Client

测试副本怪物AI功能：
1. LOGIN -> SELECT_CHARACTER -> DUNGEON_ENTER
2. DUNGEON_ATTACK (攻击怪物)
3. DUNGEON_MONSTER_TICK (214) - 怪物AI更新
4. DUNGEON_PLAYER_HIT (215) - 玩家受击
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

def create_packet(msg_no, data=b'', seq=0):
    """创建数据包"""
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
    """解析响应"""
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
    """接收完整响应"""
    sock.settimeout(timeout)
    try:
        data = sock.recv(4096)
        return data
    except socket.timeout:
        return None

def test_login(sock, seq, account_id):
    """登录"""
    print("\n=== LOGIN (msg_no=1) ===")
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    session_key = b'TESTSESSIONKEY00' * 2
    data[4:36] = session_key
    struct.pack_into('<I', data, 36, 1)

    packet = create_packet(1, bytes(data), seq)
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header and payload:
            result = struct.unpack('<I', payload[0:4])[0]
            char_count = struct.unpack('<I', payload[4:8])[0] if len(payload) >= 8 else 0
            print(f"  Result: {result}, Characters: {char_count}")
            return result == 0
    return False

def test_select_character(sock, seq, char_index=0):
    """选择角色"""
    print("\n=== SELECT_CHARACTER (msg_no=4) ===")
    data = struct.pack('<I', char_index)
    packet = create_packet(4, data, seq)
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header and payload:
            result = struct.unpack('<I', payload[0:4])[0]
            print(f"  Result: {result}")
            return result == 0
    return False

def test_dungeon_enter(sock, seq, dungeon_id=1, difficulty=1):
    """进入副本 (msg_no=16) - SELECT_DUNGEON = DUNGEON_ENTER"""
    print(f"\n=== DUNGEON_ENTER (msg_no=16) dungeon={dungeon_id} difficulty={difficulty} ===")
    data = struct.pack('<II', dungeon_id, difficulty)
    packet = create_packet(16, data, seq)  # 16 = SELECT_DUNGEON
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header:
            print(f"  Response msg_no={header['msg_no']} len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result: {result}")

                # 解析副本信息
                # Format: result(4) + dungeon_id(4) + difficulty(1) + total_rooms(1) + current_room(1) + mon_count(1) + monsters
                if len(payload) >= 12:
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

                    # 解析怪物列表: id(4) + type(2) + x(2) + y(2) + hp(4) = 14 bytes each
                    for i in range(monster_count):
                        if offset + 14 <= len(payload):
                            m_id = struct.unpack('<I', payload[offset:offset+4])[0]
                            m_type = struct.unpack('<H', payload[offset+4:offset+6])[0]
                            m_x = struct.unpack('<H', payload[offset+6:offset+8])[0]
                            m_y = struct.unpack('<H', payload[offset+8:offset+10])[0]
                            m_hp = struct.unpack('<I', payload[offset+10:offset+14])[0]
                            offset += 14
                            print(f"  Monster {i+1}: id={m_id} type={m_type} pos=({m_x},{m_y}) hp={m_hp}")

                return result == 0
    return False

def test_dungeon_attack(sock, seq, monster_id, skill_id=100, skill_level=1):
    """攻击怪物 (msg_no=212) - 使用扩展协议号 + 技能伤害公式"""
    print(f"\n=== DUNGEON_ATTACK (msg_no=212) monster={monster_id} skill={skill_id} lv={skill_level} ===")
    # New format: monster_id(4) + skill_id(2) + skill_level(1)
    data = struct.pack('<IHB', monster_id, skill_id, skill_level)
    packet = create_packet(212, data, seq)  # 212 = Custom DUNGEON_ATTACK
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header:
            print(f"  Response msg_no={header['msg_no']} len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result: {result}")

                # New response format: result(4) + monster_id(4) + damage(4) + remaining_hp(4) + killed(1) + crit(1) + hits(1) + room_cleared(1)
                if len(payload) >= 20:
                    m_id = struct.unpack('<I', payload[4:8])[0]
                    damage = struct.unpack('<I', payload[8:12])[0]
                    remaining_hp = struct.unpack('<I', payload[12:16])[0]
                    killed = payload[16]
                    is_crit = payload[17]
                    hit_count = payload[18]
                    room_cleared = payload[19]
                    print(f"  Monster ID: {m_id}")
                    print(f"  Damage dealt: {damage}" + (" (CRIT!)" if is_crit else ""))
                    print(f"  Hit count: {hit_count}")
                    print(f"  Remaining HP: {remaining_hp}")
                    print(f"  Killed: {killed}")
                    print(f"  Room cleared: {room_cleared}")
                return result == 0
    return False

def test_monster_tick(sock, seq, player_x=100, player_y=100, current_time=1000):
    """怪物AI更新 (msg_no=214)"""
    print(f"\n=== MONSTER_TICK (msg_no=214) player=({player_x},{player_y}) time={current_time} ===")
    data = struct.pack('<HHI', player_x, player_y, current_time)
    packet = create_packet(214, data, seq)
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header:
            print(f"  Response msg_no={header['msg_no']} len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result: {result}")

                # Format: result(4) + monster_count(1) + monsters[id(4)+x(2)+y(2)+state(1)+hp(4)=13] + attacks_count(1)
                if len(payload) >= 5:
                    update_count = payload[4]
                    print(f"  Monster updates: {update_count}")

                    offset = 5
                    for i in range(update_count):
                        if offset + 13 <= len(payload):
                            m_id = struct.unpack('<I', payload[offset:offset+4])[0]
                            m_x = struct.unpack('<H', payload[offset+4:offset+6])[0]
                            m_y = struct.unpack('<H', payload[offset+6:offset+8])[0]
                            state = payload[offset+8]
                            m_hp = struct.unpack('<I', payload[offset+9:offset+13])[0]
                            offset += 13
                            state_names = ['IDLE', 'PATROL', 'CHASE', 'ATTACK', 'HIT_STUN', 'DEAD']
                            state_name = state_names[state] if state < len(state_names) else f'UNKNOWN({state})'
                            print(f"    Monster {m_id}: state={state_name} pos=({m_x},{m_y}) hp={m_hp}")

                    # Parse attacks
                    if offset < len(payload):
                        attack_count = payload[offset]
                        offset += 1
                        if attack_count > 0:
                            print(f"  Pending attacks: {attack_count}")
                            for i in range(attack_count):
                                if offset + 5 <= len(payload):
                                    mon_idx = payload[offset]
                                    dmg = struct.unpack('<I', payload[offset+1:offset+5])[0]
                                    offset += 5
                                    print(f"    Attack from monster {mon_idx}: damage={dmg}")
                return result == 0
    return False

def test_player_hit(sock, seq, monster_id):
    """玩家受击 (msg_no=215)"""
    print(f"\n=== PLAYER_HIT (msg_no=215) monster={monster_id} ===")
    data = struct.pack('<I', monster_id)
    packet = create_packet(215, data, seq)
    sock.send(packet)

    response = recv_full(sock)
    if response:
        header, payload = parse_response(response)
        if header:
            print(f"  Response msg_no={header['msg_no']} len={header['length']}")
            if payload and len(payload) >= 4:
                result = struct.unpack('<I', payload[0:4])[0]
                print(f"  Result: {result}")

                if len(payload) >= 12:
                    damage = struct.unpack('<I', payload[4:8])[0]
                    remaining_hp = struct.unpack('<I', payload[8:12])[0]
                    print(f"  Damage taken: {damage}")
                    print(f"  Remaining HP: {remaining_hp}")
                return result == 0
    return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_monster_ai.py <host:port> [account_id]")
        print("Example: python test_monster_ai.py 192.168.200.131:20011 1")
        sys.exit(1)

    addr = sys.argv[1]
    if ':' in addr:
        host, port = addr.split(':')
        port = int(port)
    else:
        host = addr
        port = 20011

    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print("=== Monster AI Test Client ===")
    print(f"Target: {host}:{port}")
    print(f"Account ID: {account_id}")
    print("=" * 50)

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        sock.connect((host, port))
        print(f"[OK] Connected to {host}:{port}")
    except Exception as e:
        print(f"[FAIL] Connection failed: {e}")
        sys.exit(1)

    seq = 0

    try:
        # 1. 登录
        if not test_login(sock, seq, account_id):
            print("[FAIL] Login failed")
            return
        seq += 1
        time.sleep(0.3)

        # 2. 选择角色
        if not test_select_character(sock, seq, 0):
            print("[FAIL] Select character failed")
            return
        seq += 1
        time.sleep(0.3)

        # 3. 进入副本 (difficulty=1 正常难度, 应该有4个房间)
        if not test_dungeon_enter(sock, seq, 1, 1):
            print("[FAIL] Dungeon enter failed")
            return
        seq += 1
        time.sleep(0.3)

        # 4. 测试怪物AI更新 - 模拟玩家在(100,100)位置
        test_monster_tick(sock, seq, 100, 100, 1000)
        seq += 1
        time.sleep(0.3)

        # 5. 测试怪物AI更新 - 移动到(200,200)
        test_monster_tick(sock, seq, 200, 200, 2000)
        seq += 1
        time.sleep(0.3)

        # 6. 使用普通攻击 (skill_id=100) 攻击怪物1
        test_dungeon_attack(sock, seq, 1, skill_id=100, skill_level=1)
        seq += 1
        time.sleep(0.3)

        # 7. 使用 Fighter 技能 (skill_id=303 Hundred Fists - 8 hits!) 攻击怪物2
        test_dungeon_attack(sock, seq, 2, skill_id=303, skill_level=10)
        seq += 1
        time.sleep(0.3)

        # 8. 使用 Gunner 技能 (skill_id=401 Rapid Fire - 5 hits hybrid) 攻击怪物3
        test_dungeon_attack(sock, seq, 3, skill_id=401, skill_level=5)
        seq += 1
        time.sleep(0.3)

        # 9. 再次更新AI (怪物应该进入HIT_STUN)
        test_monster_tick(sock, seq, 200, 200, 3000)
        seq += 1
        time.sleep(0.3)

        # 10. 测试玩家被怪物攻击
        test_player_hit(sock, seq, 1)
        seq += 1

    finally:
        sock.close()
        print("\n=== Test completed ===")

if __name__ == '__main__':
    main()
