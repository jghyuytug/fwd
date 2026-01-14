/*
 * tcp_user.cpp - TCP 用户连接实现
 */

#include "../include/tcp_user.h"
#include "../include/file_log.h"
#include "../include/packet_dump.h"
#include <cstdio>

namespace ChannelServiceApp {

TCPUser::TCPUser()
    : m_socket(NULL)
    , m_state(USER_STATE_DISCONNECTED)
    , m_userId(0)
    , m_sequence(0)
    , m_connectTime(0)
    , m_lastAccessTime(0)
    , m_recvLen(0)
    , m_sendLen(0)
    , m_sendPos(0)
{
    memset(m_recvBuffer, 0, sizeof(m_recvBuffer));
    memset(m_sendBuffer, 0, sizeof(m_sendBuffer));
}

TCPUser::~TCPUser() {
    shutdown();
}

/*
 * onAccept - 连接接受事件
 */
void TCPUser::onAccept() {
    m_connectTime = time(NULL);
    m_lastAccessTime = m_connectTime;
    m_state = USER_STATE_CONNECTED;

    LOG_INFO("User accepted: id=%u addr=%s:%u",
            m_userId, getPeerAddress(), getPeerPort());
}

/*
 * onClose - 连接关闭事件
 */
void TCPUser::onClose(char* reason, int code) {
    LOG_INFO("User closed: id=%u reason=%s code=%d",
            m_userId, reason ? reason : "unknown", code);

    shutdown();
}

/*
 * onError - 连接错误事件
 */
void TCPUser::onError() {
    LOG_WARN("User error: id=%u", m_userId);
    shutdown();
}

/*
 * onRead - 读取数据事件 (外部缓冲区)
 */
void TCPUser::onRead(char* buf, int len) {
    if (!buf || len <= 0) {
        return;
    }

    ScopedLinuxLock guard(m_lock);

    /* 检查缓冲区空间 */
    if (m_recvLen + len > RECV_BUFFER_SIZE) {
        LOG_WARN("User recv buffer overflow: id=%u", m_userId);
        return;
    }

    /* 复制数据到接收缓冲区 */
    memcpy(m_recvBuffer + m_recvLen, buf, len);
    m_recvLen += len;

    setLastAccessTime();
}

/*
 * onRead_ - 读取数据事件 (从套接字读取)
 */
void TCPUser::onRead_() {
    if (!m_socket || !m_socket->isValid()) {
        return;
    }

    ScopedLinuxLock guard(m_lock);

    /* 计算可用空间 */
    int space = RECV_BUFFER_SIZE - m_recvLen;
    if (space <= 0) {
        LOG_WARN("User recv buffer full: id=%u", m_userId);
        return;
    }

    /* 从套接字读取数据 */
    int n = m_socket->recv(reinterpret_cast<char*>(m_recvBuffer + m_recvLen), space);
    if (n > 0) {
        /* 原始包落盘 - 记录刚收到的数据 */
        PACKET_DUMP_RECV(m_recvBuffer + m_recvLen, n, m_socket->getSocket());

        m_recvLen += n;
        setLastAccessTime();
    }
    else if (n == 0) {
        /* 连接关闭 */
        postDisconnected(0);
    }
    else {
        /* 错误 */
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            onError();
        }
    }
}

/*
 * onWrite - 写入数据事件 (外部缓冲区)
 */
void TCPUser::onWrite(char* buf, int len) {
    if (!buf || len <= 0 || !m_socket) {
        return;
    }

    m_socket->send(buf, len);
}

/*
 * onWrite_ - 写入数据事件 (从发送缓冲区)
 */
void TCPUser::onWrite_() {
    if (!m_socket || !m_socket->isValid()) {
        return;
    }

    ScopedLinuxLock guard(m_lock);

    /* 检查是否有待发送数据 */
    int remaining = m_sendLen - m_sendPos;
    if (remaining <= 0) {
        return;
    }

    /* 发送数据 */
    int n = m_socket->send(reinterpret_cast<char*>(m_sendBuffer + m_sendPos), remaining);
    if (n > 0) {
        m_sendPos += n;

        /* 如果全部发送完成，重置缓冲区 */
        if (m_sendPos >= m_sendLen) {
            m_sendLen = 0;
            m_sendPos = 0;
        }
    }
    else if (n < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            onError();
        }
    }
}

/*
 * onWrite2Buffer - 写入数据到发送缓冲区
 */
int TCPUser::onWrite2Buffer(CMsgCell* msg) {
    if (!msg) {
        return -1;
    }

    ScopedLinuxLock guard(m_lock);

    int len = msg->GetSize();
    if (m_sendLen + len > SEND_BUFFER_SIZE) {
        LOG_WARN("User send buffer overflow: id=%u", m_userId);
        return -1;
    }

    memcpy(m_sendBuffer + m_sendLen, msg->GetBuf(), len);
    m_sendLen += len;

    return len;
}

/*
 * send - 发送消息
 */
int TCPUser::send(CMsgCell* msg) {
    if (!msg || !m_socket || !m_socket->isValid()) {
        return -1;
    }

    /* 先尝试直接发送 */
    int len = msg->GetSize();
    PACKET_DUMP_SEND(msg->GetBuf(), len, m_socket->getSocket());
    int n = m_socket->send(reinterpret_cast<const char*>(msg->GetBuf()), len);

    if (n == len) {
        return n;
    }

    /* 部分发送或失败，加入发送缓冲区 */
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            n = 0;
        } else {
            return -1;
        }
    }

    /* 将剩余数据加入发送缓冲区 */
    ScopedLinuxLock guard(m_lock);
    int remaining = len - n;
    if (m_sendLen + remaining > SEND_BUFFER_SIZE) {
        return -1;
    }

    memcpy(m_sendBuffer + m_sendLen, msg->GetBuf() + n, remaining);
    m_sendLen += remaining;

    return len;
}

/*
 * sendRaw - 发送原始数据
 */
int TCPUser::sendRaw(const uint8_t* data, int len) {
    if (!data || len <= 0 || !m_socket || !m_socket->isValid()) {
        return -1;
    }

    PACKET_DUMP_SEND(data, len, m_socket->getSocket());
    return m_socket->send(reinterpret_cast<const char*>(data), len);
}

/*
 * setSocket - 设置套接字
 */
void TCPUser::setSocket(TCPSocket* socket) {
    m_socket = socket;
}

/*
 * startupAfterSetSocket - 设置套接字后的初始化
 */
void TCPUser::startupAfterSetSocket() {
    if (m_socket) {
        m_socket->setOptNonBlock();
    }
}

/*
 * shutdown - 关闭连接
 */
void TCPUser::shutdown() {
    if (m_socket) {
        m_socket->close();
    }
    m_state = USER_STATE_DISCONNECTED;
}

/*
 * postDisconnected - 标记断开连接
 */
void TCPUser::postDisconnected(int reason) {
    LOG_INFO("User disconnected: id=%u reason=%d", m_userId, reason);
    m_state = USER_STATE_DISCONNECTED;
}

/*
 * setLastAccessTime - 更新最后访问时间
 */
void TCPUser::setLastAccessTime() {
    m_lastAccessTime = time(NULL);
}

/*
 * consumeRecvBuffer - 消费接收缓冲区数据
 */
void TCPUser::consumeRecvBuffer(int len) {
    if (len <= 0 || len > m_recvLen) {
        return;
    }

    ScopedLinuxLock guard(m_lock);

    m_recvLen -= len;
    if (m_recvLen > 0) {
        memmove(m_recvBuffer, m_recvBuffer + len, m_recvLen);
    }
}

/*
 * reset - 重置用户状态 (用于内存池复用)
 */
void TCPUser::reset() {
    if (m_socket) {
        m_socket->close();
        m_socket = NULL;
    }

    m_state = USER_STATE_DISCONNECTED;
    m_userId = 0;
    m_sequence = 0;
    m_connectTime = 0;
    m_lastAccessTime = 0;
    m_recvLen = 0;
    m_sendLen = 0;
    m_sendPos = 0;
}

/*
 * getPeerAddress - 获取对端地址
 */
const char* TCPUser::getPeerAddress() {
    if (m_socket) {
        return m_socket->getPeerAdrs();
    }
    return "0.0.0.0";
}

/*
 * getPeerPort - 获取对端端口
 */
uint16_t TCPUser::getPeerPort() {
    if (m_socket) {
        return m_socket->getPeerPort();
    }
    return 0;
}

} /* namespace ChannelServiceApp */
