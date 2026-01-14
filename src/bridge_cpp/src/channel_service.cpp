/*
 * channel_service.cpp - Channel 服务主类实现
 */

#include "../include/channel_service.h"
#include "../include/server_group.h"
#include "../include/crypto_checksum.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace ChannelServiceApp {

static void FillPacketCRC32(tagPacketHeader* header, uint8_t* packet, int totalLen) {
    if (!header || !packet || totalLen < PACKET_HEADER_SIZE) {
        return;
    }

    int payloadLen = totalLen - PACKET_HEADER_SIZE;

    ChecksumCRC32 crc32;
    crc32.init();
    crc32.update(packet, 7); /* classification + length + msg_no + reserved */
    if (payloadLen > 0) {
        crc32.update(packet + PACKET_HEADER_SIZE, payloadLen);
    }

    header->checksum = crc32.getValue();
    header->serialize(packet, totalLen);
}

ChannelService::ChannelService()
    : m_mode(MODE_NORMAL)
    , m_tick(0)
    , m_userCount(0)
    , m_maxUsers(1000)
{
    ChannelServiceManager::setManager(this);
}

ChannelService::ChannelService(char* configName)
    : m_mode(MODE_NORMAL)
    , m_tick(0)
    , m_userCount(0)
    , m_maxUsers(1000)
{
    ChannelServiceManager::setManager(this);

    /* 设置配置文件路径 */
    char configPath[256];
    char pidPath[256];
    snprintf(configPath, sizeof(configPath), "cfg/%s.cfg", configName);
    snprintf(pidPath, sizeof(pidPath), "pid/%s.pid", configName);

    m_serviceInfo.setInfo(configName, configName, configPath, pidPath);
}

ChannelService::~ChannelService() {
    shutdown();
    ChannelServiceManager::setManager(NULL);
}

/*
 * onStart - 服务启动
 */
bool ChannelService::onStart() {
    LOG_INFO("ChannelService::onStart()");

    return startup();
}

/*
 * onStop - 服务停止
 */
void ChannelService::onStop() {
    LOG_INFO("ChannelService::onStop()");

    shutdown();
}

/*
 * run - 主循环
 */
void ChannelService::run() {
    LOG_INFO("ChannelService::run() entering main loop");

    while (m_running) {
        /* 处理新接受的连接 */
        processAcceptedUsers();

        /* 处理 epoll 事件 */
        int nfds = m_reactor.handleEvents(100);  /* 100ms 超时 */

        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            LOG_ERROR("epoll_wait failed: %s", strerror(errno));
            break;
        }

        if (nfds > 0) {
            processUserEvents(nfds);
        }

        /* 更新 tick */
        setTick();
    }

    LOG_INFO("ChannelService::run() exiting main loop");
}

/*
 * readConfig - 读取配置文件
 */
bool ChannelService::readConfig() {
    const char* configFile = m_serviceInfo.getConfigFileName();
    LOG_INFO("Reading config: %s", configFile);

    FILE* fp = fopen(configFile, "r");
    if (!fp) {
        LOG_ERROR("Cannot open config file: %s", configFile);
        return false;
    }

    char line[256];
    char section[64] = "";

    while (fgets(line, sizeof(line), fp)) {
        /* 去除换行 */
        char* p = strchr(line, '\n');
        if (p) *p = '\0';
        p = strchr(line, '\r');
        if (p) *p = '\0';

        /* 跳过空行和注释 */
        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
            continue;
        }

        /* 检查节 */
        if (line[0] == '[') {
            char* end = strchr(line, ']');
            if (end) {
                *end = '\0';
                strncpy(section, line + 1, sizeof(section) - 1);
            }
            continue;
        }

        /* 解析键值对 */
        char* eq = strchr(line, '=');
        if (!eq) {
            continue;
        }

        *eq = '\0';
        char* key = line;
        char* value = eq + 1;

        /* 去除空格 */
        while (*key == ' ' || *key == '\t') key++;
        while (*value == ' ' || *value == '\t') value++;
        char* keyEnd = key + strlen(key) - 1;
        while (keyEnd > key && (*keyEnd == ' ' || *keyEnd == '\t')) {
            *keyEnd-- = '\0';
        }
        char* valueEnd = value + strlen(value) - 1;
        while (valueEnd > value && (*valueEnd == ' ' || *valueEnd == '\t')) {
            *valueEnd-- = '\0';
        }

        /* 根据节和键设置值 */
        if (strcmp(section, "server") == 0) {
            if (strcmp(key, "this_ip") == 0) {
                strncpy(m_config.listenIp, value, sizeof(m_config.listenIp) - 1);
            }
            else if (strcmp(key, "this_tcp_port") == 0) {
                m_config.tcpPort = atoi(value);
            }
            else if (strcmp(key, "this_udp_port") == 0) {
                m_config.udpPort = atoi(value);
            }
            else if (strcmp(key, "max_client") == 0) {
                m_config.maxUsers = atoi(value);
                m_maxUsers = m_config.maxUsers;
            }
        }
        else if (strcmp(section, "DB") == 0) {
            if (strcmp(key, "db_ip") == 0) {
                strncpy(m_config.dbIp, value, sizeof(m_config.dbIp) - 1);
            }
            else if (strcmp(key, "db_name") == 0) {
                strncpy(m_config.dbName, value, sizeof(m_config.dbName) - 1);
            }
            else if (strcmp(key, "db_id") == 0) {
                strncpy(m_config.dbUser, value, sizeof(m_config.dbUser) - 1);
            }
            else if (strcmp(key, "db_pwd") == 0) {
                strncpy(m_config.dbPass, value, sizeof(m_config.dbPass) - 1);
            }
            else if (strcmp(key, "db_port") == 0) {
                m_config.dbPort = atoi(value);
            }
        }
        else if (strcmp(section, "channel") == 0) {
            if (strcmp(key, "channel_ip") == 0) {
                strncpy(m_config.channelIp, value, sizeof(m_config.channelIp) - 1);
            }
            else if (strcmp(key, "channel_port") == 0) {
                m_config.channelPort = atoi(value);
            }
        }
    }

    fclose(fp);

    /* 设置默认值 */
    if (m_config.tcpPort == 0) {
        m_config.tcpPort = 7000;
    }
    if (m_config.udpPort == 0) {
        m_config.udpPort = 7000;
    }
    if (m_config.listenIp[0] == '\0') {
        strcpy(m_config.listenIp, "0.0.0.0");
    }

    LOG_INFO("Config loaded:");
    LOG_INFO("  listen: %s:%d (tcp), %d (udp)",
            m_config.listenIp, m_config.tcpPort, m_config.udpPort);
    LOG_INFO("  max_users: %d", m_config.maxUsers);
    LOG_INFO("  db: %s@%s:%d/%s",
            m_config.dbUser, m_config.dbIp, m_config.dbPort, m_config.dbName);

    return true;
}

/*
 * startup - 启动服务
 */
bool ChannelService::startup() {
    LOG_INFO("ChannelService::startup()");

    /* 初始化套接字系统 */
    TSocketSystem<SocketSystemLinux>::startup();

    /* 初始化用户池 */
    if (!m_userPools.startup()) {
        LOG_ERROR("Failed to startup user pools");
        return false;
    }

    /* 初始化 epoll 反应器 */
    if (!m_reactor.init(256)) {
        LOG_ERROR("Failed to init reactor");
        return false;
    }

    /* 创建监听套接字 */
    if (!m_listenSocket.open()) {
        LOG_ERROR("Failed to open listen socket");
        return false;
    }

    if (!m_listenSocket.bind(m_config.listenIp, m_config.tcpPort, true)) {
        LOG_ERROR("Failed to bind listen socket to %s:%d",
                 m_config.listenIp, m_config.tcpPort);
        return false;
    }

    if (!m_listenSocket.listen(128)) {
        LOG_ERROR("Failed to listen on socket");
        return false;
    }

    m_listenSocket.setOptNonBlock();

    LOG_INFO("Listening on %s:%d", m_config.listenIp, m_config.tcpPort);

    /* 启动接受线程 */
    if (!m_acceptThread.start(&m_listenSocket)) {
        LOG_ERROR("Failed to start accept thread");
        return false;
    }

    /* 注册协议处理器 */
    registerProtocols();
    registerProtocolsExtra();

    /* 设置协议分发器 */
    m_handlerRelay.setHandler(this);

    LOG_INFO("ChannelService started successfully");
    return true;
}

/*
 * shutdown - 关闭服务
 */
void ChannelService::shutdown() {
    LOG_INFO("ChannelService::shutdown()");

    /* 停止接受线程 */
    m_acceptThread.stop();

    /* 关闭监听套接字 */
    m_listenSocket.close();

    /* 关闭反应器 */
    m_reactor.shutdown();

    /* 关闭用户池 */
    m_userPools.shutdown();

    /* 关闭套接字系统 */
    TSocketSystem<SocketSystemLinux>::shutdown();

    /* 打印统计 */
    LOG_INFO("Statistics:");
    LOG_INFO("  Total connections: %lu", m_userStates.totalConnections);
    LOG_INFO("  Total disconnections: %lu", m_userStates.totalDisconnections);
    LOG_INFO("  Total packets recv: %lu", m_userStates.totalPacketsRecv);
    LOG_INFO("  Total packets sent: %lu", m_userStates.totalPacketsSent);
    LOG_INFO("  Total bytes recv: %lu", m_userStates.totalBytesRecv);
    LOG_INFO("  Total bytes sent: %lu", m_userStates.totalBytesSent);
}

/*
 * registerProtocols - 注册协议处理器
 */
void ChannelService::registerProtocols() {
    LOG_INFO("Registering protocols...");

    /* CS_GET_SCRIPT = 0 */
    registerHandler(0, onCS_GET_SCRIPT);

    /* CS_GET_GC_INFO = 1 */
    registerHandler(1, onCS_GET_GC_INFO);

    /* CS_CHECK_SCRIPT_VERSION = 2 */
    registerHandler(2, onCS_CHECK_SCRIPT_VERSION);

    /* CS_NOTICE_CHANNEL_SERVER = 3 */
    registerHandler(3, onCS_NOTICE_CHANNEL_SERVER);
}

void ChannelService::registerProtocolsExtra() {
    LOG_INFO("Registering extra protocols...");

    /* CS_UPDATE_CHANNEL_INFO = 0 */
    registerHandlerExtra(0, onCS_UPDATE_CHANNEL_INFO);
}

/*
 * setTick - 更新 tick
 */
void ChannelService::setTick() {
    m_tick++;
}

/*
 * 协议处理器实现
 */
int ChannelService::onCS_GET_SCRIPT(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_GET_SCRIPT: user=%u", user ? user->getUserId() : 0);

    /* TODO: 实现脚本获取逻辑 */

    return 0;
}

int ChannelService::onCS_GET_GC_INFO(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_GET_GC_INFO: user=%u", user ? user->getUserId() : 0);

    /* TODO: 实现 GC 信息获取逻辑 */

    return 0;
}

int ChannelService::onCS_CHECK_SCRIPT_VERSION(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_CHECK_SCRIPT_VERSION: user=%u", user ? user->getUserId() : 0);

    if (!user) {
        return -1;
    }

    /* 解析请求的脚本版本 */
    const uint8_t* packet = user->getRecvBuffer();
    const uint8_t* data = packet + PACKET_HEADER_SIZE;
    int dataLen = header->length - PACKET_HEADER_SIZE;

    uint32_t clientVersion = 0;
    if (dataLen >= 4) {
        clientVersion = ntohl(*reinterpret_cast<const uint32_t*>(data));
    }

    LOG_INFO("Client script version: %u", clientVersion);

    /* 发送脚本版本响应 */
    /* 响应格式: server_version(4) + need_update(1) */
    uint8_t response[PACKET_HEADER_SIZE + 5];
    tagPacketHeader respHeader;
    respHeader.classification = 0;
    respHeader.msg_no = 2;  /* 脚本版本响应 */
    respHeader.reserved = 0;
    respHeader.sequence = user->getNextSequence();
    respHeader.length = PACKET_HEADER_SIZE + 5;
    respHeader.checksum = 0;
    respHeader.serialize(response, sizeof(response));

    /* 服务器脚本版本 (示例: 1) */
    uint32_t serverVersion = htonl(1);
    memcpy(response + PACKET_HEADER_SIZE, &serverVersion, 4);

    /* 是否需要更新: 0=不需要, 1=需要 */
    response[PACKET_HEADER_SIZE + 4] = 0;

    FillPacketCRC32(&respHeader, response, sizeof(response));
    user->sendRaw(response, sizeof(response));

    LOG_INFO("Sent script version response: server=%u need_update=0", 1);

    return 0;
}

int ChannelService::onCS_NOTICE_CHANNEL_SERVER(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_NOTICE_CHANNEL_SERVER: user=%u", user ? user->getUserId() : 0);

    if (!user) {
        return -1;
    }

    ChannelService* service = getService();
    if (!service) {
        return -1;
    }

    (void)service;  /* 暂时不使用 */

    /* 构建频道列表响应 */
    /* 格式: count(4) + [channel_id(4) + name(32) + ip(16) + port(4) + users(4) + max_users(4)] * count */
    int channelCount = 1;  /* 暂时硬编码1个频道 */
    uint8_t responseData[1024];
    memset(responseData, 0, sizeof(responseData));

    int offset = 0;

    /* 频道数量 */
    uint32_t count = htonl(channelCount);
    memcpy(responseData + offset, &count, 4);
    offset += 4;

    /* 频道 1 信息 */
    uint32_t channelId = htonl(1);
    memcpy(responseData + offset, &channelId, 4);
    offset += 4;

    /* 频道名称 */
    const char* channelName = "Channel 1";
    strncpy(reinterpret_cast<char*>(responseData + offset), channelName, 31);
    offset += 32;

    /* Channel IP */
    const char* channelIp = service->getConfig().channelIp[0] ? service->getConfig().channelIp : "127.0.0.1";
    strncpy(reinterpret_cast<char*>(responseData + offset), channelIp, 15);
    offset += 16;

    /* Channel Port */
    uint32_t channelPort = htonl(service->getConfig().channelPort ? service->getConfig().channelPort : 7001);
    memcpy(responseData + offset, &channelPort, 4);
    offset += 4;

    /* 当前用户数 */
    uint32_t users = htonl(0);
    memcpy(responseData + offset, &users, 4);
    offset += 4;

    /* 最大用户数 */
    uint32_t maxUsers = htonl(1000);
    memcpy(responseData + offset, &maxUsers, 4);
    offset += 4;

    /* 构建响应头 */
    uint8_t response[1024 + PACKET_HEADER_SIZE];
    tagPacketHeader respHeader;
    respHeader.classification = 0;
    respHeader.msg_no = 3;  /* 频道列表响应 */
    respHeader.reserved = 0;
    respHeader.sequence = 0;
    respHeader.length = PACKET_HEADER_SIZE + offset;
    respHeader.checksum = 0;
    respHeader.serialize(response, sizeof(response));

    memcpy(response + PACKET_HEADER_SIZE, responseData, offset);

    FillPacketCRC32(&respHeader, response, PACKET_HEADER_SIZE + offset);
    user->sendRaw(response, PACKET_HEADER_SIZE + offset);

    LOG_INFO("Sent channel list: count=%d", channelCount);

    return 0;
}

int ChannelService::onCS_UPDATE_CHANNEL_INFO(tagPacketHeader* header, const uint8_t* data, int dataLen) {
    LOG_INFO("onCS_UPDATE_CHANNEL_INFO");

    ChannelService* service = getService();
    if (!service) {
        return -1;
    }

    /* 解析 Channel 注册数据 */
    /* 格式: channel_id(4) + channel_ip(64) + channel_port(4) */
    if (dataLen < 72) {
        LOG_ERROR("onCS_UPDATE_CHANNEL_INFO: Invalid data length: %d", dataLen);
        return -1;
    }

    /* 解析字段 */
    uint32_t channelId = ntohl(*reinterpret_cast<const uint32_t*>(data));
    const char* channelIp = reinterpret_cast<const char*>(data + 4);
    uint32_t channelPort = ntohl(*reinterpret_cast<const uint32_t*>(data + 68));

    LOG_INFO("Channel registered: id=%u ip=%s port=%u", channelId, channelIp, channelPort);

    /* 创建并存储 Channel 服务器信息 */
    ChannelServer* server = new ChannelServer();
    server->setId(channelId);
    server->setIp(channelIp);
    server->setPort(static_cast<uint16_t>(channelPort));
    server->setOnline(true);

    /* 添加到服务器组 (需要先移除旧的同 ID 服务器) */
    /* TODO: 使用 ServerGroup 管理 */

    /* 发送注册确认响应 */
    /* 响应格式: result(4) = 0 表示成功 */
    uint8_t response[PACKET_HEADER_SIZE + 4];
    tagPacketHeader respHeader;
    respHeader.classification = 0;
    respHeader.msg_no = 0;  /* 注册响应 */
    respHeader.reserved = 0;
    respHeader.sequence = 0;
    respHeader.length = PACKET_HEADER_SIZE + 4;
    respHeader.checksum = 0;
    respHeader.serialize(response, sizeof(response));

    /* 写入结果: 0 = 成功 */
    uint32_t result = htonl(0);
    memcpy(response + PACKET_HEADER_SIZE, &result, 4);

    /* TODO: 发送响应给发起者 (需要在 dispatch 时传递 user) */
    LOG_INFO("Channel registration confirmed: id=%u", channelId);

    delete server;  /* 临时，后续应存储 */

    return 0;
}

/*
 * createUser - 创建用户
 */
TCPUser* ChannelService::createUser() {
    if (m_userCount >= m_maxUsers) {
        LOG_WARN("Max user count reached: %d", m_maxUsers);
        return NULL;
    }

    TCPUser* user = m_userPools.createTCPUser(__FILE__, __LINE__);
    if (user) {
        m_userCount++;
        m_userStates.totalConnections++;
    }
    return user;
}

/*
 * destroyUser - 销毁用户
 */
void ChannelService::destroyUser(TCPUser* user) {
    if (user) {
        m_userPools.destroyTCPUser(user, __FILE__, __LINE__);
        m_userCount--;
        m_userStates.totalDisconnections++;
    }
}

/*
 * processAcceptedUsers - 处理新接受的连接
 */
void ChannelService::processAcceptedUsers() {
    TCPUser* user;
    while ((user = m_acceptThread.lockPopAcceptedUser()) != NULL) {
        /* 分配用户 ID */
        static uint32_t s_nextUserId = 1;
        user->setUserId(s_nextUserId++);

        /* 注册到 epoll */
        if (!m_reactor.registHandle(user, EVENT_READ | EVENT_ERROR | EVENT_HUP)) {
            LOG_ERROR("Failed to register user to reactor");
            delete user->getSocket();
            delete user;
            continue;
        }

        m_userCount++;
        m_userStates.totalConnections++;

        LOG_INFO("User registered: id=%u addr=%s:%u count=%d",
                user->getUserId(), user->getPeerAddress(),
                user->getPeerPort(), m_userCount);
    }
}

/*
 * processUserEvents - 处理用户事件
 */
void ChannelService::processUserEvents(int nfds) {
    for (int i = 0; i < nfds; i++) {
        TCPUser* user = m_reactor.getEventUser(i);
        unsigned int events = m_reactor.getEventMask(i);

        if (!user) {
            continue;
        }

        /* 错误或挂起 */
        if (events & (EVENT_ERROR | EVENT_HUP)) {
            LOG_INFO("User error/hup: id=%u", user->getUserId());
            m_reactor.unregistHandle(user);
            delete user->getSocket();
            delete user;
            m_userCount--;
            m_userStates.totalDisconnections++;
            continue;
        }

        /* 可读 */
        if (events & EVENT_READ) {
            user->onRead_();

            /* 处理接收到的数据 */
            int processed = processUserData(user);
            if (processed < 0) {
                /* 处理错误，断开连接 */
                LOG_WARN("User data processing error: id=%u", user->getUserId());
                m_reactor.unregistHandle(user);
                delete user->getSocket();
                delete user;
                m_userCount--;
                m_userStates.totalDisconnections++;
            }
        }

        /* 可写 */
        if (events & EVENT_WRITE) {
            user->onWrite_();
        }
    }
}

/*
 * processUserData - 处理用户数据
 */
int ChannelService::processUserData(TCPUser* user) {
    int processed = 0;

    while (user->getRecvLen() >= PACKET_HEADER_SIZE) {
        /* 解析头部获取长度 */
        tagPacketHeader header;
        if (!header.parse(user->getRecvBuffer(), user->getRecvLen())) {
            /* 无法解析，丢弃一个字节重试 */
            user->consumeRecvBuffer(1);
            continue;
        }

        /* 检查是否收到完整数据包 */
        int packetLen = header.getSize();
        if (user->getRecvLen() < packetLen) {
            /* 数据不完整，等待更多数据 */
            break;
        }

        /* 分发数据包 */
        m_handlerRelay.dispatch(user,
                               reinterpret_cast<char*>(user->getRecvBuffer()),
                               packetLen, 0);

        /* 消费已处理的数据 */
        user->consumeRecvBuffer(packetLen);
        processed += packetLen;

        m_userStates.totalPacketsRecv++;
        m_userStates.totalBytesRecv += packetLen;
    }

    return processed;
}

} /* namespace ChannelServiceApp */
