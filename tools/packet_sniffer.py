#!/usr/bin/env python3
"""
packet_sniffer.py - DNF 协议抓包分析工具

用于捕获真实客户端与服务器之间的通信，分析协议格式。

功能:
1. TCP 代理模式: 客户端 -> 本机 -> 服务器
2. 原始包 hex dump 落盘
3. 协议头解析对比 (13字节 vs 10字节)

使用方法:
  python packet_sniffer.py --mode proxy --listen 7000 --target 192.168.200.131:7000
  python packet_sniffer.py --mode analyze --file captured_packets.bin
"""

import socket
import select
import struct
import argparse
import os
import time
from datetime import datetime
from threading import Thread
import binascii


def hexdump(data, prefix=""):
    """格式化 hex dump"""
    result = []
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_str = ' '.join(f'{b:02X}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        result.append(f"{prefix}{i:04X}  {hex_str:<48}  {ascii_str}")
    return '\n'.join(result)


def parse_tagPacketHeader(data):
    """
    解析 tagPacketHeader (6字节) - 客户端<->Bridge/Channel 通信

    结构:
      [0:1] category   - 路由分类
      [1:1] packet_id  - 消息号 (0-255)
      [2:4] size       - 包大小 (小端序)
    """
    if len(data) < 6:
        return None

    category = data[0]
    packet_id = data[1]
    size = struct.unpack('<I', data[2:6])[0]

    return {
        'type': 'tagPacketHeader (6-byte, external)',
        'category': category,
        'packet_id': packet_id,
        'size': size,
        'valid': size >= 6 and size <= 65536,  # 合理性检查
    }


def parse_PacketHeader(data):
    """
    解析 PacketHeader (10字节) - 服务器内部通信

    结构:
      [0:2] packet_id  - 消息号 (0-65535, 小端序)
      [2:2] size       - 包大小 (小端序)
      [4:2] reserved   - 保留字段
      [6:4] unknown    - 未知字段
    """
    if len(data) < 10:
        return None

    packet_id = struct.unpack('<H', data[0:2])[0]
    size = struct.unpack('<H', data[2:4])[0]
    reserved = struct.unpack('<H', data[4:6])[0]
    unknown = struct.unpack('<I', data[6:10])[0]

    return {
        'type': 'PacketHeader (10-byte, internal)',
        'packet_id': packet_id,
        'size': size,
        'reserved': reserved,
        'unknown': unknown,
        'valid': size >= 10 and size <= 65535,
    }


def parse_header_legacy13(data):
    """解析 13 字节协议头 (旧版/错误实现, 供对比)"""
    if len(data) < 13:
        return None

    classification = data[0]
    length = struct.unpack('<I', data[1:5])[0]
    msg_no = data[5] & 0x1F  # 低5位
    msg_no_full = data[5]    # 完整字节
    reserved = data[6]
    checksum = struct.unpack('<I', data[7:11])[0]
    sequence = struct.unpack('<H', data[11:13])[0]

    return {
        'type': 'Legacy 13-byte (incorrect)',
        'classification': classification,
        'length': length,
        'msg_no_5bit': msg_no,
        'msg_no_full': msg_no_full,
        'reserved': reserved,
        'checksum': checksum,
        'sequence': sequence,
    }


class PacketCapture:
    """包捕获和分析器"""

    def __init__(self, output_dir="captured_packets"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.raw_file = os.path.join(output_dir, f"raw_{timestamp}.bin")
        self.log_file = os.path.join(output_dir, f"packets_{timestamp}.log")

        self.packet_count = 0

    def log(self, msg):
        """写日志"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        line = f"[{timestamp}] {msg}"
        print(line)
        with open(self.log_file, 'a', encoding='utf-8') as f:
            f.write(line + '\n')

    def save_raw(self, direction, data):
        """保存原始数据"""
        with open(self.raw_file, 'ab') as f:
            # 格式: [direction:1][timestamp:8][length:4][data:N]
            ts = int(time.time() * 1000)
            dir_byte = 0 if direction == 'C->S' else 1
            f.write(struct.pack('<BQI', dir_byte, ts, len(data)))
            f.write(data)

    def analyze_packet(self, direction, data):
        """分析单个数据包"""
        self.packet_count += 1

        self.log(f"\n{'='*70}")
        self.log(f"Packet #{self.packet_count} [{direction}] Length: {len(data)} bytes")
        self.log(f"{'='*70}")

        # Hex dump
        self.log("Raw data:")
        self.log(hexdump(data, "  "))

        # 尝试多种解析方式
        self.log("\n--- Header Analysis ---")

        # 1. tagPacketHeader (6字节, 正确格式, 外部通信)
        h6 = parse_tagPacketHeader(data)
        if h6:
            valid_mark = "✓" if h6['valid'] else "✗"
            self.log(f"\n[{valid_mark}] tagPacketHeader (6-byte, external):")
            self.log(f"    category:   {h6['category']} (0x{h6['category']:02X})")
            self.log(f"    packet_id:  {h6['packet_id']} (0x{h6['packet_id']:02X})")
            self.log(f"    size:       {h6['size']}")
            if h6['valid'] and h6['size'] == len(data):
                self.log(f"    ** SIZE MATCHES PACKET LENGTH - LIKELY CORRECT **")

        # 2. PacketHeader (10字节, 正确格式, 内部通信)
        h10 = parse_PacketHeader(data)
        if h10:
            valid_mark = "✓" if h10['valid'] else "✗"
            self.log(f"\n[{valid_mark}] PacketHeader (10-byte, internal):")
            self.log(f"    packet_id:  {h10['packet_id']} (0x{h10['packet_id']:04X})")
            self.log(f"    size:       {h10['size']}")
            self.log(f"    reserved:   {h10['reserved']} (0x{h10['reserved']:04X})")
            self.log(f"    unknown:    {h10['unknown']} (0x{h10['unknown']:08X})")
            if h10['valid'] and h10['size'] == len(data):
                self.log(f"    ** SIZE MATCHES PACKET LENGTH - LIKELY CORRECT **")

        # 3. Legacy 13字节 (错误格式, 供对比)
        h13 = parse_header_legacy13(data)
        if h13:
            self.log(f"\n[?] Legacy 13-byte (for comparison):")
            self.log(f"    classification: {h13['classification']} (0x{h13['classification']:02X})")
            self.log(f"    length:         {h13['length']}")
            self.log(f"    msg_no (full):  {h13['msg_no_full']} (0x{h13['msg_no_full']:02X})")
            self.log(f"    checksum:       0x{h13['checksum']:08X}")
            self.log(f"    sequence:       {h13['sequence']}")

        # 4. 自动检测最可能的格式
        self.log("\n--- Auto Detection ---")
        best_match = None

        if h6 and h6['valid'] and h6['size'] == len(data):
            best_match = 'tagPacketHeader'
            self.log(f"Best match: tagPacketHeader (6-byte)")
            self.log(f"  → category={h6['category']}, packet_id={h6['packet_id']}")
        elif h10 and h10['valid'] and h10['size'] == len(data):
            best_match = 'PacketHeader'
            self.log(f"Best match: PacketHeader (10-byte)")
            self.log(f"  → packet_id=0x{h10['packet_id']:04X}")
        else:
            self.log(f"No clear match - packet may be fragmented or unknown format")

        # 保存原始数据
        self.save_raw(direction, data)


class ProxyServer:
    """TCP 代理服务器"""

    def __init__(self, listen_host, listen_port, target_host, target_port, capture):
        self.listen_host = listen_host
        self.listen_port = listen_port
        self.target_host = target_host
        self.target_port = target_port
        self.capture = capture

    def handle_client(self, client_sock, client_addr):
        """处理客户端连接"""
        self.capture.log(f"Client connected: {client_addr}")

        # 连接目标服务器
        try:
            server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_sock.connect((self.target_host, self.target_port))
            self.capture.log(f"Connected to server: {self.target_host}:{self.target_port}")
        except Exception as e:
            self.capture.log(f"Failed to connect to server: {e}")
            client_sock.close()
            return

        # 双向转发
        sockets = [client_sock, server_sock]

        try:
            while True:
                readable, _, _ = select.select(sockets, [], [], 1.0)

                for sock in readable:
                    data = sock.recv(65536)
                    if not data:
                        raise Exception("Connection closed")

                    if sock == client_sock:
                        # 客户端 -> 服务器
                        self.capture.analyze_packet('C->S', data)
                        server_sock.send(data)
                    else:
                        # 服务器 -> 客户端
                        self.capture.analyze_packet('S->C', data)
                        client_sock.send(data)

        except Exception as e:
            self.capture.log(f"Connection ended: {e}")
        finally:
            client_sock.close()
            server_sock.close()

    def run(self):
        """运行代理服务器"""
        server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_sock.bind((self.listen_host, self.listen_port))
        server_sock.listen(5)

        self.capture.log(f"Proxy listening on {self.listen_host}:{self.listen_port}")
        self.capture.log(f"Forwarding to {self.target_host}:{self.target_port}")

        while True:
            client_sock, client_addr = server_sock.accept()
            thread = Thread(target=self.handle_client, args=(client_sock, client_addr))
            thread.daemon = True
            thread.start()


def analyze_file(filename):
    """分析已捕获的数据文件"""
    capture = PacketCapture()

    with open(filename, 'rb') as f:
        while True:
            header = f.read(13)  # dir:1 + ts:8 + len:4
            if len(header) < 13:
                break

            dir_byte, ts, length = struct.unpack('<BQI', header)
            direction = 'C->S' if dir_byte == 0 else 'S->C'

            data = f.read(length)
            if len(data) < length:
                break

            capture.analyze_packet(direction, data)

    print(f"\nTotal packets analyzed: {capture.packet_count}")


def main():
    parser = argparse.ArgumentParser(description='DNF Protocol Packet Sniffer')
    parser.add_argument('--mode', choices=['proxy', 'analyze'], default='proxy',
                       help='Mode: proxy (TCP proxy) or analyze (analyze captured file)')
    parser.add_argument('--listen', default='0.0.0.0:7000',
                       help='Proxy listen address (host:port)')
    parser.add_argument('--target', default='192.168.200.131:7000',
                       help='Target server address (host:port)')
    parser.add_argument('--file', default=None,
                       help='File to analyze (for analyze mode)')
    parser.add_argument('--output', default='captured_packets',
                       help='Output directory for captured packets')

    args = parser.parse_args()

    if args.mode == 'analyze':
        if not args.file:
            print("Error: --file required for analyze mode")
            return
        analyze_file(args.file)
    else:
        listen_host, listen_port = args.listen.split(':')
        target_host, target_port = args.target.split(':')

        capture = PacketCapture(args.output)
        proxy = ProxyServer(
            listen_host, int(listen_port),
            target_host, int(target_port),
            capture
        )

        try:
            proxy.run()
        except KeyboardInterrupt:
            print("\nStopping...")


if __name__ == '__main__':
    main()
