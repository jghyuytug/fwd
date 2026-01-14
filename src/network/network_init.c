/*
 * Network Module Initialization - Pure C Implementation
 * Provides module-level initialization and cleanup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Include network headers
#include "network/headers/network_interface.h"
#include "network/headers/CSession.h"
#include "network/headers/PacketDispatcher.h"

// 外部函数声明（在network.c中定义）
extern int *CSession_Listen(void *self, char *address);
extern int Network_EventLoop(int listen_fd);
extern void Network_StopEventLoop(void);
extern void CEpoll_CEpoll(void *this, void *a1);
extern void CEpoll_destructor(void *this);

// 外部函数声明（在packet_handlers.c中定义）
extern int RegisterDefaultHandlers(void);

// 外部函数声明（在session_timer.c中定义 - Week 6）
extern int SessionTimer_Init(void);
extern void SessionTimer_Cleanup(void);

// Global state (simple stub)
static struct {
    int initialized;
    int max_connections;
    int port;
} g_network_state = {0};

/**
 * Network_Initialize - Initialize network module
 */
int Network_Initialize(int max_connections, int port)
{
    if (g_network_state.initialized) {
        return ERR_INVALID_STATE;
    }

    g_network_state.initialized = 1;
    g_network_state.max_connections = max_connections;
    g_network_state.port = port;

    // 初始化epoll实例
    CEpoll_CEpoll(NULL, NULL);

    // 初始化PacketDispatcher
    if (PacketDispatcher_Initialize() < 0) {
        fprintf(stderr, "[Network] ERROR: Failed to initialize PacketDispatcher\n");
        return ERR_INVALID_STATE;
    }

    // 注册默认数据包处理器
    if (RegisterDefaultHandlers() < 0) {
        fprintf(stderr, "[Network] ERROR: Failed to register default handlers\n");
        PacketDispatcher_Cleanup();
        return ERR_INVALID_STATE;
    }

    // Week 6: 初始化会话定时器
    if (SessionTimer_Init() < 0) {
        fprintf(stderr, "[Network] ERROR: Failed to initialize SessionTimer\n");
        PacketDispatcher_Cleanup();
        return ERR_INVALID_STATE;
    }

    printf("[Network] Initialized with max_connections=%d, port=%d\n",
           max_connections, port);

    return ERR_SUCCESS;
}

/**
 * Network_Cleanup - Cleanup network module
 */
void Network_Cleanup()
{
    if (!g_network_state.initialized) {
        return;
    }

    // Week 6: 清理会话定时器
    SessionTimer_Cleanup();

    // 清理PacketDispatcher
    PacketDispatcher_Cleanup();

    // 清理epoll实例
    CEpoll_destructor(NULL);

    printf("[Network] Cleanup complete\n");

    g_network_state.initialized = 0;
    g_network_state.max_connections = 0;
    g_network_state.port = 0;
}

/**
 * Network_Start - Start network services
 *
 * 实现说明 (2025-11-11):
 * - 创建监听socket并绑定到配置的端口
 * - 返回监听socket的文件描述符
 * - 调用者应该使用返回的fd启动事件循环
 *
 * @return 监听socket的文件描述符，失败返回负数
 */
int Network_Start()
{
    if (!g_network_state.initialized) {
        fprintf(stderr, "[Network] ERROR: Network not initialized\n");
        return ERR_INVALID_STATE;
    }

    // 构造监听地址字符串（"0.0.0.0:PORT"）
    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "0.0.0.0:%d", g_network_state.port);

    printf("[Network] Starting network services on %s\n", listen_addr);

    // 调用CSession_Listen创建监听socket
    int *listen_fd_ptr = CSession_Listen(NULL, listen_addr);
    if (!listen_fd_ptr) {
        fprintf(stderr, "[Network] Failed to create listen socket\n");
        return -1;
    }

    int listen_fd = (int)(intptr_t)listen_fd_ptr;

    printf("[Network] Network services started successfully (listen_fd=%d)\n", listen_fd);
    printf("[Network] To start event loop, call: Network_EventLoop(%d)\n", listen_fd);

    return listen_fd;  // 返回监听socket的fd
}

/**
 * Network_Stop - Stop network services
 */
void Network_Stop()
{
    if (!g_network_state.initialized) {
        return;
    }

    printf("[Network] Network services stopped\n");
}

/**
 * Network_SendPacket - Send packet to session
 */
int Network_SendPacket(void *session, const void *packet, int length)
{
    if (!g_network_state.initialized || !session || !packet || length <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    // Stub: just return success
    return ERR_SUCCESS;
}

/**
 * Network_GetConnectionCount - Get current connection count
 */
int Network_GetConnectionCount()
{
    return 0; // Stub
}

/**
 * Network_GetMaxConnections - Get max connections
 */
int Network_GetMaxConnections()
{
    return g_network_state.max_connections;
}

/**
 * Network_SetPacketHandler - Set packet handler callback
 */
void Network_SetPacketHandler(void (*handler)(void *session, const void *packet, int length))
{
    // Stub: store handler (not implemented in stub)
    (void)handler;
}