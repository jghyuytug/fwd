/*
 * server_group.h - 服务器组管理
 *
 * 基于生产版的 ServerGroup 设计:
 * - Channel 服务器管理
 * - 服务器计数
 */

#ifndef __SERVER_GROUP_H__
#define __SERVER_GROUP_H__

#include <stdint.h>
#include <vector>
#include "thread_lock.h"

namespace ChannelServiceApp {

/*
 * ChannelServer - Channel 服务器信息
 */
class ChannelServer {
public:
    ChannelServer();
    ~ChannelServer();

    void setId(uint32_t id) { m_id = id; }
    uint32_t getId() const { return m_id; }

    void setIp(const char* ip);
    const char* getIp() const { return m_ip; }

    void setPort(uint16_t port) { m_port = port; }
    uint16_t getPort() const { return m_port; }

    void setOnline(bool online) { m_online = online; }
    bool isOnline() const { return m_online; }

    void setUserCount(int count) { m_userCount = count; }
    int getUserCount() const { return m_userCount; }

private:
    uint32_t m_id;
    char m_ip[64];
    uint16_t m_port;
    bool m_online;
    int m_userCount;
};

/*
 * ServerGroup - 服务器组管理
 */
class ServerGroup {
public:
    ServerGroup();
    ~ServerGroup();

    /*
     * addServer - 添加服务器
     */
    void addServer(ChannelServer* server);

    /*
     * removeServer - 移除服务器
     */
    void removeServer(uint32_t id);

    /*
     * getServer - 获取服务器
     */
    ChannelServer* getServer(uint32_t id);

    /*
     * getServerCount - 获取服务器数量
     */
    int getServerCount() const { return m_serverCount; }

    /*
     * increseServerCount - 增加服务器计数
     */
    void increseServerCount() { m_serverCount++; }

    /*
     * decreseServerCount - 减少服务器计数
     */
    void decreseServerCount() { if (m_serverCount > 0) m_serverCount--; }

    /*
     * getTotalUserCount - 获取总用户数
     */
    int getTotalUserCount() const;

private:
    std::vector<ChannelServer*> m_servers;
    int m_serverCount;
    LinuxLock m_lock;
};

} /* namespace ChannelServiceApp */

#endif /* __SERVER_GROUP_H__ */
