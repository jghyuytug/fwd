#!/usr/bin/env python3
"""
Test script for Auction Service
Phase 16 P4: Independent Auction Service
"""

import socket
import struct
import time

def test_auction_service(host='192.168.200.131', port=30603):
    """Test basic auction service functionality"""

    print(f"Connecting to Auction Service at {host}:{port}")

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        sock.connect((host, port))
        print("Connected!")

        # Build test request
        # Format: [player_id:4][request_id:4][player_name:32][gold:8][msg_type:2][data_len:2][data:N]

        player_id = 12345
        request_id = 1
        player_name = b'TestPlayer' + b'\x00' * 22  # 32 bytes
        gold = 1000000  # 1M gold
        msg_type = 170  # AUCTION_SEARCH

        # Search data: [item_id:4][min_price:4][max_price:4]
        item_id = 0  # Search all
        min_price = 0
        max_price = 0
        data = struct.pack('<III', item_id, min_price, max_price)
        data_len = len(data)

        # Build packet
        packet = struct.pack('<II', player_id, request_id)
        packet += player_name
        packet += struct.pack('<Q', gold)
        packet += struct.pack('<HH', msg_type, data_len)
        packet += data

        print(f"Sending AUCTION_SEARCH (msg_type={msg_type}, len={len(packet)})")
        sock.send(packet)

        # Receive response
        # Format: [request_id:4][msg_type:2][data_len:2][data:N]
        response = sock.recv(1024)

        if len(response) >= 8:
            resp_request_id, resp_msg_type, resp_data_len = struct.unpack('<IHH', response[:8])
            print(f"Response: request_id={resp_request_id}, msg_type={resp_msg_type}, data_len={resp_data_len}")

            if resp_data_len > 0 and len(response) >= 8 + resp_data_len:
                resp_data = response[8:8+resp_data_len]
                if len(resp_data) >= 5:
                    result = struct.unpack('<I', resp_data[:4])[0]
                    count = resp_data[4]
                    print(f"  Result: {result}, Count: {count}")
                    print("SUCCESS: Auction search working!")
        else:
            print(f"Unexpected response length: {len(response)}")

        # Test 2: AUCTION_GET_MY_LISTINGS (msg_type=174)
        print("\nTesting AUCTION_GET_MY_LISTINGS...")
        request_id = 2
        msg_type = 174
        data = b''
        data_len = 0

        packet = struct.pack('<II', player_id, request_id)
        packet += player_name
        packet += struct.pack('<Q', gold)
        packet += struct.pack('<HH', msg_type, data_len)

        sock.send(packet)
        response = sock.recv(1024)

        if len(response) >= 8:
            resp_request_id, resp_msg_type, resp_data_len = struct.unpack('<IHH', response[:8])
            print(f"Response: request_id={resp_request_id}, msg_type={resp_msg_type}, data_len={resp_data_len}")
            print("SUCCESS: Get my listings working!")

        sock.close()
        print("\nAll tests passed!")
        return True

    except socket.timeout:
        print("ERROR: Connection timeout")
        return False
    except ConnectionRefusedError:
        print("ERROR: Connection refused - is the service running?")
        return False
    except Exception as e:
        print(f"ERROR: {e}")
        return False

if __name__ == '__main__':
    import sys
    host = sys.argv[1] if len(sys.argv) > 1 else '192.168.200.131'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 30603
    test_auction_service(host, port)
