/*
 * channel_select_service.cpp - Channel 选择服务主类实现
 */

#include "../include/channel_select_service.h"
#include "../../bridge_cpp/include/file_log.h"
#include "../../bridge_cpp/include/crypto_checksum.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

namespace ChannelServiceApp {

static void FillPacketCRC32(uint8_t* packet, int totalLen) {
    if (!packet || totalLen < PACKET_HEADER_SIZE) {
        return;
    }

    int payloadLen = totalLen - PACKET_HEADER_SIZE;

    ChecksumCRC32 crc32;
    crc32.init();
    crc32.update(packet, 7); /* classification + length + msg_no + reserved */
    if (payloadLen > 0) {
        crc32.update(packet + PACKET_HEADER_SIZE, payloadLen);
    }

    *reinterpret_cast<uint32_t*>(packet + 7) = crc32.getValue();
}

/*
 * GameServer 实现
 */
GameServer::GameServer()
    : m_id(0)
    , m_port(0)
    , m_online(false)
    , m_userCount(0)
    , m_maxUsers(1000)
{
    m_ip[0] = '\0';
}

GameServer::~GameServer() {
}

void GameServer::setIp(const char* ip) {
    if (ip) {
        strncpy(m_ip, ip, sizeof(m_ip) - 1);
        m_ip[sizeof(m_ip) - 1] = '\0';
    }
}

/*
 * ChannelInfo 实现
 */
ChannelInfo::ChannelInfo()
    : m_id(0)
    , m_gameServer(NULL)
    , m_userCount(0)
    , m_maxUsers(500)
    , m_status(0)
{
    m_name[0] = '\0';
}

ChannelInfo::~ChannelInfo() {
}

void ChannelInfo::setName(const char* name) {
    if (name) {
        strncpy(m_name, name, sizeof(m_name) - 1);
        m_name[sizeof(m_name) - 1] = '\0';
    }
}

/*
 * ChannelSelectService 实现
 */
ChannelSelectService::ChannelSelectService()
    : m_mode(CHANNEL_MODE_NORMAL)
    , m_tick(0)
    , m_lastTickTime(0)
    , m_userCount(0)
    , m_maxUsers(1000)
{
    ChannelSelectServiceManager::setManager(this);
}

ChannelSelectService::ChannelSelectService(char* configName)
    : m_mode(CHANNEL_MODE_NORMAL)
    , m_tick(0)
    , m_lastTickTime(0)
    , m_userCount(0)
    , m_maxUsers(1000)
{
    ChannelSelectServiceManager::setManager(this);

    /* 设置配置文件路径 */
    char configPath[256];
    char pidPath[256];
    snprintf(configPath, sizeof(configPath), "cfg/%s.cfg", configName);
    snprintf(pidPath, sizeof(pidPath), "pid/%s.pid", configName);

    m_serviceInfo.setInfo(configName, configName, configPath, pidPath);
}

ChannelSelectService::~ChannelSelectService() {
    shutdown();
    ChannelSelectServiceManager::setManager(NULL);
}

/*
 * onStart - 服务启动
 */
bool ChannelSelectService::onStart() {
    LOG_INFO("ChannelSelectService::onStart()");

    return startup();
}

/*
 * onStop - 服务停止
 */
void ChannelSelectService::onStop() {
    LOG_INFO("ChannelSelectService::onStop()");

    shutdown();
}

/*
 * run - 主循环
 */
void ChannelSelectService::run() {
    LOG_INFO("ChannelSelectService::run() entering main loop");

    /* 获取初始时间 */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_lastTickTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    while (m_running) {
        /* 处理新接受的连接 */
        processAcceptedUsers();

        /* 处理 Bridge 数据 */
        processBridgeData();

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

        /* 定时任务 */
        onTick();
    }

    LOG_INFO("ChannelSelectService::run() exiting main loop");
}

/*
 * readConfig - 读取配置文件
 */
bool ChannelSelectService::readConfig() {
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
            else if (strcmp(key, "id") == 0) {
                m_config.channelId = atoi(value);
            }
            else if (strcmp(key, "bridge_ip") == 0) {
                strncpy(m_config.bridgeIp, value, sizeof(m_config.bridgeIp) - 1);
            }
            else if (strcmp(key, "bridge_port") == 0) {
                m_config.bridgePort = atoi(value);
            }
        }
        else if (strcmp(section, "game") == 0) {
            if (strcmp(key, "game_ip") == 0) {
                strncpy(m_config.gameIp, value, sizeof(m_config.gameIp) - 1);
            }
            else if (strcmp(key, "game_port") == 0) {
                m_config.gamePort = atoi(value);
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
    }

    fclose(fp);

    /* 设置默认值 */
    if (m_config.tcpPort == 0) {
        m_config.tcpPort = 7001;
    }
    if (m_config.udpPort == 0) {
        m_config.udpPort = 7001;
    }
    if (m_config.listenIp[0] == '\0') {
        strcpy(m_config.listenIp, "0.0.0.0");
    }
    if (m_config.bridgeIp[0] == '\0') {
        strcpy(m_config.bridgeIp, "127.0.0.1");
    }
    if (m_config.bridgePort == 0) {
        m_config.bridgePort = 7000;
    }
    if (m_config.gameIp[0] == '\0') {
        strcpy(m_config.gameIp, "127.0.0.1");
    }
    if (m_config.gamePort == 0) {
        m_config.gamePort = 10011;
    }

    LOG_INFO("Config loaded:");
    LOG_INFO("  listen: %s:%d (tcp), %d (udp)",
            m_config.listenIp, m_config.tcpPort, m_config.udpPort);
    LOG_INFO("  max_users: %d", m_config.maxUsers);
    LOG_INFO("  channel_id: %d", m_config.channelId);
    LOG_INFO("  bridge: %s:%d", m_config.bridgeIp, m_config.bridgePort);
    LOG_INFO("  game: %s:%d", m_config.gameIp, m_config.gamePort);
    LOG_INFO("  db: %s@%s:%d/%s",
            m_config.dbUser, m_config.dbIp, m_config.dbPort, m_config.dbName);

    return true;
}

/*
 * startup - 启动服务
 */
bool ChannelSelectService::startup() {
    LOG_INFO("ChannelSelectService::startup()");

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

    /* 设置协议分发器 */
    m_handlerRelay.setHandler(this);

    /* 配置并连接 Bridge */
    m_bridgeClient.setServerAddress(m_config.bridgeIp, m_config.bridgePort);
    m_bridgeClient.setChannelId(m_config.channelId);
    m_bridgeClient.setChannelIp(m_config.listenIp);
    m_bridgeClient.setChannelPort(m_config.tcpPort);

    if (!m_bridgeClient.connect()) {
        LOG_WARN("Failed to connect to Bridge at %s:%d (will retry)",
                m_config.bridgeIp, m_config.bridgePort);
    } else {
        LOG_INFO("Connected to Bridge at %s:%d",
                m_config.bridgeIp, m_config.bridgePort);

        /* 发送注册 */
        m_bridgeClient.sendRegister();
    }

    /* 启动重连线程 */
    m_bridgeClient.startReconnectThread();

    LOG_INFO("ChannelSelectService started successfully");
    return true;
}

/*
 * shutdown - 关闭服务
 */
void ChannelSelectService::shutdown() {
    LOG_INFO("ChannelSelectService::shutdown()");

    /* 停止 Bridge 重连线程 */
    m_bridgeClient.stopReconnectThread();

    /* 断开 Bridge 连接 */
    m_bridgeClient.disconnect();

    /* 停止接受线程 */
    m_acceptThread.stop();

    /* 关闭监听套接字 */
    m_listenSocket.close();

    /* 关闭反应器 */
    m_reactor.shutdown();

    /* 关闭用户池 */
    m_userPools.shutdown();

    /* 清理频道列表 */
    {
        ScopedLinuxLock guard(m_channelLock);
        for (size_t i = 0; i < m_channels.size(); i++) {
            delete m_channels[i];
        }
        m_channels.clear();
    }

    /* 清理服务器列表 */
    {
        ScopedLinuxLock guard(m_serverLock);
        for (size_t i = 0; i < m_gameServers.size(); i++) {
            delete m_gameServers[i];
        }
        m_gameServers.clear();
    }

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
void ChannelSelectService::registerProtocols() {
    LOG_INFO("Registering protocols...");

    /* CS_CONNECT = 0 - 客户端连接 */
    registerHandler(0, onCS_CONNECT);

    /* CS_ASK_CHANNEL_INFO = 1 - 请求频道列表 */
    registerHandler(1, onCS_ASK_CHANNEL_INFO);

    /* CS_SELECT_CHANNEL = 2 - 选择频道 */
    registerHandler(2, onCS_SELECT_CHANNEL);

    /* CS_PING = 3 - 心跳 */
    registerHandler(3, onCS_PING);
}

/*
 * setTick - 更新 tick
 */
void ChannelSelectService::setTick() {
    m_tick++;
}

/*
 * onTick - 定时任务
 */
void ChannelSelectService::onTick() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    /* 每秒执行一次 */
    if (now - m_lastTickTime < 1000) {
        return;
    }
    m_lastTickTime = now;

    setTick();

    /* 每 30 秒发送心跳到 Bridge */
    if (m_tick % 30 == 0) {
        if (m_bridgeClient.isConnected()) {
            m_bridgeClient.sendHeartbeat();
        }
    }

    /* 每 10 秒更新用户数量到 Bridge */
    if (m_tick % 10 == 0) {
        if (m_bridgeClient.isConnected()) {
            m_bridgeClient.sendUserCount(m_userCount);
        }
    }
}

/*
 * 协议处理器实现
 */
int ChannelSelectService::onCS_CONNECT(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_CONNECT: user=%u", user ? user->getUserId() : 0);

    if (!user) {
        return -1;
    }

    /* 构建连接响应 */
    /* 响应格式: header(13) + result(4) + server_time(4) */
    uint8_t response[32];
    memset(response, 0, sizeof(response));

    int dataLen = 8;  /* result(4) + server_time(4) */
    int totalLen = PACKET_HEADER_SIZE + dataLen;

    /* 构建头部 */
    response[0] = 0;  /* classification */
    *reinterpret_cast<uint32_t*>(response + 1) = totalLen;  /* length (little-endian) */
    response[5] = 0;  /* msg_no = 0 (SC_CONNECT_RESULT) */
    response[6] = 0;  /* reserved */
    *reinterpret_cast<uint32_t*>(response + 7) = 0;  /* checksum */
    *reinterpret_cast<uint16_t*>(response + 11) = user->getNextSequence();  /* sequence */

    /* 数据部分 */
    uint32_t result = htonl(0);  /* 0 = 成功 */
    uint32_t serverTime = htonl(static_cast<uint32_t>(time(NULL)));
    memcpy(response + PACKET_HEADER_SIZE, &result, 4);
    memcpy(response + PACKET_HEADER_SIZE + 4, &serverTime, 4);

    FillPacketCRC32(response, totalLen);
    user->sendRaw(response, totalLen);
    LOG_INFO("Sent connect response to user %u", user->getUserId());

    return 0;
}

int ChannelSelectService::onCS_ASK_CHANNEL_INFO(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_ASK_CHANNEL_INFO: user=%u", user ? user->getUserId() : 0);

    if (!user) {
        return -1;
    }

    ChannelSelectService* service = getChannelService();
    if (!service) {
        LOG_ERROR("No channel service instance");
        return -1;
    }

    /* 获取频道数量 */
    int channelCount = service->getChannelCount();
    if (channelCount == 0) {
        /* 没有已注册的频道，返回默认频道 */
        channelCount = 1;
    }

    /* 构建响应 */
    /* 格式: header(13) + count(4) + [id(4) + name(32) + status(4) + users(4) + max_users(4)] * count */
    /* 每个频道 48 字节 */
    int entrySize = 48;
    int dataLen = 4 + channelCount * entrySize;
    int totalLen = PACKET_HEADER_SIZE + dataLen;

    uint8_t* response = new uint8_t[totalLen];
    memset(response, 0, totalLen);

    /* 构建头部 */
    response[0] = 0;  /* classification */
    *reinterpret_cast<uint32_t*>(response + 1) = totalLen;  /* length */
    response[5] = 1;  /* msg_no = 1 (SC_CHANNEL_INFO) */
    response[6] = 0;  /* reserved */
    *reinterpret_cast<uint32_t*>(response + 7) = 0;  /* checksum */
    *reinterpret_cast<uint16_t*>(response + 11) = user->getNextSequence();  /* sequence */

    /* 数据部分 */
    uint8_t* data = response + PACKET_HEADER_SIZE;
    *reinterpret_cast<uint32_t*>(data) = htonl(channelCount);
    int offset = 4;

    if (service->getChannelCount() > 0) {
        /* 从服务获取真实频道列表 */
        /* TODO: 遍历 m_channels，需要加锁 */
        /* 暂时使用硬编码频道 */
    }

    /* 硬编码默认频道 */
    for (int i = 0; i < channelCount; i++) {
        /* channel_id */
        *reinterpret_cast<uint32_t*>(data + offset) = htonl(i + 1);
        offset += 4;

        /* channel_name (32 bytes) */
        char name[32];
        snprintf(name, sizeof(name), "Channel %d", i + 1);
        memcpy(data + offset, name, 32);
        offset += 32;

        /* status: 0=正常, 1=繁忙, 2=已满, 3=维护 */
        *reinterpret_cast<uint32_t*>(data + offset) = htonl(0);
        offset += 4;

        /* current_users */
        *reinterpret_cast<uint32_t*>(data + offset) = htonl(10);
        offset += 4;

        /* max_users */
        *reinterpret_cast<uint32_t*>(data + offset) = htonl(500);
        offset += 4;
    }

    FillPacketCRC32(response, totalLen);
    user->sendRaw(response, totalLen);
    LOG_INFO("Sent channel list: count=%d to user %u", channelCount, user->getUserId());

    delete[] response;
    return 0;
}

int ChannelSelectService::onCS_SELECT_CHANNEL(tagPacketHeader* header, TCPUser* user) {
    LOG_INFO("onCS_SELECT_CHANNEL: user=%u", user ? user->getUserId() : 0);

    if (!user) {
        return -1;
    }

    ChannelSelectService* service = getChannelService();
    if (!service) {
        LOG_ERROR("No channel service instance");
        return -1;
    }

    /* 解析请求数据 */
    /* 请求格式: header 后面应该有 channel_id(4) */
    /* 由于使用 MessageHandler 而非 MessageHandlerExtra，需要从 user 缓冲区读取 */
    /* 暂时使用默认频道 1 */
    int selectedChannel = 1;

    LOG_INFO("User %u selected channel %d", user->getUserId(), selectedChannel);

    /* 构建响应 */
    /* 格式: header(13) + result(4) + game_ip(16) + game_port(4) + session_key(32) */
    int dataLen = 56;  /* 4 + 16 + 4 + 32 */
    int totalLen = PACKET_HEADER_SIZE + dataLen;

    uint8_t response[128];
    memset(response, 0, sizeof(response));

    /* 构建头部 */
    response[0] = 0;  /* classification */
    *reinterpret_cast<uint32_t*>(response + 1) = totalLen;  /* length */
    response[5] = 2;  /* msg_no = 2 (SC_SELECT_CHANNEL_RESULT) */
    response[6] = 0;  /* reserved */
    *reinterpret_cast<uint32_t*>(response + 7) = 0;  /* checksum */
    *reinterpret_cast<uint16_t*>(response + 11) = user->getNextSequence();  /* sequence */

    /* 数据部分 */
    uint8_t* data = response + PACKET_HEADER_SIZE;
    int offset = 0;

    /* result: 0=成功 */
    *reinterpret_cast<uint32_t*>(data + offset) = htonl(0);
    offset += 4;

    /* game_ip (16 bytes) */
    const char* gameIp = service->getConfig().gameIp;
    strncpy(reinterpret_cast<char*>(data + offset), gameIp, 15);
    offset += 16;

    /* game_port */
    uint32_t gamePort = static_cast<uint32_t>(service->getConfig().gamePort);
    *reinterpret_cast<uint32_t*>(data + offset) = htonl(gamePort);
    offset += 4;

    /* session_key (32 bytes) - 简单生成 */
    char sessionKey[32];
    snprintf(sessionKey, sizeof(sessionKey), "SESSION_%u_%u",
             user->getUserId(), static_cast<uint32_t>(time(NULL)));
    memcpy(data + offset, sessionKey, 32);
    offset += 32;

    FillPacketCRC32(response, totalLen);
    user->sendRaw(response, totalLen);
    LOG_INFO("Sent select channel result: game=%s:%u session=%s to user %u",
             gameIp, gamePort, sessionKey, user->getUserId());

    return 0;
}

int ChannelSelectService::onCS_PING(tagPacketHeader* header, TCPUser* user) {
    /* PING/PONG 心跳 */
    if (!user) {
        return -1;
    }

    /* 构建 PONG 响应 */
    /* 格式: header(13) + timestamp(8) */
    int dataLen = 8;
    int totalLen = PACKET_HEADER_SIZE + dataLen;

    uint8_t response[32];
    memset(response, 0, sizeof(response));

    /* 构建头部 */
    response[0] = 0;  /* classification */
    *reinterpret_cast<uint32_t*>(response + 1) = totalLen;  /* length */
    response[5] = 3;  /* msg_no = 3 (SC_PONG) */
    response[6] = 0;  /* reserved */
    *reinterpret_cast<uint32_t*>(response + 7) = 0;  /* checksum */
    *reinterpret_cast<uint16_t*>(response + 11) = user->getNextSequence();  /* sequence */

    /* 数据部分: 当前时间戳 (毫秒) */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t timestamp = static_cast<uint64_t>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;

    /* 大端序时间戳 (分两个32位) */
    uint32_t high = htonl(static_cast<uint32_t>(timestamp >> 32));
    uint32_t low = htonl(static_cast<uint32_t>(timestamp & 0xFFFFFFFF));
    memcpy(response + PACKET_HEADER_SIZE, &high, 4);
    memcpy(response + PACKET_HEADER_SIZE + 4, &low, 4);

    FillPacketCRC32(response, totalLen);
    user->sendRaw(response, totalLen);

    return 0;
}

int ChannelSelectService::onBS_CHANNEL_LIST_UPDATE(tagPacketHeader* header) {
    LOG_INFO("onBS_CHANNEL_LIST_UPDATE");

    /* TODO: 处理来自 Bridge 的频道列表更新 */

    return 0;
}

int ChannelSelectService::onBS_GAME_SERVER_UPDATE(tagPacketHeader* header) {
    LOG_INFO("onBS_GAME_SERVER_UPDATE");

    /* TODO: 处理来自 Bridge 的 Game 服务器更新 */

    return 0;
}

int ChannelSelectService::onBS_NOTICE(tagPacketHeader* header) {
    LOG_INFO("onBS_NOTICE");

    /* TODO: 处理来自 Bridge 的通知 */

    return 0;
}

/*
 * createUser - 创建用户
 */
TCPUser* ChannelSelectService::createUser() {
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
void ChannelSelectService::destroyUser(TCPUser* user) {
    if (user) {
        m_userPools.destroyTCPUser(user, __FILE__, __LINE__);
        m_userCount--;
        m_userStates.totalDisconnections++;
    }
}

/*
 * 频道管理
 */
void ChannelSelectService::addChannel(ChannelInfo* channel) {
    if (!channel) return;

    ScopedLinuxLock guard(m_channelLock);
    m_channels.push_back(channel);
}

void ChannelSelectService::removeChannel(int id) {
    ScopedLinuxLock guard(m_channelLock);

    for (size_t i = 0; i < m_channels.size(); i++) {
        if (m_channels[i]->getId() == id) {
            delete m_channels[i];
            m_channels.erase(m_channels.begin() + i);
            break;
        }
    }
}

ChannelInfo* ChannelSelectService::getChannel(int id) {
    ScopedLinuxLock guard(m_channelLock);

    for (size_t i = 0; i < m_channels.size(); i++) {
        if (m_channels[i]->getId() == id) {
            return m_channels[i];
        }
    }
    return NULL;
}

/*
 * Game 服务器管理
 */
void ChannelSelectService::addGameServer(GameServer* server) {
    if (!server) return;

    ScopedLinuxLock guard(m_serverLock);
    m_gameServers.push_back(server);
}

void ChannelSelectService::removeGameServer(int id) {
    ScopedLinuxLock guard(m_serverLock);

    for (size_t i = 0; i < m_gameServers.size(); i++) {
        if (m_gameServers[i]->getId() == id) {
            delete m_gameServers[i];
            m_gameServers.erase(m_gameServers.begin() + i);
            break;
        }
    }
}

GameServer* ChannelSelectService::getGameServer(int id) {
    ScopedLinuxLock guard(m_serverLock);

    for (size_t i = 0; i < m_gameServers.size(); i++) {
        if (m_gameServers[i]->getId() == id) {
            return m_gameServers[i];
        }
    }
    return NULL;
}

/*
 * processAcceptedUsers - 处理新接受的连接
 */
void ChannelSelectService::processAcceptedUsers() {
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
void ChannelSelectService::processUserEvents(int nfds) {
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
int ChannelSelectService::processUserData(TCPUser* user) {
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

/*
 * processBridgeData - 处理 Bridge 数据
 */
void ChannelSelectService::processBridgeData() {
    if (!m_bridgeClient.isConnected()) {
        return;
    }

    /* 处理从 Bridge 接收到的数据 */
    m_bridgeClient.processReceivedData();
}

} /* namespace ChannelServiceApp */
