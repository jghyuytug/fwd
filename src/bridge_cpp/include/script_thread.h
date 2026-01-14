/*
 * script_thread.h - 脚本线程
 *
 * 基于生产版的 ScriptThread 设计:
 * - 脚本文件监控
 * - 脚本重载
 */

#ifndef __SCRIPT_THREAD_H__
#define __SCRIPT_THREAD_H__

#include <pthread.h>
#include "thread_lock.h"

namespace ChannelServiceApp {

/*
 * ChannelScript - 脚本管理
 */
class ChannelScript {
public:
    ChannelScript();
    ~ChannelScript();

    /*
     * ReloadScript - 重新加载脚本
     */
    bool ReloadScript();

    /*
     * getScriptFromFile - 从文件获取脚本
     */
    const char* getScriptFromFile();

    /*
     * getScriptFileSize - 获取脚本文件大小
     */
    int getScriptFileSize();

    /*
     * setScriptPath - 设置脚本路径
     */
    void setScriptPath(const char* path);

private:
    char m_scriptPath[256];
    char* m_scriptData;
    int m_scriptSize;
    LinuxLock m_lock;
};

/*
 * ScriptThread - 脚本线程
 */
class ScriptThread {
public:
    ScriptThread();
    virtual ~ScriptThread();

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

    /*
     * getScript - 获取脚本管理器
     */
    ChannelScript& getScript() { return m_script; }

protected:
    /*
     * onCheck - 执行检查
     */
    virtual void onCheck();

protected:
    pthread_t m_thread;
    volatile bool m_running;
    int m_checkInterval;

    ChannelScript m_script;
    TThreadStateControl<NoLock> m_stateControl;
};

} /* namespace ChannelServiceApp */

#endif /* __SCRIPT_THREAD_H__ */
