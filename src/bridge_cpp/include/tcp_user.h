/*
 * tcp_user.h - TCP 用户连接
 *
 * 基于生产版的 TCPUser 设计:
 * - 用户连接状态管理
 * - 读写事件处理
 * - 收发缓冲区管理
 */

#ifndef __TCP_USER_H__
#define __TCP_USER_H__

#include <stdint.h>
#include <time.h>
#include <cstring>
#include "tcp_socket.h"
#include "packet_header.h"
#include "msg_cell.h"
#include "thread_lock.h"

namespace ChannelServiceApp {

/* 缓冲区大小 */
const int RECV_BUFFER_SIZE = 65536;
const int SEND_BUFFER_SIZE = 65536;

/* 用户状态 */
enum UserState {
    USER_STATE_DISCONNECTED = 0,
    USER_STATE_CONNECTING,
    USER_STATE_CONNECTED,
    USER_STATE_AUTHENTICATED,
    USER_STATE_IN_CHANNEL,
    USER_STATE_IN_GAME
};

/*
 * TCPUserStates - 用户状态统计
 */
class TCPUserStates {
public:
    TCPUserStates()
        : totalConnections(0)
        , totalDisconnections(0)
        , totalPacketsRecv(0)
        , totalPacketsSent(0)
        , totalBytesRecv(0)
        , totalBytesSent(0)
    {}

    uint64_t totalConnections;
    uint64_t totalDisconnections;
    uint64_t totalPacketsRecv;
    uint64_t totalPacketsSent;
    uint64_t totalBytesRecv;
    uint64_t totalBytesSent;
};

/*
 * TCPUser - TCP 用户连接类
 */
class TCPUser {
public:
    TCPUser();
    virtual ~TCPUser();

    /*
     * 事件处理 - 由 Reactor 调用
     */
    void onAccept();
    void onClose(char* reason, int code);
    void onError();
    void onRead(char* buf, int len);
    void onRead_();
    void onWrite(char* buf, int len);
    void onWrite_();
    int onWrite2Buffer(CMsgCell* msg);

    /*
     * 数据发送
     */
    int send(CMsgCell* msg);
    int sendRaw(const uint8_t* data, int len);

    /*
     * 套接字管理
     */
    void setSocket(TCPSocket* socket);
    TCPSocket* getSocket() { return m_socket; }
    int getHandle() { return m_socket ? m_socket->getFd() : -1; }

    /*
     * 启动/关闭
     */
    void startupAfterSetSocket();
    void shutdown();
    void postDisconnected(int reason);

    /*
     * 时间管理
     */
    void setLastAccessTime();
    time_t getLastAccessTime() const { return m_lastAccessTime; }
    time_t getConnectTime() const { return m_connectTime; }

    /*
     * 状态管理
     */
    void setState(UserState state) { m_state = state; }
    UserState getState() const { return m_state; }
    bool isConnected() const { return m_state >= USER_STATE_CONNECTED; }

    /*
     * ID 管理
     */
    void setUserId(uint32_t id) { m_userId = id; }
    uint32_t getUserId() const { return m_userId; }

    /*
     * 序列号管理
     */
    uint16_t getNextSequence() { return ++m_sequence; }
    uint16_t getCurrentSequence() const { return m_sequence; }

    /*
     * 缓冲区访问
     */
    uint8_t* getRecvBuffer() { return m_recvBuffer; }
    int getRecvLen() const { return m_recvLen; }
    void setRecvLen(int len) { m_recvLen = len; }
    void consumeRecvBuffer(int len);

    /*
     * 重置 (用于内存池复用)
     */
    void reset();

    /*
     * 对端信息
     */
    const char* getPeerAddress();
    uint16_t getPeerPort();

protected:
    TCPSocket* m_socket;            /* 套接字 */
    UserState m_state;              /* 连接状态 */
    uint32_t m_userId;              /* 用户 ID */
    uint16_t m_sequence;            /* 序列号 */

    time_t m_connectTime;           /* 连接时间 */
    time_t m_lastAccessTime;        /* 最后活动时间 */

    uint8_t m_recvBuffer[RECV_BUFFER_SIZE];  /* 接收缓冲区 */
    int m_recvLen;                           /* 接收数据长度 */

    uint8_t m_sendBuffer[SEND_BUFFER_SIZE];  /* 发送缓冲区 */
    int m_sendLen;                           /* 待发送数据长度 */
    int m_sendPos;                           /* 已发送位置 */

    LinuxLock m_lock;               /* 线程锁 */
};

} /* namespace ChannelServiceApp */

#endif /* __TCP_USER_H__ */
