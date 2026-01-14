/*
 * packet_header.h - 数据包头结构
 *
 * 基于生产版的 tagPacketHeader 设计:
 * - 13 字节固定头部
 * - 支持分类、长度、消息号、校验和、序列号
 */

#ifndef __PACKET_HEADER_H__
#define __PACKET_HEADER_H__

#include <stdint.h>
#include <cstring>

namespace ChannelServiceApp {

/* 协议常量 */
const int PACKET_HEADER_SIZE = 13;
const int MAX_PACKET_SIZE = 65536;

/*
 * tagPacketHeader - 数据包头结构
 *
 * 布局 (13字节):
 *   [0:1]  classification - 分类/类型
 *   [1:4]  length - 数据包总长度
 *   [5:1]  msg_no - 消息号 (低5位有效)
 *   [6:1]  reserved - 保留
 *   [7:4]  checksum - 校验和
 *   [11:2] sequence - 序列号
 */
#pragma pack(push, 1)
struct tagPacketHeader {
    uint8_t  classification;    /* [0:1]  分类标识 */
    uint32_t length;            /* [1:4]  数据包长度 */
    uint8_t  msg_no;            /* [5:1]  消息号 (低5位有效) */
    uint8_t  reserved;          /* [6:1]  保留 */
    uint32_t checksum;          /* [7:4]  校验和 */
    uint16_t sequence;          /* [11:2] 序列号 */

    tagPacketHeader() {
        memset(this, 0, sizeof(tagPacketHeader));
    }

    /* Getters */
    int getPacketID() const {
        return static_cast<int>(msg_no);
    }

    int getSize() const {
        return static_cast<int>(length);
    }

    int getCategory() const {
        return static_cast<int>(classification);
    }

    uint16_t getSequence() const {
        return sequence;
    }

    uint32_t getChecksum() const {
        return checksum;
    }

    /* Setters */
    void setPacketID(int id) {
        msg_no = static_cast<uint8_t>(id & 0xFF);
    }

    void setSize(int size) {
        length = static_cast<uint32_t>(size);
    }

    void setCategory(int cat) {
        classification = static_cast<uint8_t>(cat);
    }

    void setSequence(uint16_t seq) {
        sequence = seq;
    }

    void setChecksum(uint32_t sum) {
        checksum = sum;
    }

    /* 辅助方法 */
    bool isVariableLength() const {
        /* 根据分类判断是否可变长度 */
        return true;
    }

    /*
     * parse - 从原始字节解析头部
     * @data: 原始数据
     * @len: 数据长度
     * 返回: true 成功, false 失败
     */
    bool parse(const uint8_t* data, int len) {
        if (!data || len < PACKET_HEADER_SIZE) {
            return false;
        }

        /* 按字节解析，处理字节序 (小端) */
        classification = data[0];

        length = data[1] |
                (static_cast<uint32_t>(data[2]) << 8) |
                (static_cast<uint32_t>(data[3]) << 16) |
                (static_cast<uint32_t>(data[4]) << 24);

        msg_no = data[5];
        reserved = data[6];

        checksum = data[7] |
                  (static_cast<uint32_t>(data[8]) << 8) |
                  (static_cast<uint32_t>(data[9]) << 16) |
                  (static_cast<uint32_t>(data[10]) << 24);

        sequence = data[11] |
                  (static_cast<uint16_t>(data[12]) << 8);

        return true;
    }

    /*
     * serialize - 序列化头部到字节数组
     * @data: 目标缓冲区
     * @len: 缓冲区长度
     * 返回: 写入的字节数，-1 表示失败
     */
    int serialize(uint8_t* data, int len) const {
        if (!data || len < PACKET_HEADER_SIZE) {
            return -1;
        }

        data[0] = classification;

        data[1] = length & 0xFF;
        data[2] = (length >> 8) & 0xFF;
        data[3] = (length >> 16) & 0xFF;
        data[4] = (length >> 24) & 0xFF;

        data[5] = msg_no;
        data[6] = reserved;

        data[7] = checksum & 0xFF;
        data[8] = (checksum >> 8) & 0xFF;
        data[9] = (checksum >> 16) & 0xFF;
        data[10] = (checksum >> 24) & 0xFF;

        data[11] = sequence & 0xFF;
        data[12] = (sequence >> 8) & 0xFF;

        return PACKET_HEADER_SIZE;
    }

    /*
     * validate - 验证头部有效性
     */
    bool validate() const {
        if (length < PACKET_HEADER_SIZE || length > MAX_PACKET_SIZE) {
            return false;
        }
        return true;
    }
};
#pragma pack(pop)

/*
 * 计算简单校验和
 */
inline uint32_t calcChecksum(const uint8_t* data, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

} /* namespace ChannelServiceApp */

#endif /* __PACKET_HEADER_H__ */
