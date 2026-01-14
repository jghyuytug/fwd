/*
 * linux_service.cpp - Linux 服务框架实现
 */

#include "../include/linux_service.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ChannelServiceApp {

/* 静态成员初始化 */
LinuxService* LinuxService::theInstance = NULL;

LinuxService::LinuxService()
    : m_state(SERVICE_STOPPED)
    , m_running(false)
{
    theInstance = this;
}

LinuxService::~LinuxService() {
    if (theInstance == this) {
        theInstance = NULL;
    }
}

/*
 * processCommandLine - 处理命令行参数
 */
int LinuxService::processCommandLine(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <config_name> <command>\n", argv[0]);
        fprintf(stderr, "Commands: start, stop\n");
        return 1;
    }

    const char* configName = argv[1];
    const char* command = argv[2];

    /* 设置服务信息 */
    char configPath[256];
    char pidPath[256];
    snprintf(configPath, sizeof(configPath), "cfg/%s.cfg", configName);
    snprintf(pidPath, sizeof(pidPath), "pid/%s.pid", configName);

    m_serviceInfo.setInfo(configName, configName, configPath, pidPath);

    /* 检查是否作为守护进程运行 */
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--daemon") == 0) {
            m_serviceInfo.setRunAsService(true);
        }
    }

    /* 处理命令 */
    if (strcmp(command, "start") == 0) {
        return main(const_cast<char*>(configName));
    }
    else if (strcmp(command, "stop") == 0) {
        /* 读取 PID 文件并发送信号 */
        FILE* fp = fopen(pidPath, "r");
        if (!fp) {
            fprintf(stderr, "Error: cannot read PID file: %s\n", pidPath);
            return 1;
        }

        int pid;
        if (fscanf(fp, "%d", &pid) != 1) {
            fprintf(stderr, "Error: invalid PID file format\n");
            fclose(fp);
            return 1;
        }
        fclose(fp);

        printf("Stopping service (PID %d)...\n", pid);
        if (kill(pid, SIGTERM) < 0) {
            perror("kill");
            return 1;
        }
        printf("Signal sent.\n");
        return 0;
    }
    else {
        fprintf(stderr, "Error: unknown command: %s\n", command);
        return 1;
    }
}

/*
 * main - 服务主入口
 */
int LinuxService::main(char* configName) {
    LOG_INFO("============================================");
    LOG_INFO("DNF Bridge Service (C++ Version)");
    LOG_INFO("============================================");

    /* 准备启动 */
    if (prepareStart() != 0) {
        return 1;
    }

    /* 守护进程化 */
    if (m_serviceInfo.isRunAsService()) {
        daemonize();
    }

    /* 写入 PID 文件 */
    if (!writePIDFile()) {
        LOG_ERROR("Failed to write PID file");
        return 1;
    }

    /* 启动服务 */
    return start();
}

/*
 * start - 启动服务
 */
int LinuxService::start() {
    /* 设置信号处理 */
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    /* 调用子类的启动函数 */
    if (!onStart()) {
        LOG_ERROR("Service startup failed");
        return 1;
    }

    setRunning();
    LOG_INFO("Service started successfully");

    /* 运行主循环 */
    run();

    /* 停止服务 */
    onStop();
    setStopped();

    /* 清理 PID 文件 */
    removePIDFile();

    LOG_INFO("Service stopped");
    return 0;
}

/*
 * install / remove - Linux 下无操作
 */
int LinuxService::install() {
    LOG_INFO("Install is not supported on Linux");
    return 0;
}

int LinuxService::remove() {
    LOG_INFO("Remove is not supported on Linux");
    return 0;
}

/*
 * 状态控制
 */
void LinuxService::setServiceState(unsigned long state) {
    m_state = static_cast<ServiceState>(state);
}

void LinuxService::setRunning() {
    m_state = SERVICE_RUNNING;
    m_running = true;
}

void LinuxService::setStopped() {
    m_state = SERVICE_STOPPED;
    m_running = false;
}

void LinuxService::stopPending() {
    m_state = SERVICE_STOP_PENDING;
}

void LinuxService::controlStop() {
    m_running = false;
}

void LinuxService::controlPause() {
    m_state = SERVICE_PAUSED;
}

void LinuxService::controlContinue() {
    m_state = SERVICE_RUNNING;
}

/*
 * prepareStart - 准备启动
 */
int LinuxService::prepareStart() {
    /* 检查配置文件 */
    if (!checkConfigFile()) {
        LOG_ERROR("Config file not found: %s",
                 m_serviceInfo.getConfigFileName());
        return 1;
    }

    /* 检查 PID 文件 */
    if (!checkPIDFile()) {
        LOG_ERROR("Service may already be running");
        return 1;
    }

    /* 读取配置 */
    if (!readConfig()) {
        LOG_ERROR("Failed to read config");
        return 1;
    }

    /* 初始化日志 */
    createLogInfo();
    createLogWarn();
    createLogError();
    createLogCri();
    createFileLogInfo();
    createFileLogWarn();
    createFileLogError();
    createFileLogCri();

    return 0;
}

/*
 * checkConfigFile - 检查配置文件
 */
bool LinuxService::checkConfigFile() {
    struct stat st;
    return stat(m_serviceInfo.getConfigFileName(), &st) == 0;
}

/*
 * checkPIDFile - 检查 PID 文件
 */
bool LinuxService::checkPIDFile() {
    struct stat st;
    const char* pidFile = m_serviceInfo.getPIDFileName();

    if (stat(pidFile, &st) != 0) {
        /* PID 文件不存在，可以启动 */
        return true;
    }

    /* PID 文件存在，检查进程是否还在运行 */
    FILE* fp = fopen(pidFile, "r");
    if (!fp) {
        return true;
    }

    int pid;
    if (fscanf(fp, "%d", &pid) != 1) {
        fclose(fp);
        return true;
    }
    fclose(fp);

    /* 检查进程是否存在 */
    if (kill(pid, 0) == 0) {
        /* 进程存在 */
        return false;
    }

    /* 进程不存在，删除旧的 PID 文件 */
    unlink(pidFile);
    return true;
}

/*
 * sendBroadCastMessage - 广播消息 (未实现)
 */
void LinuxService::sendBroadCastMessage(const char* msg) {
    LOG_INFO("Broadcast: %s", msg);
}

/*
 * writePIDFile - 写入 PID 文件
 */
bool LinuxService::writePIDFile() {
    /* 确保 pid 目录存在 */
    struct stat st;
    if (stat("pid", &st) != 0) {
        mkdir("pid", 0755);
    }

    FILE* fp = fopen(m_serviceInfo.getPIDFileName(), "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "%d\n", getpid());
    fclose(fp);

    LOG_INFO("PID file written: %s (PID=%d)",
            m_serviceInfo.getPIDFileName(), getpid());
    return true;
}

/*
 * removePIDFile - 移除 PID 文件
 */
void LinuxService::removePIDFile() {
    unlink(m_serviceInfo.getPIDFileName());
}

/*
 * signalHandler - 信号处理
 */
void LinuxService::signalHandler(int sig) {
    LOG_INFO("Received signal %d", sig);

    if (theInstance) {
        theInstance->controlStop();
    }
}

/*
 * runAsDaemon - 设置为守护进程模式
 */
void LinuxService::runAsDaemon() {
    daemonize();
}

/*
 * daemonize - 守护进程化
 */
void LinuxService::daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        LOG_ERROR("Failed to fork");
        exit(1);
    }

    if (pid > 0) {
        /* 父进程退出 */
        exit(0);
    }

    /* 子进程继续 */

    /* 创建新会话 */
    if (setsid() < 0) {
        LOG_ERROR("Failed to create new session");
        exit(1);
    }

    /* 改变工作目录 */
    /* chdir("/"); */

    /* 重定向标准文件描述符 */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_WRONLY);
}

} /* namespace ChannelServiceApp */
