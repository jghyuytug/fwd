/*
 * channel_service.h - Channel 服务主类
 *
 * 基于生产版的 ChannelService 设计:
 * - 服务主循环
 * - 协议注册
 * - 用户管理
 */

#ifndef __CHANNEL_SERVICE_H__
#define __CHANNEL_SERVICE_H__

#include "linux_service.h"
#include "tcp_handler.h"
#include "tcp_accept_thread.h"
#include "user_pools.h"
#include "epoll_reactor.h"
#include "memory_pool.h"

namespace ChannelServiceApp {

/* 服务模式 */
enum ServiceMode {
    MODE_NORMAL = 0,
    MODE_MAINTENANCE,
    MODE_SHUTDOWN
};

/*
 * TManager - 单例管理器模板
 */
template<typename T>
class TManager {
public:
    TManager() {}

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
T* TManager<T>::s_manager = NULL;

/*
 * ChannelService - Channel 服务主类
 */
class ChannelService : public LinuxService, public TCPHandler {
public:
    ChannelService();
    ChannelService(char* configName);
    virtual ~ChannelService();

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
    void registerProtocolsExtra();

    /*
     * 模式管理
     */
    void setMode(ServiceMode mode) { m_mode = mode; }
    ServiceMode getMode() const { return m_mode; }

    /*
     * Tick 管理
     */
    void setTick();
    uint64_t getTick() const { return m_tick; }

    /*
     * 获取服务名称
     */
    const char* getServiceName() const { return "ChannelService"; }

    /*
     * 协议处理器
     */
    static int onCS_GET_SCRIPT(tagPacketHeader* header, TCPUser* user);
    static int onCS_GET_GC_INFO(tagPacketHeader* header, TCPUser* user);
    static int onCS_CHECK_SCRIPT_VERSION(tagPacketHeader* header, TCPUser* user);
    static int onCS_NOTICE_CHANNEL_SERVER(tagPacketHeader* header, TCPUser* user);
    static int onCS_UPDATE_CHANNEL_INFO(tagPacketHeader* header, const uint8_t* data, int dataLen);

    /*
     * 用户管理
     */
    TCPUser* createUser();
    void destroyUser(TCPUser* user);
    int getUserCount() const { return m_userCount; }
    int getMaxUsers() const { return m_maxUsers; }

    /*
     * 配置
     */
    struct Config {
        char listenIp[64];
        int tcpPort;
        int udpPort;
        int maxUsers;

        char dbIp[64];
        char dbName[64];
        char dbUser[64];
        char dbPass[64];
        int dbPort;

        char channelIp[64];
        int channelPort;

        Config() {
            memset(this, 0, sizeof(Config));
            tcpPort = 7000;
            udpPort = 7000;
            maxUsers = 1000;
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

private:
    Config m_config;                /* 配置 */
    ServiceMode m_mode;             /* 服务模式 */
    uint64_t m_tick;                /* 时钟 */

    TCPSocket m_listenSocket;       /* 监听套接字 */
    TCPAcceptThread m_acceptThread; /* 接受线程 */

    UserPools m_userPools;          /* 用户池 */
    TReactor<EpollReactor<TCPUser>, TCPUser> m_reactor;  /* 事件反应器 */
    TCPHandlerRelay m_handlerRelay; /* 协议分发器 */

    int m_userCount;                /* 当前用户数 */
    int m_maxUsers;                 /* 最大用户数 */

    TCPUserStates m_userStates;     /* 用户统计 */
};

/* 全局服务访问 */
typedef TManager<ChannelService> ChannelServiceManager;

inline ChannelService* getService() {
    return ChannelServiceManager::getManager();
}

} /* namespace ChannelServiceApp */

#endif /* __CHANNEL_SERVICE_H__ */
