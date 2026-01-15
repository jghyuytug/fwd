/*
 * Network Basic Test - 网络基础功能测试
 *
 * 测试内容：
 * 1. 初始化网络模块
 * 2. 启动监听socket
 * 3. 运行事件循环
 * 4. 接受客户端连接
 *
 * 编译命令：
 *   gcc -o network_test network_basic_test.c \
 *       ../src/network/network.c \
 *       ../src/network/network_init.c \
 *       -I../include \
 *       -pthread -m32
 *
 * 运行：
 *   ./network_test
 *
 * 测试连接：
 *   telnet localhost 7001
 *   或
 *   nc localhost 7001
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// 引入网络模块头文件
#include "network/headers/network_interface.h"

// 外部函数声明（在network.c中定义）
extern int Network_EventLoop(int listen_fd);
extern void Network_StopEventLoop(void);

// 全局变量：监听socket fd
static int g_listen_fd = -1;

/**
 * 信号处理函数 - 捕获Ctrl+C
 */
void signal_handler(int signum)
{
    printf("\n[Test] Caught signal %d, shutting down...\n", signum);

    // 停止事件循环
    Network_StopEventLoop();

    // 清理网络模块
    Network_Cleanup();

    exit(0);
}

/**
 * 主函数
 */
int main(int argc, char *argv[])
{
    printf("==============================================\n");
    printf("  DNF Network Module - Basic Functionality Test\n");
    printf("==============================================\n\n");

    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 1. 初始化网络模块
    printf("[Test] Step 1: Initializing network module...\n");
    int result = Network_Initialize(4096, 7001);
    if (result != 0) {
        fprintf(stderr, "[Test] ERROR: Network_Initialize failed (code=%d)\n", result);
        return 1;
    }
    printf("[Test] ✓ Network module initialized (max_connections=4096, port=7001)\n\n");

    // 2. 启动网络服务（创建监听socket）
    printf("[Test] Step 2: Starting network services...\n");
    g_listen_fd = Network_Start();
    if (g_listen_fd < 0) {
        fprintf(stderr, "[Test] ERROR: Network_Start failed (fd=%d)\n", g_listen_fd);
        Network_Cleanup();
        return 1;
    }
    printf("[Test] ✓ Network services started (listen_fd=%d)\n\n", g_listen_fd);

    // 3. 打印测试说明
    printf("==============================================\n");
    printf("  Server is now running!\n");
    printf("==============================================\n");
    printf("\n");
    printf("Listening on: 0.0.0.0:7001\n");
    printf("\n");
    printf("How to test:\n");
    printf("  1. Open another terminal\n");
    printf("  2. Run: telnet localhost 7001\n");
    printf("     or: nc localhost 7001\n");
    printf("  3. You should see connection messages below\n");
    printf("\n");
    printf("Press Ctrl+C to stop the server\n");
    printf("==============================================\n\n");

    // 4. 运行事件循环（阻塞直到停止）
    printf("[Test] Step 3: Starting event loop...\n\n");
    result = Network_EventLoop(g_listen_fd);

    if (result != 0) {
        fprintf(stderr, "[Test] WARNING: Event loop exited with code %d\n", result);
    } else {
        printf("[Test] ✓ Event loop exited normally\n");
    }

    // 5. 清理
    printf("\n[Test] Step 4: Cleaning up...\n");
    Network_Cleanup();
    printf("[Test] ✓ Cleanup complete\n");

    printf("\n==============================================\n");
    printf("  Test completed\n");
    printf("==============================================\n");

    return 0;
}
