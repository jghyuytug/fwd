/*
 * user_pools.h - 用户池管理
 *
 * 基于生产版的 UserPools 设计:
 * - TCPSocket 内存池
 * - TCPUser 内存池
 */

#ifndef __USER_POOLS_H__
#define __USER_POOLS_H__

#include "memory_pool.h"
#include "tcp_socket.h"
#include "tcp_user.h"

namespace ChannelServiceApp {

/* 池大小常量 */
const int MAX_SOCKET_POOL_SIZE = 1000;
const int MAX_USER_POOL_SIZE = 1000;

/*
 * UserPools - 用户池管理类
 */
class UserPools {
public:
    UserPools();
    ~UserPools();

    /*
     * startup - 初始化所有池
     */
    bool startup();

    /*
     * shutdown - 关闭所有池
     */
    void shutdown();

    /*
     * createTCPSocket - 创建 TCP 套接字
     * @file: 调用文件名 (调试用)
     * @line: 调用行号 (调试用)
     */
    TCPSocket* createTCPSocket(const char* file, int line);

    /*
     * destroyTCPSocket - 销毁 TCP 套接字
     */
    void destroyTCPSocket(TCPSocket* socket);

    /*
     * createTCPUser - 创建 TCP 用户
     * @file: 调用文件名 (调试用)
     * @line: 调用行号 (调试用)
     */
    TCPUser* createTCPUser(const char* file, int line);

    /*
     * destroyTCPUser - 销毁 TCP 用户
     * @user: 用户对象
     * @file: 调用文件名 (调试用)
     * @line: 调用行号 (调试用)
     */
    void destroyTCPUser(TCPUser* user, const char* file, int line);

    /*
     * 统计信息
     */
    int getSocketPoolRemain();
    int getUserPoolRemain();

private:
    TMemoryPoolStatic<TCPSocket, MAX_SOCKET_POOL_SIZE> m_socketPool;
    TMemoryPoolStatic<TCPUser, MAX_USER_POOL_SIZE> m_userPool;
};

/* 便捷宏 */
#define CREATE_TCP_SOCKET() \
    UserPools::getInstance()->createTCPSocket(__FILE__, __LINE__)
#define DESTROY_TCP_SOCKET(s) \
    UserPools::getInstance()->destroyTCPSocket(s)
#define CREATE_TCP_USER() \
    UserPools::getInstance()->createTCPUser(__FILE__, __LINE__)
#define DESTROY_TCP_USER(u) \
    UserPools::getInstance()->destroyTCPUser(u, __FILE__, __LINE__)

} /* namespace ChannelServiceApp */

#endif /* __USER_POOLS_H__ */
