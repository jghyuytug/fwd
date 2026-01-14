/*
 * main.cpp - DNF Bridge Service (C++ Version) 入口点
 *
 * 用法: ./df_bridge_r <config_name> start [-d]
 * 例如: ./df_bridge_r bridge start
 *       ./df_bridge_r bridge start -d  (守护进程模式)
 */

#include <cstdio>
#include <cstdlib>
#include "../include/channel_service.h"
#include "../include/packet_dump.h"

using namespace ChannelServiceApp;

/*
 * print_version - 打印版本信息
 */
static void print_version() {
    printf("DNF Bridge Service (C++ Version)\n");
    printf("Version: 2.0.0\n");
    printf("Built: %s %s\n", __DATE__, __TIME__);
}

/*
 * print_usage - 打印用法
 */
static void print_usage(const char* prog) {
    print_version();
    printf("\n");
    printf("Usage: %s <config_name> <command> [options]\n", prog);
    printf("\n");
    printf("Commands:\n");
    printf("  start    Start the bridge service\n");
    printf("  stop     Stop the bridge service (send SIGTERM)\n");
    printf("\n");
    printf("Options:\n");
    printf("  -d, --daemon    Run as daemon\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s bridge start\n", prog);
    printf("  %s bridge start -d\n", prog);
    printf("  %s bridge stop\n", prog);
    printf("\n");
    printf("Config file: cfg/<config_name>.cfg\n");
}

/*
 * main - 程序入口
 */
int main(int argc, char* argv[]) {
    /* 初始化包落盘功能 */
    packet_dump_init();

    /* 检查参数 */
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    /* 创建服务实例 */
    ChannelService service;

    /* 处理命令行参数 */
    return service.processCommandLine(argc, argv);
}
