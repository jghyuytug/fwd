/*
 * tcp_accept_thread.h - TCP 接受线程
 *
 * 基于生产版的 TCPAcceptThread 设计:
 * - 独立线程处理新连接
 * - 连接队列管理
 */

#ifndef __TCP_ACCEPT_THREAD_H__
#define __TCP_ACCEPT_THREAD_H__

#include <pthread.h>
#include <queue>
#include "tcp_socket.h"
#include "tcp_user.h"
#include "thread_lock.h"

namespace ChannelServiceApp {

/*
 * TCPAcceptThread - TCP 接受线程
 */
class TCPAcceptThread {
public:
    TCPAcceptThread();
    virtual ~TCPAcceptThread();

    /*
     * start - 启动接受线程
     * @listenSocket: 监听套接字
     */
    bool start(TCPSocket* listenSocket);

    /*
     * stop - 停止接受线程
     */
    void stop();

    /*
     * lockPushAcceptedUser - 将接受的用户加入队列
     */
    void lockPushAcceptedUser(TCPUser* user);

    /*
     * lockPopAcceptedUser - 从队列取出用户
     */
    TCPUser* lockPopAcceptedUser();

    /*
     * getQueueSize - 获取队列大小
     */
    int getQueueSize();

    /*
     * notifyCannotCreateUser - 通知无法创建用户
     */
    void notifyCannotCreateUser(TCPSocket& socket);

    /*
     * notifyCannotLoginByMaxUserCount - 通知用户数已满
     */
    void notifyCannotLoginByMaxUserCount(TCPSocket& socket);

    /*
     * isRunning - 检查是否运行中
     */
    bool isRunning() const { return m_running; }

protected:
    /*
     * threadFunc - 线程函数
     */
    static void* threadFunc(void* arg);

    /*
     * run - 线程主循环
     */
    void run();

protected:
    pthread_t m_thread;             /* 线程 ID */
    TCPSocket* m_listenSocket;      /* 监听套接字 */
    volatile bool m_running;        /* 运行标志 */

    std::queue<TCPUser*> m_acceptQueue;  /* 接受队列 */
    LinuxLock m_queueLock;               /* 队列锁 */
};

} /* namespace ChannelServiceApp */

#endif /* __TCP_ACCEPT_THREAD_H__ */
