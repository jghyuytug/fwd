/*
 * packet_dump.h - 原始包落盘/Hex Dump 功能
 *
 * 用于抓取和分析真实客户端通信协议
 *
 * 使用方法:
 *   1. 编译时定义 PACKET_DUMP_ENABLED
 *   2. 或运行时创建 ./packet_dump_enabled 文件
 *   3. 包数据会写入 ./packet_dumps/ 目录
 */

#ifndef PACKET_DUMP_H
#define PACKET_DUMP_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

/* 配置 */
#define PACKET_DUMP_DIR      "./packet_dumps"
#define PACKET_DUMP_MAX_SIZE 65536
#define PACKET_DUMP_ENABLED_FILE "./packet_dump_enabled"

/* 全局状态（每个可执行文件各自一份；定义在 src/packet_dump.cpp） */
extern FILE* g_packet_dump_file;
extern int g_packet_dump_enabled;
extern uint32_t g_packet_dump_count;

/*
 * packet_dump_init - 初始化包落盘功能
 *
 * 检查是否启用，创建输出目录和文件
 */
static inline void packet_dump_init(void) {
    struct stat st;

    /* 检查是否启用 */
#ifdef PACKET_DUMP_ENABLED
    g_packet_dump_enabled = 1;
#else
    if (stat(PACKET_DUMP_ENABLED_FILE, &st) == 0) {
        g_packet_dump_enabled = 1;
    }
#endif

    if (!g_packet_dump_enabled) {
        printf("[DUMP] Packet dump disabled. Create '%s' to enable.\n",
               PACKET_DUMP_ENABLED_FILE);
        return;
    }

    /* 创建目录 */
    mkdir(PACKET_DUMP_DIR, 0755);

    /* 创建输出文件 (按时间戳命名) */
    char filename[256];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    snprintf(filename, sizeof(filename), "%s/packets_%04d%02d%02d_%02d%02d%02d.log",
             PACKET_DUMP_DIR,
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    g_packet_dump_file = fopen(filename, "w");
    if (g_packet_dump_file) {
        printf("[DUMP] Packet dump enabled: %s\n", filename);
        fprintf(g_packet_dump_file, "# Packet Dump - %s\n", filename);
        fprintf(g_packet_dump_file, "# Format: [timestamp] [direction] [length] [hex...]\n\n");
        fflush(g_packet_dump_file);
    } else {
        printf("[DUMP] Failed to create dump file: %s\n", filename);
        g_packet_dump_enabled = 0;
    }
}

/*
 * packet_dump_close - 关闭包落盘
 */
static inline void packet_dump_close(void) {
    if (g_packet_dump_file) {
        fclose(g_packet_dump_file);
        g_packet_dump_file = NULL;
    }
}

/*
 * packet_dump_hexline - 输出一行 hex dump
 */
static inline void packet_dump_hexline(FILE* f, const uint8_t* data, int len, int offset) {
    int i;
    char hex[50] = {0};
    char ascii[18] = {0};
    int hex_pos = 0;

    for (i = 0; i < 16 && (offset + i) < len; i++) {
        uint8_t b = data[offset + i];
        hex_pos += sprintf(hex + hex_pos, "%02X ", b);
        ascii[i] = (b >= 32 && b < 127) ? b : '.';
    }
    ascii[i] = '\0';

    fprintf(f, "  %04X  %-48s  %s\n", offset, hex, ascii);
}

/*
 * packet_dump_write - 写入包数据
 *
 * @direction: 方向 ("C->S" 客户端到服务器, "S->C" 服务器到客户端)
 * @data: 原始数据
 * @len: 数据长度
 * @fd: 连接文件描述符 (用于区分不同客户端)
 */
static inline void packet_dump_write(const char* direction, const uint8_t* data,
                                     int len, int fd) {
    if (!g_packet_dump_enabled || !g_packet_dump_file || !data || len <= 0) {
        return;
    }

    if (len > PACKET_DUMP_MAX_SIZE) {
        len = PACKET_DUMP_MAX_SIZE;
    }

    g_packet_dump_count++;

    /* 时间戳 */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm* tm_info = localtime(&ts.tv_sec);

    fprintf(g_packet_dump_file,
            "========== Packet #%u ==========\n"
            "Time:      %02d:%02d:%02d.%03ld\n"
            "Direction: %s\n"
            "FD:        %d\n"
            "Length:    %d bytes\n"
            "---\n",
            g_packet_dump_count,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, ts.tv_nsec / 1000000,
            direction, fd, len);

    /* Header 解析 (多种格式尝试) */
    fprintf(g_packet_dump_file, "--- Header Analysis ---\n");

    /* tagPacketHeader (6-byte, 正确格式) */
    if (len >= 6) {
        uint8_t category = data[0];
        uint8_t packet_id = data[1];
        uint32_t size;
        memcpy(&size, data + 2, 4);

        fprintf(g_packet_dump_file,
                "[tagPacketHeader 6-byte] category=%d(0x%02X) packet_id=%d(0x%02X) size=%u%s\n",
                category, category, packet_id, packet_id, size,
                (size == (uint32_t)len) ? " <-- SIZE MATCH!" : "");
    }

    /* PacketHeader (10-byte, 服务器内部) */
    if (len >= 10) {
        uint16_t pkt_id;
        uint16_t size;
        memcpy(&pkt_id, data, 2);
        memcpy(&size, data + 2, 2);

        fprintf(g_packet_dump_file,
                "[PacketHeader 10-byte]   packet_id=%d(0x%04X) size=%u%s\n",
                pkt_id, pkt_id, size,
                (size == (uint16_t)len) ? " <-- SIZE MATCH!" : "");
    }

    /* DNF 13-byte */
    if (len >= 13) {
        uint8_t classification = data[0];
        uint32_t length;
        uint8_t msg_no = data[5];
        uint8_t padding = data[6];
        uint32_t checksum;
        uint16_t sequence;
        memcpy(&length, data + 1, 4);
        memcpy(&checksum, data + 7, 4);
        memcpy(&sequence, data + 11, 2);

        fprintf(g_packet_dump_file,
                "[DNF 13-byte]            class=%d(0x%02X) len=%u msg_no=%d(0x%02X) pad=%u crc32=0x%08X seq=%u%s\n",
                classification, classification,
                length,
                msg_no, msg_no,
                padding,
                checksum,
                sequence,
                (length == (uint32_t)len) ? " <-- SIZE MATCH!" : "");

        /* CRC32 校验（仅在拿到完整包时验证） */
        if (length >= 13 && length <= (uint32_t)len) {
            /* header(0..6) + payload(13..length-1) */
            static uint32_t s_crc32_table[256];
            static int s_crc32_table_init = 0;

            if (!s_crc32_table_init) {
                for (uint32_t i = 0; i < 256; i++) {
                    uint32_t crc = i;
                    for (int j = 0; j < 8; j++) {
                        crc = (crc & 1u) ? ((crc >> 1) ^ 0xEDB88320u) : (crc >> 1);
                    }
                    s_crc32_table[i] = crc;
                }
                s_crc32_table_init = 1;
            }

            uint32_t crc = 0xFFFFFFFFu;
            for (int i = 0; i < 7; i++) {
                crc = (crc >> 8) ^ s_crc32_table[(crc ^ data[i]) & 0xFFu];
            }

            int payload_len = (int)length - 13;
            const uint8_t* payload = data + 13;
            for (int i = 0; i < payload_len; i++) {
                crc = (crc >> 8) ^ s_crc32_table[(crc ^ payload[i]) & 0xFFu];
            }
            crc ^= 0xFFFFFFFFu;

            fprintf(g_packet_dump_file,
                    "  [CRC32 verify] stored=0x%08X calc=0x%08X %s\n",
                    checksum, crc, (checksum == crc) ? "OK" : "MISMATCH");
        }
    }

    /* Hex Dump */
    fprintf(g_packet_dump_file, "--- Hex Dump ---\n");
    int offset;
    for (offset = 0; offset < len; offset += 16) {
        packet_dump_hexline(g_packet_dump_file, data, len, offset);
    }

    fprintf(g_packet_dump_file, "\n");
    fflush(g_packet_dump_file);
}

/*
 * packet_dump_raw_recv - 记录原始接收数据
 *
 * 在 recv() 后立即调用
 */
#define PACKET_DUMP_RECV(data, len, fd) \
    packet_dump_write("C->S", (const uint8_t*)(data), (len), (fd))

/*
 * packet_dump_raw_send - 记录原始发送数据
 *
 * 在 send() 前调用
 */
#define PACKET_DUMP_SEND(data, len, fd) \
    packet_dump_write("S->C", (const uint8_t*)(data), (len), (fd))

#endif /* PACKET_DUMP_H */
