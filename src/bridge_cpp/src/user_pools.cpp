/*
 * user_pools.cpp - 用户池管理实现
 */

#include "../include/user_pools.h"
#include "../include/file_log.h"

namespace ChannelServiceApp {

UserPools::UserPools() {
}

UserPools::~UserPools() {
    shutdown();
}

bool UserPools::startup() {
    LOG_INFO("Starting user pools...");

    if (!m_socketPool.startup()) {
        LOG_ERROR("Failed to startup socket pool");
        return false;
    }
    LOG_INFO("Socket pool started: max=%d", MAX_SOCKET_POOL_SIZE);

    if (!m_userPool.startup()) {
        LOG_ERROR("Failed to startup user pool");
        return false;
    }
    LOG_INFO("User pool started: max=%d", MAX_USER_POOL_SIZE);

    return true;
}

void UserPools::shutdown() {
    LOG_INFO("Shutting down user pools...");
    /* 内存池析构函数会自动清理 */
}

TCPSocket* UserPools::createTCPSocket(const char* file, int line) {
    TCPSocket* socket = m_socketPool.alloc();
    if (!socket) {
        LOG_WARN("Socket pool exhausted at %s:%d", file, line);
        return NULL;
    }
    socket->reset();
    return socket;
}

void UserPools::destroyTCPSocket(TCPSocket* socket) {
    if (socket) {
        socket->close();
        socket->reset();
        m_socketPool.free(socket);
    }
}

TCPUser* UserPools::createTCPUser(const char* file, int line) {
    TCPUser* user = m_userPool.alloc();
    if (!user) {
        LOG_WARN("User pool exhausted at %s:%d", file, line);
        return NULL;
    }
    user->reset();
    return user;
}

void UserPools::destroyTCPUser(TCPUser* user, const char* file, int line) {
    if (user) {
        user->shutdown();
        user->reset();
        m_userPool.free(user);
    }
}

int UserPools::getSocketPoolRemain() {
    return m_socketPool.getRemain();
}

int UserPools::getUserPoolRemain() {
    return m_userPool.getRemain();
}

} /* namespace ChannelServiceApp */
