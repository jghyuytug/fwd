/*
 * check_thread.h - 检查线程
 *
 * 基于生产版的 CheckThread 设计:
 * - 周期性检查用户连接状态
 * - 清理超时连接
 * - 统计信息收集
 */

#ifndef __CHECK_THREAD_H__
#define __CHECK_THREAD_H__

#include <pthread.h>
#include "thread_lock.h"

namespace ChannelServiceApp {

/*
 * CheckThread - 检查线程
 */
class CheckThread {
public:
    CheckThread();
    virtual ~CheckThread();

    /*
     * start - 启动线程
     */
    bool start();

    /*
     * stop - 停止线程
     */
    void stop();

    /*
     * loop - 线程主循环 (静态入口)
     */
    static void* loop(void* arg);

    /*
     * isRunning - 检查是否运行中
     */
    bool isRunning() const { return m_running; }

    /*
     * setCheckInterval - 设置检查间隔 (毫秒)
     */
    void setCheckInterval(int interval) { m_checkInterval = interval; }

protected:
    /*
     * onCheck - 执行检查 (子类可重写)
     */
    virtual void onCheck();

    /*
     * checkTimeoutUsers - 检查超时用户
     */
    void checkTimeoutUsers();

    /*
     * collectStatistics - 收集统计信息
     */
    void collectStatistics();

protected:
    pthread_t m_thread;
    volatile bool m_running;
    int m_checkInterval;        /* 检查间隔 (毫秒) */
    int m_userTimeout;          /* 用户超时时间 (秒) */

    TThreadStateControl<NoLock> m_stateControl;
};

} /* namespace ChannelServiceApp */

#endif /* __CHECK_THREAD_H__ */
