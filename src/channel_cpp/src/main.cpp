/*
 * main.cpp - DNF Channel Service 入口
 *
 * 用法:
 *   ./df_channel_r channel        # 使用 cfg/channel.cfg 配置
 *   ./df_channel_r channel -d     # 后台运行
 */

#include "../include/channel_select_service.h"
#include "../../bridge_cpp/include/file_log.h"
#include "../../bridge_cpp/include/packet_dump.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>

using namespace ChannelServiceApp;

/* 全局服务指针 */
static ChannelSelectService* g_service = NULL;

/*
 * 信号处理
 */
static void signalHandler(int sig) {
    LOG_INFO("Received signal %d", sig);

    if (g_service) {
        g_service->setStopping();
    }
}

static void setupSignals() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);
}

/*
 * 打印使用方法
 */
static void printUsage(const char* program) {
    printf("DNF Channel Service (C++ Version)\n");
    printf("\n");
    printf("Usage: %s <config_name> [-d]\n", program);
    printf("\n");
    printf("Arguments:\n");
    printf("  config_name    Configuration name (uses cfg/<name>.cfg)\n");
    printf("  -d             Run as daemon (background)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s channel        Run with cfg/channel.cfg\n", program);
    printf("  %s channel -d     Run as daemon\n", program);
    printf("\n");
}

/*
 * 主函数
 */
int main(int argc, char* argv[]) {
    /* 初始化包落盘（受 packet_dump_enabled 文件控制） */
    packet_dump_init();

    /* 解析参数 */
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    char* configName = argv[1];
    bool daemon = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            daemon = true;
        }
    }

    /* 初始化日志 */
    char logPath[256];
    snprintf(logPath, sizeof(logPath), "log/%s.log", configName);
    g_LogInfo.open(logPath, configName);
    g_LogWarn.open(logPath, configName);
    g_LogError.open(logPath, configName);
    g_LogCri.open(logPath, configName);

    LOG_INFO("=== DNF Channel Service Starting ===");
    LOG_INFO("Config: %s", configName);
    LOG_INFO("Daemon: %s", daemon ? "yes" : "no");

    /* 设置信号处理 */
    setupSignals();

    /* 创建服务 */
    g_service = new ChannelSelectService(configName);
    if (!g_service) {
        LOG_ERROR("Failed to create service");
        return 1;
    }

    /* 读取配置 */
    if (!g_service->readConfig()) {
        LOG_ERROR("Failed to read config");
        delete g_service;
        return 1;
    }

    /* 后台运行 */
    if (daemon) {
        g_service->runAsDaemon();
    }

    /* 启动服务 */
    if (g_service->start() != 0) {
        LOG_ERROR("Failed to start service");
        delete g_service;
        return 1;
    }

    LOG_INFO("=== DNF Channel Service Running ===");

    /* 主循环 */
    g_service->run();

    /* 停止服务 */
    g_service->onStop();

    LOG_INFO("=== DNF Channel Service Stopped ===");

    /* 清理 */
    delete g_service;
    g_service = NULL;

    /* 关闭日志 */
    g_LogInfo.close();
    g_LogWarn.close();
    g_LogError.close();
    g_LogCri.close();

    return 0;
}
