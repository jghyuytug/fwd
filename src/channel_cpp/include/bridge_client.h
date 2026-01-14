/*
 * bridge_client.h - Bridge 服务器连接客户端
 *
 * Channel 服务作为客户端连接到 Bridge 服务器
 * 用于服务注册和状态同步
 */

#ifndef __BRIDGE_CLIENT_H__
#define __BRIDGE_CLIENT_H__

#include "../../bridge_cpp/include/tcp_socket.h"
#include "../../bridge_cpp/include/thread_lock.h"
#include "../../bridge_cpp/include/packet_header.h"
#include <pthread.h>

namespace ChannelServiceApp {

/*
 * 连接状态
 */
enum BridgeConnState {
    CONN_DISCONNECTED = 0,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_REGISTERED
};

/*
 * BridgeClient - Bridge 连接客户端
 */
class BridgeClient {
public:
    BridgeClient();
    ~BridgeClient();

    /*
     * 连接配置
     */
    void setServerAddress(const char* ip, int port);
    const char* getServerIp() const { return m_serverIp; }
    int getServerPort() const { return m_serverPort; }

    /*
     * Channel 信息
     */
    void setChannelId(int id) { m_channelId = id; }
    int getChannelId() const { return m_channelId; }

    void setChannelIp(const char* ip);
    const char* getChannelIp() const { return m_channelIp; }

    void setChannelPort(int port) { m_channelPort = port; }
    int getChannelPort() const { return m_channelPort; }

    /*
     * 连接管理
     */
    bool connect();
    void disconnect();
    bool reconnect();
    bool isConnected() const { return m_state == CONN_CONNECTED || m_state == CONN_REGISTERED; }
    BridgeConnState getState() const { return m_state; }

    /*
     * 数据发送
     */
    int send(const void* data, int len);
    int sendPacket(uint8_t msgNo, const void* data, int dataLen);

    /*
     * 数据接收
     */
    int recv(void* buffer, int bufLen);
    int processReceivedData();

    /*
     * 协议处理
     */
    bool sendRegister();          /* 发送注册请求 */
    bool sendHeartbeat();         /* 发送心跳 */
    bool sendUserCount(int count); /* 发送用户数量 */

    /*
     * 重连线程
     */
    bool startReconnectThread();
    void stopReconnectThread();

protected:
    static void* reconnectLoop(void* arg);
    void onConnected();
    void onDisconnected();
    int handlePacket(tagPacketHeader* header, const uint8_t* data, int dataLen);

private:
    /* Bridge 服务器地址 */
    char m_serverIp[64];
    int m_serverPort;

    /* 本 Channel 信息 */
    int m_channelId;
    char m_channelIp[64];
    int m_channelPort;

    /* 连接 */
    TCPSocket m_socket;
    BridgeConnState m_state;

    /* 接收缓冲区 */
    uint8_t m_recvBuffer[65536];
    int m_recvLen;

    /* 线程 */
    pthread_t m_reconnectThread;
    bool m_reconnectRunning;

    /* 同步 */
    LinuxLock m_lock;

    /* 配置 */
    int m_reconnectInterval;  /* 重连间隔 (秒) */
    int m_heartbeatInterval;  /* 心跳间隔 (秒) */
    uint64_t m_lastHeartbeat; /* 上次心跳时间 */
    uint64_t m_lastRecv;      /* 上次接收时间 */
};

} /* namespace ChannelServiceApp */

#endif /* __BRIDGE_CLIENT_H__ */
