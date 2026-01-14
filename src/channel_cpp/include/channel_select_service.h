/*
 * channel_select_service.h - Channel 选择服务主类
 *
 * 基于生产版 Channel 服务设计:
 * - 监听端口 7001
 * - 连接到 Bridge (7000)
 * - 管理频道列表
 * - 处理客户端频道选择
 */

#ifndef __CHANNEL_SELECT_SERVICE_H__
#define __CHANNEL_SELECT_SERVICE_H__

#include "../../bridge_cpp/include/linux_service.h"
#include "../../bridge_cpp/include/tcp_handler.h"
#include "../../bridge_cpp/include/tcp_accept_thread.h"
#include "../../bridge_cpp/include/user_pools.h"
#include "../../bridge_cpp/include/epoll_reactor.h"
#include "../../bridge_cpp/include/memory_pool.h"
#include "../../bridge_cpp/include/server_group.h"
#include "bridge_client.h"

namespace ChannelServiceApp {

/* 服务模式 */
enum ChannelServiceMode {
    CHANNEL_MODE_NORMAL = 0,
    CHANNEL_MODE_MAINTENANCE,
    CHANNEL_MODE_SHUTDOWN
};

/*
 * GameServer - 游戏服务器信息
 */
class GameServer {
public:
    GameServer();
    ~GameServer();

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

    void setIp(const char* ip);
    const char* getIp() const { return m_ip; }

    void setPort(int port) { m_port = port; }
    int getPort() const { return m_port; }

    void setOnline(bool online) { m_online = online; }
    bool isOnline() const { return m_online; }

    void setUserCount(int count) { m_userCount = count; }
    int getUserCount() const { return m_userCount; }

    void setMaxUsers(int max) { m_maxUsers = max; }
    int getMaxUsers() const { return m_maxUsers; }

private:
    int m_id;
    char m_ip[64];
    int m_port;
    bool m_online;
    int m_userCount;
    int m_maxUsers;
};

/*
 * ChannelInfo - 频道信息
 */
class ChannelInfo {
public:
    ChannelInfo();
    ~ChannelInfo();

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

    void setName(const char* name);
    const char* getName() const { return m_name; }

    void setGameServer(GameServer* server) { m_gameServer = server; }
    GameServer* getGameServer() const { return m_gameServer; }

    void setUserCount(int count) { m_userCount = count; }
    int getUserCount() const { return m_userCount; }

    void setMaxUsers(int max) { m_maxUsers = max; }
    int getMaxUsers() const { return m_maxUsers; }

    void setStatus(int status) { m_status = status; }
    int getStatus() const { return m_status; }

private:
    int m_id;
    char m_name[64];
    GameServer* m_gameServer;
    int m_userCount;
    int m_maxUsers;
    int m_status;       /* 0=正常, 1=繁忙, 2=已满, 3=维护 */
};

/*
 * TChannelManager - 单例管理器模板
 */
template<typename T>
class TChannelManager {
public:
    TChannelManager() {}

    static T* getManager() {
        return s_manager;
    }

    static void setManager(T* manager) {
        s_manager = manager;
    }

private:
    static T* s_manager;
};

template<typename T>
T* TChannelManager<T>::s_manager = NULL;

/*
 * ChannelSelectService - Channel 选择服务主类
 */
class ChannelSelectService : public LinuxService, public TCPHandler {
public:
    ChannelSelectService();
    ChannelSelectService(char* configName);
    virtual ~ChannelSelectService();

    /* LinuxService 虚函数实现 */
    virtual bool onStart();
    virtual void onStop();
    virtual void run();
    virtual bool readConfig();

    /*
     * startup - 启动服务
     */
    bool startup();

    /*
     * shutdown - 关闭服务
     */
    void shutdown();

    /*
     * registerProtocols - 注册协议处理器
     */
    void registerProtocols();

    /*
     * 模式管理
     */
    void setMode(ChannelServiceMode mode) { m_mode = mode; }
    ChannelServiceMode getMode() const { return m_mode; }

    /*
     * Tick 管理
     */
    void setTick();
    uint64_t getTick() const { return m_tick; }

    /*
     * 获取服务名称
     */
    const char* getServiceName() const { return "ChannelSelectService"; }

    /*
     * Bridge 客户端
     */
    BridgeClient* getBridgeClient() { return &m_bridgeClient; }

    /*
     * 协议处理器 - 来自客户端
     */
    static int onCS_CONNECT(tagPacketHeader* header, TCPUser* user);
    static int onCS_ASK_CHANNEL_INFO(tagPacketHeader* header, TCPUser* user);
    static int onCS_SELECT_CHANNEL(tagPacketHeader* header, TCPUser* user);
    static int onCS_PING(tagPacketHeader* header, TCPUser* user);

    /*
     * 协议处理器 - 来自 Bridge
     */
    static int onBS_CHANNEL_LIST_UPDATE(tagPacketHeader* header);
    static int onBS_GAME_SERVER_UPDATE(tagPacketHeader* header);
    static int onBS_NOTICE(tagPacketHeader* header);

    /*
     * 用户管理
     */
    TCPUser* createUser();
    void destroyUser(TCPUser* user);
    int getUserCount() const { return m_userCount; }
    int getMaxUsers() const { return m_maxUsers; }

    /*
     * 频道管理
     */
    void addChannel(ChannelInfo* channel);
    void removeChannel(int id);
    ChannelInfo* getChannel(int id);
    int getChannelCount() const { return static_cast<int>(m_channels.size()); }

    /*
     * Game 服务器管理
     */
    void addGameServer(GameServer* server);
    void removeGameServer(int id);
    GameServer* getGameServer(int id);
    int getGameServerCount() const { return static_cast<int>(m_gameServers.size()); }

    /*
     * 配置
     */
    struct Config {
        /* 本服务配置 */
        char listenIp[64];
        int tcpPort;
        int udpPort;
        int maxUsers;
        int channelId;

        /* Bridge 连接配置 */
        char bridgeIp[64];
        int bridgePort;

        /* Game 跳转配置（返回给客户端） */
        char gameIp[64];
        int gamePort;

        /* 数据库配置 */
        char dbIp[64];
        char dbName[64];
        char dbUser[64];
        char dbPass[64];
        int dbPort;

        Config() {
            memset(this, 0, sizeof(Config));
            tcpPort = 7001;
            udpPort = 7001;
            maxUsers = 1000;
            channelId = 1;
            bridgePort = 7000;
            strcpy(gameIp, "127.0.0.1");
            gamePort = 10011;
            dbPort = 3306;
        }
    };

    Config& getConfig() { return m_config; }

protected:
    /*
     * 处理新连接
     */
    void processAcceptedUsers();

    /*
     * 处理用户事件
     */
    void processUserEvents(int nfds);

    /*
     * 处理用户数据
     */
    int processUserData(TCPUser* user);

    /*
     * 处理 Bridge 数据
     */
    void processBridgeData();

    /*
     * 定时任务
     */
    void onTick();

private:
    Config m_config;                    /* 配置 */
    ChannelServiceMode m_mode;          /* 服务模式 */
    uint64_t m_tick;                    /* 时钟 */
    uint64_t m_lastTickTime;            /* 上次 Tick 时间 */

    TCPSocket m_listenSocket;           /* 监听套接字 */
    TCPAcceptThread m_acceptThread;     /* 接受线程 */

    UserPools m_userPools;              /* 用户池 */
    TReactor<EpollReactor<TCPUser>, TCPUser> m_reactor;  /* 事件反应器 */
    TCPHandlerRelay m_handlerRelay;     /* 协议分发器 */

    BridgeClient m_bridgeClient;        /* Bridge 连接 */

    int m_userCount;                    /* 当前用户数 */
    int m_maxUsers;                     /* 最大用户数 */

    std::vector<ChannelInfo*> m_channels;      /* 频道列表 */
    std::vector<GameServer*> m_gameServers;    /* Game 服务器列表 */

    LinuxLock m_channelLock;            /* 频道列表锁 */
    LinuxLock m_serverLock;             /* 服务器列表锁 */

    TCPUserStates m_userStates;         /* 用户统计 */
};

/* 全局服务访问 */
typedef TChannelManager<ChannelSelectService> ChannelSelectServiceManager;

inline ChannelSelectService* getChannelService() {
    return ChannelSelectServiceManager::getManager();
}

} /* namespace ChannelServiceApp */

#endif /* __CHANNEL_SELECT_SERVICE_H__ */
