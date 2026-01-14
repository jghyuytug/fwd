/*
 * protocol.c - Bridge 协议处理
 *
 * 解析 13 字节协议头并路由数据包
 *
 * 协议头结构 (基于配置):
 *   [0:1]  classification - 分类
 *   [1:4]  length - 数据包长度
 *   [5:1]  msg_no - 消息号 (低5位)
 *   [6:1]  reserved
 *   [7:4]  checksum - 校验和
 *   [11:2] sequence - 序列号
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "bridge.h"

/*
 * bridge_parse_header - 解析协议头
 *
 * @data: 原始数据
 * @len: 数据长度
 * @header: 输出的协议头结构
 *
 * 返回: 0成功, -1失败
 */
int bridge_parse_header(const uint8_t* data, int len, packet_header_t* header)
{
    if (!data || !header || len < PACKET_HEADER_SIZE) {
        return -1;
    }

    /* 按字节解析，处理字节序 */
    header->classification = data[0];

    /* length 在偏移1，4字节，小端序 */
    header->length = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);

    /* msg_no 在偏移5，1字节 */
    header->msg_no = data[5] & 0x1F;  /* 只取低5位 */

    /* reserved 在偏移6 */
    header->reserved = data[6];

    /* checksum 在偏移7，4字节，小端序 */
    header->checksum = data[7] | (data[8] << 8) | (data[9] << 16) | (data[10] << 24);

    /* sequence 在偏移11，2字节，小端序 */
    header->sequence = data[11] | (data[12] << 8);

    return 0;
}

/*
 * bridge_validate_header - 验证协议头
 *
 * @header: 协议头
 *
 * 返回: 0有效, -1无效
 */
int bridge_validate_header(const packet_header_t* header)
{
    if (!header) {
        return -1;
    }

    /* 检查长度合理性 */
    if (header->length < PACKET_HEADER_SIZE || header->length > MAX_PACKET_SIZE) {
        bridge_log(LOG_WARN, "protocol: invalid packet length: %u", header->length);
        return -1;
    }

    /* TODO: 验证校验和 */

    return 0;
}

/*
 * bridge_calc_checksum - 计算校验和
 *
 * 简单的加法校验和
 */
uint32_t bridge_calc_checksum(const uint8_t* data, int len)
{
    uint32_t sum = 0;

    for (int i = 0; i < len; i++) {
        sum += data[i];
    }

    return sum;
}

/*
 * bridge_determine_route - 确定路由目标
 *
 * 根据 classification 和 msg_no 决定路由目标
 */
route_target_t bridge_determine_route(const packet_header_t* header)
{
    if (!header) {
        return ROUTE_TARGET_NONE;
    }

    /*
     * 路由规则 (需要根据实际协议调整):
     * - classification 0: 通用/Bridge内部
     * - classification 1: Channel 相关
     * - classification 2+: Game 相关
     */
    switch (header->classification) {
        case 0:
            /* Bridge 内部处理 */
            return ROUTE_TARGET_BRIDGE_INTERNAL;

        case 1:
            /* Channel 相关 */
            return ROUTE_TARGET_CHANNEL;

        default:
            /* Game 相关 */
            return ROUTE_TARGET_GAME;
    }
}

/*
 * bridge_route_to_channel - 路由到 Channel 服务
 */
int bridge_route_to_channel(bridge_server_t* server, client_conn_t* client,
                            const uint8_t* data, int len)
{
    /* TODO: 实现与 Channel 的通信 */
    bridge_log(LOG_DEBUG, "protocol: routing to channel, len=%d", len);

    /*
     * 基本流程:
     * 1. 建立/复用到 Channel 的连接
     * 2. 添加客户端标识
     * 3. 转发数据包
     * 4. 等待响应
     * 5. 将响应发回客户端
     */

    return 0;
}

/*
 * bridge_route_to_game - 路由到 Game 服务
 */
int bridge_route_to_game(bridge_server_t* server, client_conn_t* client,
                         const uint8_t* data, int len)
{
    /* TODO: 实现与 Game 的通信 */
    bridge_log(LOG_DEBUG, "protocol: routing to game, len=%d", len);

    return 0;
}

/*
 * bridge_handle_internal - 处理 Bridge 内部消息
 *
 * 处理心跳、版本查询等内部消息
 */
int bridge_handle_internal(bridge_server_t* server, client_conn_t* client,
                          const packet_header_t* header, const uint8_t* data, int len)
{
    bridge_log(LOG_DEBUG, "protocol: internal message, msg_no=%d", header->msg_no);

    /*
     * 内部消息类型:
     * - 0: 心跳
     * - 1: 版本查询
     * - 2: 状态查询
     */
    switch (header->msg_no) {
        case 0:
            /* 心跳 - 直接回复 */
            return bridge_send_tcp(client, data, len);

        case 1:
            /* 版本查询 */
            {
                uint8_t response[32];
                int resp_len = 0;

                /* 构建版本响应 */
                response[0] = 0;  /* classification */
                response[1] = 32; /* length */
                response[2] = 0;
                response[3] = 0;
                response[4] = 0;
                response[5] = 1;  /* msg_no = 1 (版本响应) */
                response[6] = 0;  /* reserved */
                /* checksum 和 sequence 略 */

                /* 版本数据 */
                response[13] = BRIDGE_VERSION_MAJOR;
                response[14] = BRIDGE_VERSION_MINOR;
                response[15] = BRIDGE_VERSION_PATCH;

                resp_len = 32;
                return bridge_send_tcp(client, response, resp_len);
            }

        default:
            bridge_log(LOG_WARN, "protocol: unknown internal msg_no: %d", header->msg_no);
            break;
    }

    return 0;
}

/*
 * bridge_process_packet - 处理完整数据包
 *
 * @server: Bridge服务器
 * @client: 客户端连接
 * @data: 完整数据包
 * @len: 数据包长度
 */
int bridge_process_packet(bridge_server_t* server, client_conn_t* client,
                          const uint8_t* data, int len)
{
    packet_header_t header;
    route_target_t target;

    /* 解析协议头 */
    if (bridge_parse_header(data, len, &header) < 0) {
        bridge_log(LOG_WARN, "protocol: failed to parse header");
        return -1;
    }

    /* 验证协议头 */
    if (bridge_validate_header(&header) < 0) {
        bridge_log(LOG_WARN, "protocol: invalid header");
        return -1;
    }

    bridge_log(LOG_DEBUG, "protocol: packet class=%d msg=%d len=%u seq=%u",
              header.classification, header.msg_no, header.length, header.sequence);

    server->total_packets++;

    /* 确定路由目标 */
    target = bridge_determine_route(&header);

    switch (target) {
        case ROUTE_TARGET_BRIDGE_INTERNAL:
            return bridge_handle_internal(server, client, &header, data, len);

        case ROUTE_TARGET_CHANNEL:
            return bridge_route_to_channel(server, client, data, len);

        case ROUTE_TARGET_GAME:
            return bridge_route_to_game(server, client, data, len);

        default:
            bridge_log(LOG_WARN, "protocol: unknown route target");
            return -1;
    }
}

/*
 * bridge_process_client_data - 处理客户端数据
 *
 * 从接收缓冲区中提取完整数据包并处理
 */
int bridge_process_client_data(bridge_server_t* server, client_conn_t* client)
{
    int processed = 0;

    while (client->recv_len >= PACKET_HEADER_SIZE) {
        packet_header_t header;

        /* 预览协议头获取长度 */
        if (bridge_parse_header(client->recv_buffer, client->recv_len, &header) < 0) {
            bridge_log(LOG_WARN, "protocol: failed to parse header preview");
            /* 丢弃一个字节，尝试重新同步 */
            memmove(client->recv_buffer, client->recv_buffer + 1, client->recv_len - 1);
            client->recv_len--;
            continue;
        }

        /* 检查是否收到完整数据包 */
        if (client->recv_len < (int)header.length) {
            /* 数据不完整，等待更多数据 */
            break;
        }

        /* 处理完整数据包 */
        if (bridge_process_packet(server, client, client->recv_buffer, header.length) < 0) {
            bridge_log(LOG_WARN, "protocol: failed to process packet");
        }

        /* 从缓冲区移除已处理的数据 */
        processed += header.length;
        client->recv_len -= header.length;
        if (client->recv_len > 0) {
            memmove(client->recv_buffer, client->recv_buffer + header.length, client->recv_len);
        }
    }

    return processed;
}
