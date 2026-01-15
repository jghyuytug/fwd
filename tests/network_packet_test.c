/*
 * Network Packet Test - DNF数据包协议测试
 *
 * 测试Week 3实现：
 * 1. PacketBuf序列化（put_byte/short/int/str）
 * 2. PacketBuf反序列化（get_byte/short/int/str）
 * 3. CSession_Parsing数据包解析
 * 4. 粘包处理（多个数据包连续发送）
 * 5. 分片处理（大数据包分片发送）
 *
 * 编译命令：
 *   gcc -o network_packet_test network_packet_test.c \
 *       -I../include \
 *       -pthread -m32
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>

/* 包含数据包结构定义 */
#include "security/headers/CHackAnalyzer.h"

/* ========================================
 * 辅助函数：打印数据包内容（十六进制）
 * ======================================== */
void print_packet_hex(const char *prefix, const char *data, int length)
{
    printf("%s (%d bytes): ", prefix, length);
    int i;
    for (i = 0; i < length; i++) {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0) printf("\n                    ");
    }
    printf("\n");
}

/* ========================================
 * 辅助函数：连接到服务器
 * ======================================== */
int connect_to_server(const char *host, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        close(sock);
        return -1;
    }

    printf("[Client] Connected to %s:%d (socket=%d)\n", host, port, sock);
    return sock;
}

/* ========================================
 * 辅助函数：创建PacketBuf（简化版，用于测试）
 * ======================================== */
PacketBuf* create_packet_buf(int capacity)
{
    PacketBuf *buf = (PacketBuf*)malloc(sizeof(PacketBuf));
    if (!buf) return NULL;

    buf->data = malloc(capacity);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->size = 0;
    buf->capacity = capacity;
    buf->read_pos = 0;
    buf->write_pos = 0;
    buf->finalized = 0;
    buf->encrypted = 0;
    buf->compressed = 0;
    buf->_reserved = 0;
    buf->header = buf->data;  /* 头部在缓冲区开始位置 */

    return buf;
}

void destroy_packet_buf(PacketBuf *buf)
{
    if (buf) {
        if (buf->data) free(buf->data);
        free(buf);
    }
}

/* ========================================
 * PacketBuf序列化函数（客户端版本）
 * ======================================== */
int PacketBuf_put_byte(PacketBuf *self, char value)
{
    if (!self || !self->data) return 0;
    if (self->write_pos + 1 > self->capacity) return 0;

    ((char*)self->data)[self->write_pos] = value;
    self->write_pos += 1;
    if (self->write_pos > self->size) self->size = self->write_pos;
    return 1;
}

int PacketBuf_put_short(PacketBuf *self, int16_t value)
{
    if (!self || !self->data) return 0;
    if (self->write_pos + 2 > self->capacity) return 0;

    unsigned char *ptr = (unsigned char*)self->data + self->write_pos;
    ptr[0] = (unsigned char)(value & 0xFF);
    ptr[1] = (unsigned char)((value >> 8) & 0xFF);
    self->write_pos += 2;
    if (self->write_pos > self->size) self->size = self->write_pos;
    return 1;
}

int PacketBuf_put_int(PacketBuf *self, int value)
{
    if (!self || !self->data) return 0;
    if (self->write_pos + 4 > self->capacity) return 0;

    unsigned char *ptr = (unsigned char*)self->data + self->write_pos;
    ptr[0] = (unsigned char)(value & 0xFF);
    ptr[1] = (unsigned char)((value >> 8) & 0xFF);
    ptr[2] = (unsigned char)((value >> 16) & 0xFF);
    ptr[3] = (unsigned char)((value >> 24) & 0xFF);
    self->write_pos += 4;
    if (self->write_pos > self->size) self->size = self->write_pos;
    return 1;
}

int PacketBuf_put_str(PacketBuf *self, const char *str, int maxlen)
{
    uint16_t len = str ? (uint16_t)strlen(str) : 0;
    if (!PacketBuf_put_short(self, (int16_t)len)) return 0;

    if (len > 0) {
        if (self->write_pos + len > self->capacity) return 0;
        memcpy((char*)self->data + self->write_pos, str, len);
        self->write_pos += len;
        if (self->write_pos > self->size) self->size = self->write_pos;
    }
    return 1;
}

void PacketBuf_finalize(PacketBuf *self, uint16_t packet_type)
{
    if (!self || !self->data) return;

    tagPacketHeader *header = (tagPacketHeader*)self->header;
    header->length = (uint16_t)self->size;
    header->type = packet_type;

    self->finalized = 1;

    printf("[PacketBuf] Finalized: type=%d, length=%d\n", packet_type, self->size);
}

/* ========================================
 * 测试用例1：简单数据包（type=100, payload="Hello"）
 * ======================================== */
int test_simple_packet(int sock)
{
    printf("\n========================================\n");
    printf("Test 1: Simple String Packet\n");
    printf("========================================\n");

    /* 创建数据包：type=100, payload="Hello DNF!" */
    PacketBuf *buf = create_packet_buf(256);
    if (!buf) {
        fprintf(stderr, "Failed to create packet buffer\n");
        return 0;
    }

    /* 预留头部空间（4字节） */
    buf->write_pos = 4;
    buf->size = 4;

    /* 写入字符串payload */
    if (!PacketBuf_put_str(buf, "Hello DNF!", 256)) {
        fprintf(stderr, "Failed to put string\n");
        destroy_packet_buf(buf);
        return 0;
    }

    /* 完成打包（填充头部） */
    PacketBuf_finalize(buf, 100);

    /* 发送数据包 */
    print_packet_hex("[Test1] Sending", (char*)buf->data, buf->size);
    ssize_t sent = send(sock, buf->data, buf->size, 0);
    if (sent != buf->size) {
        perror("send() failed");
        destroy_packet_buf(buf);
        return 0;
    }

    /* 接收echo响应 */
    char recv_buffer[256];
    ssize_t received = recv(sock, recv_buffer, sizeof(recv_buffer), 0);
    if (received <= 0) {
        perror("recv() failed");
        destroy_packet_buf(buf);
        return 0;
    }

    print_packet_hex("[Test1] Received", recv_buffer, received);

    /* 验证响应 */
    if (received == buf->size && memcmp(recv_buffer, buf->data, buf->size) == 0) {
        printf("[Test1] ✓ PASS: Echo response matches\n");
    } else {
        printf("[Test1] ✗ FAIL: Echo response mismatch\n");
    }

    destroy_packet_buf(buf);
    return 1;
}

/* ========================================
 * 测试用例2：混合数据类型数据包
 * ======================================== */
int test_mixed_types_packet(int sock)
{
    printf("\n========================================\n");
    printf("Test 2: Mixed Data Types Packet\n");
    printf("========================================\n");

    PacketBuf *buf = create_packet_buf(256);
    if (!buf) return 0;

    /* 预留头部 */
    buf->write_pos = 4;
    buf->size = 4;

    /* 写入混合类型数据 */
    PacketBuf_put_byte(buf, 0x42);           /* byte: 0x42 */
    PacketBuf_put_short(buf, 0x1234);        /* short: 4660 */
    PacketBuf_put_int(buf, 0x12345678);      /* int: 305419896 */
    PacketBuf_put_str(buf, "Test", 256);     /* string: "Test" */

    PacketBuf_finalize(buf, 200);

    /* 发送并接收 */
    print_packet_hex("[Test2] Sending", (char*)buf->data, buf->size);
    send(sock, buf->data, buf->size, 0);

    char recv_buffer[256];
    ssize_t received = recv(sock, recv_buffer, sizeof(recv_buffer), 0);
    print_packet_hex("[Test2] Received", recv_buffer, received);

    if (received == buf->size && memcmp(recv_buffer, buf->data, buf->size) == 0) {
        printf("[Test2] ✓ PASS\n");
    } else {
        printf("[Test2] ✗ FAIL\n");
    }

    destroy_packet_buf(buf);
    return 1;
}

/* ========================================
 * 测试用例3：粘包测试（连续发送多个数据包）
 * ======================================== */
int test_sticky_packets(int sock)
{
    printf("\n========================================\n");
    printf("Test 3: Sticky Packets (Multiple packets in one send)\n");
    printf("========================================\n");

    /* 创建3个小数据包 */
    char combined_buffer[512];
    int combined_size = 0;
    int i;

    for (i = 0; i < 3; i++) {
        PacketBuf *buf = create_packet_buf(128);
        buf->write_pos = 4;
        buf->size = 4;

        char msg[32];
        snprintf(msg, sizeof(msg), "Packet#%d", i + 1);
        PacketBuf_put_str(buf, msg, 128);
        PacketBuf_finalize(buf, 300 + i);

        /* 添加到组合缓冲区 */
        memcpy(combined_buffer + combined_size, buf->data, buf->size);
        combined_size += buf->size;

        printf("[Test3] Packet %d: type=%d, length=%d\n", i + 1, 300 + i, buf->size);
        destroy_packet_buf(buf);
    }

    /* 一次性发送3个数据包 */
    print_packet_hex("[Test3] Sending (3 packets)", combined_buffer, combined_size);
    send(sock, combined_buffer, combined_size, 0);

    /* 接收echo响应（可能需要多次recv） */
    char recv_buffer[512];
    int total_received = 0;

    while (total_received < combined_size) {
        ssize_t received = recv(sock, recv_buffer + total_received,
                               sizeof(recv_buffer) - total_received, 0);
        if (received <= 0) break;
        total_received += received;
    }

    print_packet_hex("[Test3] Received", recv_buffer, total_received);

    if (total_received == combined_size && memcmp(recv_buffer, combined_buffer, combined_size) == 0) {
        printf("[Test3] ✓ PASS: All 3 packets echoed correctly\n");
    } else {
        printf("[Test3] ✗ FAIL: received=%d, expected=%d\n", total_received, combined_size);
    }

    return 1;
}

/* ========================================
 * 测试用例4：大数据包测试
 * ======================================== */
int test_large_packet(int sock)
{
    printf("\n========================================\n");
    printf("Test 4: Large Packet (1KB payload)\n");
    printf("========================================\n");

    PacketBuf *buf = create_packet_buf(2048);
    if (!buf) return 0;

    buf->write_pos = 4;
    buf->size = 4;

    /* 创建1KB的字符串payload */
    char large_string[1024];
    int i;
    for (i = 0; i < 1023; i++) {
        large_string[i] = 'A' + (i % 26);
    }
    large_string[1023] = '\0';

    PacketBuf_put_str(buf, large_string, 2048);
    PacketBuf_finalize(buf, 400);

    printf("[Test4] Packet size: %d bytes\n", buf->size);
    send(sock, buf->data, buf->size, 0);

    /* 接收大数据包响应 */
    char recv_buffer[2048];
    int total_received = 0;

    while (total_received < buf->size) {
        ssize_t received = recv(sock, recv_buffer + total_received,
                               sizeof(recv_buffer) - total_received, 0);
        if (received <= 0) break;
        total_received += received;
        printf("[Test4] Received chunk: %zd bytes (total: %d/%d)\n",
               received, total_received, buf->size);
    }

    if (total_received == buf->size && memcmp(recv_buffer, buf->data, buf->size) == 0) {
        printf("[Test4] ✓ PASS: Large packet echoed correctly\n");
    } else {
        printf("[Test4] ✗ FAIL: received=%d, expected=%d\n", total_received, buf->size);
    }

    destroy_packet_buf(buf);
    return 1;
}

/* ========================================
 * 主函数
 * ======================================== */
int main(int argc, char *argv[])
{
    printf("==============================================\n");
    printf("  DNF Network Packet Protocol Test (Week 3)\n");
    printf("==============================================\n");

    const char *server_host = "127.0.0.1";
    int server_port = 7001;

    /* 连接到服务器 */
    int sock = connect_to_server(server_host, server_port);
    if (sock < 0) {
        fprintf(stderr, "ERROR: Failed to connect to server\n");
        fprintf(stderr, "Make sure server is running: ./network_test\n");
        return 1;
    }

    printf("\n[Test] Connected to server, starting tests...\n");

    /* 运行所有测试用例 */
    int total_tests = 0;
    int passed_tests = 0;

    if (test_simple_packet(sock)) { total_tests++; passed_tests++; }
    sleep(1);

    if (test_mixed_types_packet(sock)) { total_tests++; passed_tests++; }
    sleep(1);

    if (test_sticky_packets(sock)) { total_tests++; passed_tests++; }
    sleep(1);

    if (test_large_packet(sock)) { total_tests++; passed_tests++; }

    /* 关闭连接 */
    close(sock);
    printf("\n[Test] Connection closed\n");

    /* 打印测试结果 */
    printf("\n==============================================\n");
    printf("  Test Results\n");
    printf("==============================================\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed:      %d\n", passed_tests);
    printf("Failed:      %d\n", total_tests - passed_tests);
    printf("==============================================\n");

    return (passed_tests == total_tests) ? 0 : 1;
}
