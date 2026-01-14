/*
 * server_group.cpp - 服务器组管理实现
 */

#include "../include/server_group.h"
#include <cstring>

namespace ChannelServiceApp {

/*
 * ChannelServer 实现
 */
ChannelServer::ChannelServer()
    : m_id(0)
    , m_port(0)
    , m_online(false)
    , m_userCount(0)
{
    m_ip[0] = '\0';
}

ChannelServer::~ChannelServer() {
}

void ChannelServer::setIp(const char* ip) {
    if (ip) {
        strncpy(m_ip, ip, sizeof(m_ip) - 1);
    }
}

/*
 * ServerGroup 实现
 */
ServerGroup::ServerGroup()
    : m_serverCount(0)
{
}

ServerGroup::~ServerGroup() {
    ScopedLinuxLock guard(m_lock);
    for (size_t i = 0; i < m_servers.size(); i++) {
        delete m_servers[i];
    }
    m_servers.clear();
}

void ServerGroup::addServer(ChannelServer* server) {
    if (!server) {
        return;
    }

    ScopedLinuxLock guard(m_lock);
    m_servers.push_back(server);
    m_serverCount++;
}

void ServerGroup::removeServer(uint32_t id) {
    ScopedLinuxLock guard(m_lock);

    for (size_t i = 0; i < m_servers.size(); i++) {
        if (m_servers[i]->getId() == id) {
            delete m_servers[i];
            m_servers.erase(m_servers.begin() + i);
            m_serverCount--;
            break;
        }
    }
}

ChannelServer* ServerGroup::getServer(uint32_t id) {
    ScopedLinuxLock guard(m_lock);

    for (size_t i = 0; i < m_servers.size(); i++) {
        if (m_servers[i]->getId() == id) {
            return m_servers[i];
        }
    }
    return NULL;
}

int ServerGroup::getTotalUserCount() const {
    int total = 0;
    for (size_t i = 0; i < m_servers.size(); i++) {
        total += m_servers[i]->getUserCount();
    }
    return total;
}

} /* namespace ChannelServiceApp */
