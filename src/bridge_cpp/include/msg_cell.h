/*
 * msg_cell.h - 消息单元
 *
 * 基于生产版的 CMsgCell 设计:
 * - 消息缓冲区管理
 * - 序列化支持
 * - 流操作符
 */

#ifndef __MSG_CELL_H__
#define __MSG_CELL_H__

#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include "packet_header.h"

namespace ChannelServiceApp {

/* 默认消息缓冲区大小 */
const int DEFAULT_MSG_BUFFER_SIZE = 8192;

/*
 * CMsgCell - 消息单元
 *
 * 用于构建和解析网络消息
 */
class CMsgCell {
public:
    CMsgCell()
        : m_buffer(NULL)
        , m_bufSize(0)
        , m_dataSize(0)
        , m_readPos(0)
        , m_ownBuffer(false)
    {}

    virtual ~CMsgCell() {
        if (m_ownBuffer && m_buffer) {
            ::free(m_buffer);
            m_buffer = NULL;
        }
    }

    /*
     * AttachStream - 附加外部缓冲区
     * @buf: 外部缓冲区
     * @size: 缓冲区大小
     */
    void AttachStream(char* buf, int size) {
        if (m_ownBuffer && m_buffer) {
            ::free(m_buffer);
        }
        m_buffer = reinterpret_cast<uint8_t*>(buf);
        m_bufSize = size;
        m_dataSize = size;
        m_readPos = 0;
        m_ownBuffer = false;
    }

    /*
     * AllocBuffer - 分配内部缓冲区
     * @size: 缓冲区大小
     */
    bool AllocBuffer(int size = DEFAULT_MSG_BUFFER_SIZE) {
        if (m_ownBuffer && m_buffer) {
            ::free(m_buffer);
        }
        m_buffer = reinterpret_cast<uint8_t*>(::malloc(size));
        if (!m_buffer) {
            return false;
        }
        m_bufSize = size;
        m_dataSize = 0;
        m_readPos = 0;
        m_ownBuffer = true;
        return true;
    }

    /*
     * PAD - 填充/对齐
     */
    void PAD() {
        /* 4字节对齐 */
        int padding = (4 - (m_dataSize % 4)) % 4;
        if (m_dataSize + padding <= m_bufSize) {
            memset(m_buffer + m_dataSize, 0, padding);
            m_dataSize += padding;
        }
    }

    /*
     * GetBuf - 获取缓冲区指针
     */
    const uint8_t* GetBuf() const {
        return m_buffer;
    }

    uint8_t* GetBuf() {
        return m_buffer;
    }

    /*
     * GetSize - 获取数据大小
     */
    int GetSize() const {
        return m_dataSize;
    }

    /*
     * SetSize - 设置数据大小
     */
    void SetSize(int size) {
        m_dataSize = size;
    }

    /*
     * GetBufSize - 获取缓冲区容量
     */
    int GetBufSize() const {
        return m_bufSize;
    }

    /*
     * Clear - 清除数据
     */
    void Clear() {
        m_dataSize = 0;
        m_readPos = 0;
    }

    /*
     * 写入数据包头
     */
    CMsgCell& operator<<(tagPacketHeader* header) {
        if (header && m_buffer && m_dataSize + PACKET_HEADER_SIZE <= m_bufSize) {
            header->serialize(m_buffer + m_dataSize, m_bufSize - m_dataSize);
            m_dataSize += PACKET_HEADER_SIZE;
        }
        return *this;
    }

    /*
     * 写入原始数据
     */
    bool WriteData(const void* data, int len) {
        if (!data || len <= 0) {
            return false;
        }
        if (m_dataSize + len > m_bufSize) {
            return false;
        }
        memcpy(m_buffer + m_dataSize, data, len);
        m_dataSize += len;
        return true;
    }

    /*
     * 读取原始数据
     */
    bool ReadData(void* data, int len) {
        if (!data || len <= 0) {
            return false;
        }
        if (m_readPos + len > m_dataSize) {
            return false;
        }
        memcpy(data, m_buffer + m_readPos, len);
        m_readPos += len;
        return true;
    }

    /* 基本类型写入 */
    CMsgCell& operator<<(uint8_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator<<(uint16_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator<<(uint32_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator<<(int8_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator<<(int16_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator<<(int32_t val) {
        WriteData(&val, sizeof(val));
        return *this;
    }

    /* 基本类型读取 */
    CMsgCell& operator>>(uint8_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator>>(uint16_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator>>(uint32_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator>>(int8_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator>>(int16_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

    CMsgCell& operator>>(int32_t& val) {
        ReadData(&val, sizeof(val));
        return *this;
    }

protected:
    uint8_t* m_buffer;      /* 缓冲区 */
    int m_bufSize;          /* 缓冲区容量 */
    int m_dataSize;         /* 数据大小 */
    int m_readPos;          /* 读取位置 */
    bool m_ownBuffer;       /* 是否拥有缓冲区 */
};

/*
 * TSerializer - 序列化模板
 */
template<typename T>
class TSerializer {
public:
    TSerializer(const T& data) : m_data(data) {}

    const T& getData() const { return m_data; }

private:
    const T& m_data;
};

} /* namespace ChannelServiceApp */

#endif /* __MSG_CELL_H__ */
