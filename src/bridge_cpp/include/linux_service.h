/*
 * linux_service.h - Linux 服务框架
 *
 * 基于生产版的 LinuxService 设计:
 * - 守护进程支持
 * - 信号处理
 * - PID 文件管理
 * - 配置文件检查
 */

#ifndef __LINUX_SERVICE_H__
#define __LINUX_SERVICE_H__

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "service_info.h"
#include "file_log.h"

namespace ChannelServiceApp {

/* 服务状态 */
enum ServiceState {
    SERVICE_STOPPED = 0,
    SERVICE_START_PENDING,
    SERVICE_STOP_PENDING,
    SERVICE_RUNNING,
    SERVICE_CONTINUE_PENDING,
    SERVICE_PAUSE_PENDING,
    SERVICE_PAUSED
};

/*
 * LinuxService - Linux 服务基类
 *
 * 子类需要实现:
 * - onStart(): 服务启动时调用
 * - onStop(): 服务停止时调用
 * - run(): 主循环
 */
class LinuxService {
public:
    LinuxService();
    virtual ~LinuxService();

    /* 单例访问 */
    static LinuxService* getInstance() { return theInstance; }

    /*
     * processCommandLine - 处理命令行参数
     * @argc: 参数数量
     * @argv: 参数数组
     * 返回: 0 成功, 非0 失败
     */
    int processCommandLine(int argc, char** argv);

    /*
     * main - 服务主入口
     * @configName: 配置名称
     */
    int main(char* configName);

    /*
     * start - 启动服务
     */
    int start();

    /*
     * install - 安装服务 (Linux 下无操作)
     */
    int install();

    /*
     * remove - 移除服务 (Linux 下无操作)
     */
    int remove();

    /* 状态控制 */
    void setServiceState(unsigned long state);
    void setRunning();
    void setStopped();
    void stopPending();

    void controlStop();
    void controlPause();
    void controlContinue();

    /* 停止请求 */
    void setStopping() { m_running = false; }

    /* 守护进程模式 */
    void runAsDaemon();

    /* 准备工作 */
    int prepareStart();
    bool checkConfigFile();
    bool checkPIDFile();

    /* 广播消息 */
    void sendBroadCastMessage(const char* msg);

    /* 子类需要实现的虚函数 */
    virtual bool onStart() = 0;
    virtual void onStop() = 0;
    virtual void run() = 0;
    virtual bool readConfig() = 0;

    /* 获取服务信息 */
    ServiceInfo& getServiceInfo() { return m_serviceInfo; }

protected:
    /* 写入 PID 文件 */
    bool writePIDFile();

    /* 移除 PID 文件 */
    void removePIDFile();

    /* 信号处理 */
    static void signalHandler(int sig);

    /* 守护进程化 */
    void daemonize();

protected:
    ServiceInfo m_serviceInfo;      /* 服务信息 */
    ServiceState m_state;           /* 服务状态 */
    volatile bool m_running;        /* 运行标志 */

    static LinuxService* theInstance;   /* 单例指针 */
};

} /* namespace ChannelServiceApp */

#endif /* __LINUX_SERVICE_H__ */
