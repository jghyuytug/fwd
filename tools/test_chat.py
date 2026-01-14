#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
test_chat.py - Chat System Test Client

Tests chat protocols:
- msg_no=10: CHAT (normal/world/guild/party/whisper)
"""

import socket
import struct
import sys
import time

PACKET_HEADER_SIZE = 13

# Chat types
CHAT_TYPE_NORMAL = 0
CHAT_TYPE_WHISPER = 1
CHAT_TYPE_PARTY = 2
CHAT_TYPE_GUILD = 3
CHAT_TYPE_WORLD = 4

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

def login_flow(sock, account_id):
    """Login -> Select Character -> Start Game"""
    seq = 0

    # LOGIN
    data = bytearray(40)
    struct.pack_into('<I', data, 0, account_id)
    data[4:36] = b'TESTSESSIONKEY00' * 2
    struct.pack_into('<I', data, 36, 1)
    sock.send(create_packet(1, bytes(data), seq))
    sock.recv(2048)
    seq += 1
    time.sleep(0.1)

    # SELECT CHARACTER
    sock.send(create_packet(4, struct.pack('<I', 0), seq))
    sock.recv(4096)
    seq += 1
    time.sleep(0.1)

    # START GAME
    sock.send(create_packet(15, struct.pack('<II', 1, 0), seq))
    sock.recv(2048)
    seq += 1
    time.sleep(0.1)

    return seq

def send_chat(sock, seq, chat_type, message, target_name=None):
    """Send chat message (msg_no=10)"""
    msg_bytes = message.encode('utf-8')[:250]

    data = bytearray()
    data.append(chat_type)
    data.append(len(msg_bytes))
    data.extend(msg_bytes)

    # For whisper, append target name
    if chat_type == CHAT_TYPE_WHISPER and target_name:
        name_bytes = target_name.encode('utf-8')[:20]
        data.append(len(name_bytes))
        data.extend(name_bytes)

    sock.send(create_packet(10, bytes(data), seq))
    response = sock.recv(2048)
    header, payload = parse_response(response)

    if header and payload:
        result = payload[0] if len(payload) > 0 else 255
        recv_type = payload[1] if len(payload) > 1 else 0

        if len(payload) >= 23:
            sender_name = payload[2:22].rstrip(b'\x00').decode('utf-8', errors='replace')
            recv_msg_len = payload[22]
            recv_msg = payload[23:23+recv_msg_len].decode('utf-8', errors='replace') if recv_msg_len > 0 else ""
            # Sanitize for output
            sender_name = ''.join(c if ord(c) < 128 else '?' for c in sender_name)
            recv_msg = ''.join(c if ord(c) < 128 else '?' for c in recv_msg)
            return result, recv_type, sender_name, recv_msg
        return result, recv_type, None, None
    return None, None, None, None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_chat.py <host:port> [account_id]")
        sys.exit(1)

    addr = sys.argv[1]
    host, port = addr.split(':') if ':' in addr else (addr, 20011)
    port = int(port)
    account_id = int(sys.argv[2]) if len(sys.argv) > 2 else 1

    print(f"DNF Chat System Test Client")
    print(f"Target: {host}:{port}")
    print("=" * 50)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect((host, port))
    print("[OK] Connected")

    try:
        seq = login_flow(sock, account_id)
        time.sleep(0.2)

        # Test Normal Chat
        print("\n=== Normal/Area Chat ===")
        result, rtype, sender, msg = send_chat(sock, seq, CHAT_TYPE_NORMAL, "Hello area!")
        seq += 1
        print(f"  Result: {result}")
        if sender:
            print(f"  From: {sender}")
            print(f"  Message: {msg}")
        time.sleep(0.1)

        # Test World Chat
        print("\n=== World Chat ===")
        result, rtype, sender, msg = send_chat(sock, seq, CHAT_TYPE_WORLD, "Hello world!")
        seq += 1
        print(f"  Result: {result}")
        if sender:
            print(f"  From: {sender}")
            print(f"  Message: {msg}")
        time.sleep(0.1)

        # Test Guild Chat (no guild - should fail)
        print("\n=== Guild Chat (no guild) ===")
        result, rtype, sender, msg = send_chat(sock, seq, CHAT_TYPE_GUILD, "Hello guild!")
        seq += 1
        print(f"  Result: {result} ({'Success' if result == 0 else 'Failed (no guild)'})")
        time.sleep(0.1)

        # Test Party Chat (no party - should fail)
        print("\n=== Party Chat (no party) ===")
        result, rtype, sender, msg = send_chat(sock, seq, CHAT_TYPE_PARTY, "Hello party!")
        seq += 1
        print(f"  Result: {result} ({'Success' if result == 0 else 'Failed (no party)'})")
        time.sleep(0.1)

        # Test Whisper (target not online)
        print("\n=== Whisper (offline target) ===")
        result, rtype, sender, msg = send_chat(sock, seq, CHAT_TYPE_WHISPER, "Hello there!", "SomeOfflineUser")
        seq += 1
        print(f"  Result: {result} ({'Success' if result == 0 else 'Failed (user not found)'})")

        print("\n" + "=" * 50)
        print("Chat system test completed!")
        print("=" * 50)

    finally:
        sock.close()

if __name__ == '__main__':
    main()
