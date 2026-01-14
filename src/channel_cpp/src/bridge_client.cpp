/*
 * bridge_client.cpp - Bridge 服务器连接客户端实现
 */

#include "../include/bridge_client.h"
#include "../../bridge_cpp/include/file_log.h"
#include "../../bridge_cpp/include/crypto_checksum.h"
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

namespace ChannelServiceApp {

BridgeClient::BridgeClient()
    : m_serverPort(7000)
    , m_channelId(0)
    , m_channelPort(7001)
    , m_state(CONN_DISCONNECTED)
    , m_recvLen(0)
    , m_reconnectThread(0)
    , m_reconnectRunning(false)
    , m_reconnectInterval(5)
    , m_heartbeatInterval(30)
    , m_lastHeartbeat(0)
    , m_lastRecv(0)
{
    m_serverIp[0] = '\0';
    m_channelIp[0] = '\0';
    strcpy(m_serverIp, "127.0.0.1");
}

BridgeClient::~BridgeClient() {
    stopReconnectThread();
    disconnect();
}

void BridgeClient::setServerAddress(const char* ip, int port) {
    if (ip) {
        strncpy(m_serverIp, ip, sizeof(m_serverIp) - 1);
        m_serverIp[sizeof(m_serverIp) - 1] = '\0';
    }
    m_serverPort = port;
}

void BridgeClient::setChannelIp(const char* ip) {
    if (ip) {
        strncpy(m_channelIp, ip, sizeof(m_channelIp) - 1);
        m_channelIp[sizeof(m_channelIp) - 1] = '\0';
    }
}

bool BridgeClient::connect() {
    ScopedLinuxLock guard(m_lock);

    if (m_state != CONN_DISCONNECTED) {
        return m_state == CONN_CONNECTED || m_state == CONN_REGISTERED;
    }

    LOG_INFO("BridgeClient: Connecting to %s:%d", m_serverIp, m_serverPort);

    m_state = CONN_CONNECTING;

    /* 打开套接字 */
    if (!m_socket.open()) {
        LOG_ERROR("BridgeClient: Failed to open socket");
        m_state = CONN_DISCONNECTED;
        return false;
    }

    /* 设置选项 */
    m_socket.setOptNonBlock();
    m_socket.setOptNoDelay(true);
    m_socket.setOptKeepAlive(true);

    /* 连接 */
    if (!m_socket.connect(m_serverIp, m_serverPort)) {
        /* 非阻塞连接可能返回 false 但实际在进行中 */
        int err = errno;
        if (err != EINPROGRESS && err != EALREADY) {
            LOG_ERROR("BridgeClient: Connect failed: %s", strerror(err));
            m_socket.close();
            m_state = CONN_DISCONNECTED;
            return false;
        }

        /* 等待连接完成 */
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(m_socket.getSocket(), &wfds);

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(m_socket.getSocket() + 1, NULL, &wfds, NULL, &tv);
        if (ret <= 0) {
            LOG_ERROR("BridgeClient: Connect timeout");
            m_socket.close();
            m_state = CONN_DISCONNECTED;
            return false;
        }

        /* 检查连接结果 */
        int error = 0;
        socklen_t len = sizeof(error);
        getsockopt(m_socket.getSocket(), SOL_SOCKET, SO_ERROR, &error, &len);
        if (error != 0) {
            LOG_ERROR("BridgeClient: Connect error: %s", strerror(error));
            m_socket.close();
            m_state = CONN_DISCONNECTED;
            return false;
        }
    }

    m_state = CONN_CONNECTED;
    m_recvLen = 0;

    /* 更新时间戳 */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_lastRecv = tv.tv_sec;
    m_lastHeartbeat = tv.tv_sec;

    LOG_INFO("BridgeClient: Connected to Bridge");

    onConnected();

    return true;
}

void BridgeClient::disconnect() {
    ScopedLinuxLock guard(m_lock);

    if (m_state == CONN_DISCONNECTED) {
        return;
    }

    LOG_INFO("BridgeClient: Disconnecting");

    m_socket.close();
    m_state = CONN_DISCONNECTED;
    m_recvLen = 0;

    onDisconnected();
}

bool BridgeClient::reconnect() {
    disconnect();
    return connect();
}

int BridgeClient::send(const void* data, int len) {
    if (!isConnected()) {
        return -1;
    }

    ScopedLinuxLock guard(m_lock);
    return m_socket.send(reinterpret_cast<const char*>(data), len);
}

int BridgeClient::sendPacket(uint8_t msgNo, const void* data, int dataLen) {
    if (!isConnected()) {
        return -1;
    }

    /* 构建数据包 */
    uint8_t buffer[65536];
    tagPacketHeader header;

    header.classification = 0;
    header.msg_no = msgNo;
    header.reserved = 0;
    header.sequence = 0;
    header.length = PACKET_HEADER_SIZE + dataLen;
    header.checksum = 0;  /* TODO: 计算校验和 */

    /* 序列化头部 */
    header.serialize(buffer, sizeof(buffer));

    /* 复制数据 */
    if (data && dataLen > 0) {
        memcpy(buffer + PACKET_HEADER_SIZE, data, dataLen);
    }

    /* CRC32: classification + length + msg_no + reserved + payload */
    ChecksumCRC32 crc32;
    crc32.init();
    crc32.update(buffer, 7);
    if (dataLen > 0) {
        crc32.update(buffer + PACKET_HEADER_SIZE, dataLen);
    }
    header.checksum = crc32.getValue();
    header.serialize(buffer, sizeof(buffer));

    return send(buffer, PACKET_HEADER_SIZE + dataLen);
}

int BridgeClient::recv(void* buffer, int bufLen) {
    if (!isConnected()) {
        return -1;
    }

    ScopedLinuxLock guard(m_lock);
    return m_socket.recv(reinterpret_cast<char*>(buffer), bufLen);
}

int BridgeClient::processReceivedData() {
    if (!isConnected()) {
        return -1;
    }

    /* 接收数据 */
    int spaceLeft = sizeof(m_recvBuffer) - m_recvLen;
    if (spaceLeft <= 0) {
        LOG_WARN("BridgeClient: Receive buffer full");
        return -1;
    }

    int received = recv(m_recvBuffer + m_recvLen, spaceLeft);
    if (received < 0) {
        int err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK) {
            return 0;  /* 没有数据 */
        }
        LOG_ERROR("BridgeClient: Recv error: %s", strerror(err));
        disconnect();
        return -1;
    } else if (received == 0) {
        LOG_INFO("BridgeClient: Connection closed by server");
        disconnect();
        return -1;
    }

    m_recvLen += received;

    /* 更新接收时间 */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_lastRecv = tv.tv_sec;

    /* 处理完整的数据包 */
    int processed = 0;
    while (m_recvLen >= PACKET_HEADER_SIZE) {
        tagPacketHeader header;
        if (!header.parse(m_recvBuffer, m_recvLen)) {
            /* 无效数据，丢弃一个字节 */
            memmove(m_recvBuffer, m_recvBuffer + 1, m_recvLen - 1);
            m_recvLen--;
            continue;
        }

        int packetLen = header.getSize();
        if (m_recvLen < packetLen) {
            break;  /* 数据不完整 */
        }

        /* 处理数据包 */
        handlePacket(&header, m_recvBuffer + PACKET_HEADER_SIZE,
                    packetLen - PACKET_HEADER_SIZE);

        /* 移除已处理的数据 */
        memmove(m_recvBuffer, m_recvBuffer + packetLen, m_recvLen - packetLen);
        m_recvLen -= packetLen;
        processed++;
    }

    return processed;
}

int BridgeClient::handlePacket(tagPacketHeader* header, const uint8_t* data, int dataLen) {
    if (!header) {
        return -1;
    }

    uint8_t msgNo = header->msg_no;

    LOG_INFO("BridgeClient: Received packet msg_no=%d len=%d", msgNo, dataLen);

    /* TODO: 根据 msg_no 处理不同的消息 */
    switch (msgNo) {
        case 0:  /* 注册响应 */
            LOG_INFO("BridgeClient: Register response received");
            m_state = CONN_REGISTERED;
            break;

        case 1:  /* 心跳响应 */
            LOG_INFO("BridgeClient: Heartbeat response received");
            break;

        case 2:  /* 频道列表更新 */
            LOG_INFO("BridgeClient: Channel list update received");
            break;

        default:
            LOG_WARN("BridgeClient: Unknown message: %d", msgNo);
            break;
    }

    return 0;
}

bool BridgeClient::sendRegister() {
    if (!isConnected()) {
        return false;
    }

    LOG_INFO("BridgeClient: Sending register request");

    /* 构建注册数据包 */
    /* 格式: channel_id(4) + channel_ip(64) + channel_port(4) */
    uint8_t data[128];
    memset(data, 0, sizeof(data));

    int offset = 0;

    /* Channel ID */
    uint32_t id = htonl(m_channelId);
    memcpy(data + offset, &id, 4);
    offset += 4;

    /* Channel IP */
    strncpy(reinterpret_cast<char*>(data + offset), m_channelIp, 63);
    offset += 64;

    /* Channel Port */
    uint32_t port = htonl(m_channelPort);
    memcpy(data + offset, &port, 4);
    offset += 4;

    return sendPacket(0, data, offset) > 0;  /* msg_no = 0: 注册 */
}

bool BridgeClient::sendHeartbeat() {
    if (!isConnected()) {
        return false;
    }

    /* 更新心跳时间 */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_lastHeartbeat = tv.tv_sec;

    return sendPacket(1, NULL, 0) > 0;  /* msg_no = 1: 心跳 */
}

bool BridgeClient::sendUserCount(int count) {
    if (!isConnected()) {
        return false;
    }

    /* 构建用户数量数据包 */
    uint32_t netCount = htonl(count);

    return sendPacket(2, &netCount, 4) > 0;  /* msg_no = 2: 用户数量 */
}

bool BridgeClient::startReconnectThread() {
    if (m_reconnectRunning) {
        return true;
    }

    m_reconnectRunning = true;
    int ret = pthread_create(&m_reconnectThread, NULL, reconnectLoop, this);
    if (ret != 0) {
        LOG_ERROR("BridgeClient: Failed to create reconnect thread: %d", ret);
        m_reconnectRunning = false;
        return false;
    }

    LOG_INFO("BridgeClient: Reconnect thread started");
    return true;
}

void BridgeClient::stopReconnectThread() {
    if (!m_reconnectRunning) {
        return;
    }

    m_reconnectRunning = false;

    if (m_reconnectThread) {
        pthread_join(m_reconnectThread, NULL);
        m_reconnectThread = 0;
    }

    LOG_INFO("BridgeClient: Reconnect thread stopped");
}

void* BridgeClient::reconnectLoop(void* arg) {
    BridgeClient* self = static_cast<BridgeClient*>(arg);

    LOG_INFO("BridgeClient::reconnectLoop() entering");

    while (self->m_reconnectRunning) {
        /* 检查连接状态 */
        if (!self->isConnected()) {
            LOG_INFO("BridgeClient: Attempting to reconnect...");
            if (self->connect()) {
                LOG_INFO("BridgeClient: Reconnected successfully");
                self->sendRegister();
            } else {
                LOG_WARN("BridgeClient: Reconnect failed, will retry in %d seconds",
                        self->m_reconnectInterval);
            }
        }

        /* 等待 */
        sleep(self->m_reconnectInterval);
    }

    LOG_INFO("BridgeClient::reconnectLoop() exiting");
    return NULL;
}

void BridgeClient::onConnected() {
    LOG_INFO("BridgeClient: onConnected callback");
}

void BridgeClient::onDisconnected() {
    LOG_INFO("BridgeClient: onDisconnected callback");
}

} /* namespace ChannelServiceApp */
