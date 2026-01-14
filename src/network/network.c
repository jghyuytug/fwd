/*
 * Network Module - Stub Implementation
 *
 * Provides pure C implementations for all network layer functions.
 * This is a stub implementation that provides the interface for compilation.
 *
 * Contains implementations for:
 * - CNetwork (19 functions) - Connection manager
 * - CEpoll (5 functions) - Event loop
 * - CSession (16 functions) - Session handling
 * - CSessionManager (7 functions) - Session management
 * - PacketBuf (50 functions) - Packet buffer operations
 *
 * Total: 97 function implementations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Include network headers
#include "network/headers/network_interface.h"
#include "network/headers/CNetwork.h"
#include "network/headers/CEpoll.h"
#include "network/headers/CSession.h"
#include "security/headers/CHackAnalyzer.h"  // for PacketBuf and tagPacketHeader
#include "network/headers/PacketDispatcher.h"  // for packet dispatch
#include "network/headers/session_state.h"  // for session state machine
#include "network/headers/CSessionManager.h"
#include "network/headers/PacketBuf.h"
#include "network/headers/packet_types.h"
#include "network/headers/packet_enums.h"

// ========================================================================
// Module-Level Type Definitions and Global State
// ========================================================================

// 会话数据结构（完整版 - 2025-11-11）
#define RECV_BUFFER_SIZE (64 * 1024)  // 64KB接收缓冲区
#define SEND_BUFFER_SIZE (64 * 1024)  // 64KB发送缓冲区

typedef struct SessionData {
    int socket_fd;                      // Socket文件描述符

    // 接收缓冲区
    char recv_buffer[RECV_BUFFER_SIZE]; // 接收缓冲区
    int recv_size;                      // 当前接收的数据大小

    // 发送缓冲区
    char send_buffer[SEND_BUFFER_SIZE]; // 发送缓冲区
    int send_size;                      // 待发送的数据大小
    int send_pos;                       // 已发送的位置

    // 连接信息
    struct sockaddr_in addr;            // 客户端地址
    int connected;                      // 是否已连接（1=已连接，0=未连接）

    // 统计信息（保留兼容性）
    unsigned long bytes_received;       // 总接收字节数
    unsigned long bytes_sent;           // 总发送字节数

    // Week 5: 会话状态机
    SessionStateInfo state_info;        // 状态、认证、统计信息
} SessionData;

// 会话池
typedef struct {
    SessionData sessions[4096];         // 最多4096个会话
    int session_count;                  // 当前会话数
    pthread_mutex_t lock;               // 互斥锁
} SessionPool;

// Simple network connection pool structure (保留兼容性)
typedef struct {
    int socket_fd;
    int port;
    struct sockaddr_in addr;
    int status;  // 0=inactive, 1=active, 2=error
} NetworkConnection;

typedef struct {
    NetworkConnection connections[4096];
    int connection_count;
    int active_connections;
    pthread_mutex_t lock;
} NetworkPool;

// Global instances
static NetworkPool g_network_pool = {0};
static SessionPool g_session_pool = {0};  // 新增：会话池
static int g_epoll_fd = -1;

// ========================================================================
// 会话管理辅助函数（新增 - 2025-11-11）
// ========================================================================

/**
 * Session_FindByFd - 通过fd查找会话
 * @param fd socket文件描述符
 * @return SessionData指针，未找到返回NULL
 */
static SessionData* Session_FindByFd(int fd)
{
    int i;
    for (i = 0; i < g_session_pool.session_count; i++) {
        if (g_session_pool.sessions[i].socket_fd == fd &&
            g_session_pool.sessions[i].connected) {
            return &g_session_pool.sessions[i];
        }
    }
    return NULL;
}

/**
 * Session_GetStateInfo - Helper function for session_state.c
 * Returns pointer to SessionStateInfo for external access
 */
SessionStateInfo* Session_GetStateInfo(int fd)
{
    SessionData *session = Session_FindByFd(fd);
    if (!session) {
        return NULL;
    }
    return &session->state_info;
}

/**
 * Session_GetPoolLock - Helper function for session_state.c
 * Returns pointer to session pool mutex
 */
pthread_mutex_t* Session_GetPoolLock(void)
{
    return &g_session_pool.lock;
}

/**
 * Session_GetAllSessions - Helper for iterating all sessions
 * Returns session array and count (for timeout checking)
 */
void Session_GetAllSessions(SessionData **sessions, int *count)
{
    if (sessions) *sessions = g_session_pool.sessions;
    if (count) *count = g_session_pool.session_count;
}

/* Week 6: Helper functions for session_timer.c */
SessionData* Session_GetByIndex(int index)
{
    if (index < 0 || index >= 4096) {  /* 最多4096个会话 */
        return NULL;
    }
    return &g_session_pool.sessions[index];
}

int Session_GetSocketFd(SessionData *session)
{
    if (!session) return -1;
    return session->socket_fd;
}

int Session_IsConnected(SessionData *session)
{
    if (!session) return 0;
    return session->connected;
}

/**
 * Session_Create - 创建新会话
 * @param fd socket文件描述符
 * @param addr 客户端地址
 * @return SessionData指针，失败返回NULL
 */
static SessionData* Session_Create(int fd, struct sockaddr_in *addr)
{
    pthread_mutex_lock(&g_session_pool.lock);

    if (g_session_pool.session_count >= 4096) {
        pthread_mutex_unlock(&g_session_pool.lock);
        fprintf(stderr, "[Session] ERROR: Session pool full\n");
        return NULL;
    }

    // 找到空闲槽位
    SessionData *session = NULL;
    int i;
    for (i = 0; i < 4096; i++) {
        if (!g_session_pool.sessions[i].connected) {
            session = &g_session_pool.sessions[i];
            break;
        }
    }

    if (!session) {
        pthread_mutex_unlock(&g_session_pool.lock);
        return NULL;
    }

    // 初始化会话
    memset(session, 0, sizeof(SessionData));
    session->socket_fd = fd;
    if (addr) {
        memcpy(&session->addr, addr, sizeof(struct sockaddr_in));
    }
    session->connected = 1;

    // Week 5: 初始化状态机
    time_t current_time = time(NULL);
    session->state_info.state = SESSION_STATE_CONNECTED;
    session->state_info.state_enter_time = current_time;
    session->state_info.timeout_seconds = SESSION_TIMEOUT_HANDSHAKE;

    // 初始化统计信息
    session->state_info.stats.connect_time = current_time;
    session->state_info.stats.last_activity = current_time;
    session->state_info.stats.last_heartbeat = current_time;
    session->state_info.stats.packets_received = 0;
    session->state_info.stats.packets_sent = 0;
    session->state_info.stats.bytes_received = 0;
    session->state_info.stats.bytes_sent = 0;

    // 初始化认证信息
    session->state_info.auth.authenticated = 0;
    session->state_info.auth.account_id = 0;
    session->state_info.auth.character_id = 0;
    session->state_info.auth.privilege_level = 0;
    memset(session->state_info.auth.username, 0, sizeof(session->state_info.auth.username));
    if (addr) {
        inet_ntop(AF_INET, &addr->sin_addr, session->state_info.auth.ip_address,
                  sizeof(session->state_info.auth.ip_address));
    }

    g_session_pool.session_count++;

    pthread_mutex_unlock(&g_session_pool.lock);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
    printf("[Session] Created session for fd=%d (%s:%d) [%d/%d] state=%s\n",
           fd, ip, ntohs(addr->sin_port),
           g_session_pool.session_count, 4096,
           Session_GetStateName(session->state_info.state));

    return session;
}

/**
 * Session_Destroy - 销毁会话
 * @param fd socket文件描述符
 */
static void Session_Destroy(int fd)
{
    pthread_mutex_lock(&g_session_pool.lock);

    SessionData *session = Session_FindByFd(fd);
    if (session) {
        printf("[Session] Destroying session for fd=%d (recv=%lu, sent=%lu)\n",
               fd, session->bytes_received, session->bytes_sent);

        session->connected = 0;
        session->socket_fd = -1;
        g_session_pool.session_count--;
    }

    pthread_mutex_unlock(&g_session_pool.lock);
}

/**
 * Session_Close - 关闭并销毁会话（Week 6）
 * @param fd socket文件描述符
 * @return 0=success, -1=error
 */
int Session_Close(int fd)
{
    if (fd <= 0) {
        return -1;
    }

    printf("[Session] Closing session fd=%d\n", fd);

    /* 从epoll中注销 */
    if (g_epoll_fd != -1) {
        epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    }

    /* 关闭socket */
    close(fd);

    /* 销毁会话数据 */
    Session_Destroy(fd);

    return 0;
}

/**
 * Session_QueueSend - 将数据加入发送队列
 *
 * 实现说明 (2025-11-11):
 * - 将数据复制到会话的发送缓冲区
 * - 如果缓冲区之前为空，注册EPOLLOUT事件
 * - 边缘触发模式下，只在有新数据时才需要重新注册
 *
 * @param fd 会话的socket文件描述符
 * @param data 要发送的数据指针
 * @param len 数据长度
 * @return 成功返回1，失败返回0
 */
_BOOL4 Session_QueueSend(int fd, const char *data, int len)
{
    if (!data || len <= 0) {
        return 0;
    }

    pthread_mutex_lock(&g_session_pool.lock);

    SessionData *session = Session_FindByFd(fd);
    if (!session) {
        pthread_mutex_unlock(&g_session_pool.lock);
        fprintf(stderr, "[Session] Session not found for fd=%d\n", fd);
        return 0;
    }

    // 检查发送缓冲区是否有足够空间
    int available_space = SEND_BUFFER_SIZE - session->send_size;
    if (len > available_space) {
        pthread_mutex_unlock(&g_session_pool.lock);
        fprintf(stderr, "[Session] Send buffer full for fd=%d (need=%d, available=%d)\n",
                fd, len, available_space);
        return 0;
    }

    // 复制数据到发送缓冲区
    memcpy(session->send_buffer + session->send_size, data, len);
    session->send_size += len;

    // Week 5: 更新发送数据包计数（假设每次QueueSend都是一个完整数据包）
    session->state_info.stats.packets_sent++;

    printf("[Session] Queued %d bytes for send on fd=%d (total_queued=%d)\n",
           len, fd, session->send_size);

    // 如果这是第一次加入数据（send_pos == 0 && send_size刚变成非0）
    // 需要注册EPOLLOUT事件，让epoll通知我们可以发送
    int need_register_epollout = (session->send_size == len);  // 刚才缓冲区是空的

    pthread_mutex_unlock(&g_session_pool.lock);

    if (need_register_epollout) {
        // 注册EPOLLOUT事件（边缘触发模式）
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
        event.data.ptr = (void*)(intptr_t)fd;

        if (epoll_ctl(g_epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0) {
            perror("[Session] Failed to register EPOLLOUT");
            return 0;
        }

        printf("[Session] Registered EPOLLOUT for fd=%d\n", fd);
    }

    return 1;
}

// ========================================================================
// CNetwork Implementation (19 functions)
// ========================================================================

/**
 * CNetwork_CNetwork - Constructor
 * Initialize network connection manager
 */
void __cdecl CNetwork_CNetwork(void *this, int a1)
{
    // Initialize network pool
    memset(&g_network_pool, 0, sizeof(NetworkPool));
    pthread_mutex_init(&g_network_pool.lock, NULL);

    // Initialize epoll
    if (g_epoll_fd == -1) {
        g_epoll_fd = epoll_create(4096);
    }
}

/**
 * CNetwork_destructor - Destructor
 * Clean up network resources
 */
void __cdecl CNetwork_destructor(void *this)
{
    int i;
    // Close all connections
    for (i = 0; i < g_network_pool.connection_count; i++) {
        if (g_network_pool.connections[i].socket_fd > 0) {
            close(g_network_pool.connections[i].socket_fd);
        }
    }

    // Close epoll
    if (g_epoll_fd > 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }

    pthread_mutex_destroy(&g_network_pool.lock);
}

/**
 * CNetwork_disconnect - Disconnect a client connection
 */
int __cdecl CNetwork_disconnect(void *this, int a1)
{
    // Stub: Mark connection as inactive
    if (a1 >= 0 && a1 < g_network_pool.connection_count) {
        pthread_mutex_lock(&g_network_pool.lock);
        if (g_network_pool.connections[a1].socket_fd > 0) {
            close(g_network_pool.connections[a1].socket_fd);
            g_network_pool.connections[a1].socket_fd = -1;
            g_network_pool.connections[a1].status = 0;
            g_network_pool.active_connections--;
        }
        pthread_mutex_unlock(&g_network_pool.lock);
        return 0;
    }
    return -1;
}

/**
 * CNetwork_set_socket - Set socket descriptor for connection
 */
int __cdecl CNetwork_set_socket(void *this, int *a1, int a2, char a3)
{
    // Stub: Set socket for connection pool
    return 0;
}

/**
 * CNetwork_SetNonBlock - Set socket to non-blocking mode
 *
 * 实现说明 (2025-11-11):
 * - 使用fcntl设置O_NONBLOCK标志
 * - 非阻塞模式对epoll边缘触发必不可少
 * - 返回值：1=成功，0=失败
 */
_BOOL4 __cdecl CNetwork_SetNonBlock(void *this, int fd)
{
    if (fd < 0) {
        fprintf(stderr, "[CNetwork] ERROR: invalid fd: %d\n", fd);
        return 0;
    }

    // 获取当前socket标志
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("[CNetwork] fcntl F_GETFL failed");
        return 0;
    }

    // 设置O_NONBLOCK标志
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("[CNetwork] fcntl F_SETFL O_NONBLOCK failed");
        return 0;
    }

    printf("[CNetwork] Set socket %d to non-blocking mode\n", fd);
    return 1;  // Success
}

/**
 * CNetwork_GetPeerIP2 - Get peer IP address
 */
void *__cdecl CNetwork_GetPeerIP2(void *this, int *a1, void *s, int n)
{
    // Stub: Retrieve peer IP from socket
    return NULL;
}

/**
 * CNetwork_get_str_ip - Get string representation of IP
 */
int __cdecl CNetwork_get_str_ip(void *this, int a1)
{
    // Stub: Convert IP to string
    return 0;
}

/**
 * CNetwork_get_socket - Get socket descriptor
 */
int __cdecl CNetwork_get_socket(void *this, int a1)
{
    // Stub: Return socket file descriptor
    if (a1 >= 0 && a1 < g_network_pool.connection_count) {
        return g_network_pool.connections[a1].socket_fd;
    }
    return -1;
}

/**
 * CNetwork_get_ip - Get IP address as integer
 */
int __cdecl CNetwork_get_ip(void *this, int a1)
{
    // Stub: Return IP as integer
    if (a1 >= 0 && a1 < g_network_pool.connection_count) {
        return (int)g_network_pool.connections[a1].addr.sin_addr.s_addr;
    }
    return 0;
}

/**
 * CNetwork_get_port - Get port number
 */
int __cdecl CNetwork_get_port(void *this, int a1)
{
    // Stub: Return port number
    if (a1 >= 0 && a1 < g_network_pool.connection_count) {
        return ntohs(g_network_pool.connections[a1].addr.sin_port);
    }
    return 0;
}

/**
 * CNetwork_get_inner_ip - Get internal IP address
 */
int __cdecl CNetwork_get_inner_ip(void *this, int a1)
{
    // Stub: Return internal IP
    return 0;
}

/**
 * CNetwork_get_mtu - Get maximum transmission unit
 */
int __cdecl CNetwork_get_mtu(void *this, int a1)
{
    // Stub: Return MTU size
    return 1500;  // Standard Ethernet MTU
}

/**
 * CNetwork_get_nat_type - Get NAT type
 */
int __cdecl CNetwork_get_nat_type(void *this, int a1)
{
    // Stub: Return NAT type
    return 0;  // No NAT
}

/**
 * CNetwork_set_nat_v1 - Set NAT with int parameter
 */
char *__cdecl CNetwork_set_nat_v1(void *this, int a1, char a2, int a3, int a4, __int16 a5, int a6)
{
    // Stub: Configure NAT
    return NULL;
}

/**
 * CNetwork_set_nat_v2 - Set NAT with string parameter
 */
char *__cdecl CNetwork_set_nat_v2(void *this, int a1, char a2, int a3, int a4, __int16 a5, char *src)
{
    // Stub: Configure NAT with string
    return NULL;
}

/**
 * CNetwork_get_mac_addr - Get MAC address
 */
int __cdecl CNetwork_get_mac_addr(void *this, int a1)
{
    // Stub: Return MAC address
    return 0;
}

/**
 * CNetwork_send_packet_v1 - Send packet with default parameters
 */
int __cdecl CNetwork_send_packet_v1(void *this, int a1)
{
    // Stub: Send packet
    return 0;
}

/**
 * CNetwork_send_packet_v2 - Send packet with source data
 */
int __cdecl CNetwork_send_packet_v2(void *this, int a1, char *src, int n)
{
    // Stub: Send packet with data
    if (a1 >= 0 && a1 < g_network_pool.connection_count) {
        int fd = g_network_pool.connections[a1].socket_fd;
        if (fd > 0 && src && n > 0) {
            // Attempt to send data
            ssize_t sent = send(fd, src, n, MSG_DONTWAIT);
            return (sent > 0) ? sent : 0;
        }
    }
    return 0;
}

/**
 * CNetwork_recv_packet - Receive packet
 */
int __cdecl CNetwork_recv_packet(void *this, int a1)
{
    // Stub: Receive packet
    return 0;
}

/**
 * CNetwork_send_lock - Acquire send lock
 */
int __cdecl CNetwork_send_lock(void *this, int a1)
{
    // Stub: Lock send operation
    return 0;
}

/**
 * CNetwork_send_unlock - Release send lock
 */
int __cdecl CNetwork_send_unlock(void *this, int a1)
{
    // Stub: Unlock send operation
    return 0;
}

/**
 * CNetwork_initNetworkBuffer - Initialize network buffer
 */
void *__cdecl CNetwork_initNetworkBuffer(void *this, _DWORD *a1)
{
    // Stub: Initialize buffer
    return NULL;
}

/**
 * CNetwork_GetRemainSendSize - Get remaining send buffer size
 */
int __cdecl CNetwork_GetRemainSendSize(void *this, int a1)
{
    // Stub: Return remaining buffer size
    return 65536;  // Default buffer size
}

// ========================================================================
// CEpoll Implementation (5 functions)
// ========================================================================

/**
 * CEpoll_CEpoll - Constructor
 * Initialize epoll event loop
 */
void __cdecl CEpoll_CEpoll(void *this, void *a1)
{
    // Create epoll instance
    if (g_epoll_fd == -1) {
        g_epoll_fd = epoll_create(4096);
    }
}

/**
 * CEpoll_destructor - Destructor
 * Clean up epoll resources
 */
void __cdecl CEpoll_destructor(void *this)
{
    // Close epoll instance
    if (g_epoll_fd > 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }
}

/**
 * CEpoll_WaitForEvent - Wait for epoll events
 *
 * 实现说明 (2025-11-11):
 * - 实际调用epoll_wait等待事件
 * - 返回就绪的事件数量
 * - timeout: 超时时间（毫秒），-1表示无限等待
 *
 * @param this CEpoll实例指针
 * @param event 事件数组，用于接收就绪事件
 * @param timeout 超时时间（毫秒）
 * @return 就绪事件数量，-1表示错误
 */
int __cdecl CEpoll_WaitForEvent(void *this, struct epoll_event *event, int timeout)
{
    if (g_epoll_fd < 0) {
        fprintf(stderr, "[CEpoll] ERROR: epoll_fd not initialized\n");
        return -1;
    }

    // 实际调用epoll_wait等待事件
    // 注意：这里event应该是一个数组，但函数签名只传了指针
    // 我们假设调用者传入的是足够大的数组
    int n = epoll_wait(g_epoll_fd, event, 1024, timeout);

    if (n < 0) {
        if (errno == EINTR) {
            // 被信号中断，不是错误
            return 0;
        }
        perror("[CEpoll] epoll_wait failed");
        return -1;
    }

    return n;
}

/**
 * CEpoll_RegisterSession - Register session with epoll
 *
 * 实现说明 (2025-11-11):
 * - 将session的socket注册到epoll进行监听
 * - 使用边缘触发模式（EPOLLET）提高性能
 * - 监听EPOLLIN（可读）和EPOLLRDHUP（对端关闭）事件
 *
 * @param this CEpoll实例指针
 * @param event epoll_event结构，将被填充
 * @param session CSession实例指针
 * @param socket_fd socket文件描述符
 * @return true=成功，false=失败
 */
bool __cdecl CEpoll_RegisterSession(void *this, struct epoll_event *event, void *session, int socket_fd)
{
    if (g_epoll_fd < 0) {
        fprintf(stderr, "[CEpoll] ERROR: epoll_fd not initialized\n");
        return 0;
    }

    if (socket_fd < 0) {
        fprintf(stderr, "[CEpoll] ERROR: invalid socket_fd: %d\n", socket_fd);
        return 0;
    }

    // 配置epoll事件
    event->events = EPOLLIN | EPOLLET | EPOLLRDHUP;  // 边缘触发 + 可读 + 对端关闭
    event->data.ptr = session;  // 保存session指针，事件触发时可以找到对应的session

    // 注册到epoll
    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, socket_fd, event) < 0) {
        // 如果失败，可能是socket已经注册过了，尝试修改
        if (errno == EEXIST) {
            if (epoll_ctl(g_epoll_fd, EPOLL_CTL_MOD, socket_fd, event) < 0) {
                perror("[CEpoll] epoll_ctl MOD failed");
                return 0;
            }
            printf("[CEpoll] Modified existing socket %d in epoll\n", socket_fd);
        } else {
            perror("[CEpoll] epoll_ctl ADD failed");
            return 0;
        }
    } else {
        printf("[CEpoll] Registered socket %d to epoll (session=%p)\n", socket_fd, session);
    }

    return 1;
}

/**
 * CEpoll_UnregisterSession - Unregister session from epoll
 *
 * 实现说明 (2025-11-11):
 * - 从epoll中移除socket监听
 * - 通常在连接关闭时调用
 *
 * @param this CEpoll实例指针
 * @param event epoll_event结构（未使用）
 * @param socket_fd socket文件描述符
 * @return true=成功，false=失败
 */
bool __cdecl CEpoll_UnregisterSession(void *this, void *a1, int socket_fd)
{
    if (g_epoll_fd < 0) {
        return 1;  // epoll未初始化，直接返回成功
    }

    if (socket_fd < 0) {
        return 1;  // 无效的socket，直接返回成功
    }

    // 从epoll中移除
    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL) < 0) {
        if (errno != ENOENT) {  // ENOENT表示socket本来就不在epoll中，不算错误
            perror("[CEpoll] epoll_ctl DEL failed");
            return 0;
        }
    } else {
        printf("[CEpoll] Unregistered socket %d from epoll\n", socket_fd);
    }

    return 1;
}

// ========================================================================
// CSession Implementation (16 functions)
// ========================================================================

/**
 * CSession_GetChannelNo - Get channel number from session
 */
int __cdecl CSession_GetChannelNo(void *self)
{
    // Stub: Return channel number
    return 0;
}

/**
 * CSession_GetSocket - Get socket file descriptor from session
 */
int __cdecl CSession_GetSocket(void *self)
{
    // Stub: Return socket FD
    return -1;
}

/**
 * CSession_GetTriggerSessionEventType - Get trigger event type
 */
int __cdecl CSession_GetTriggerSessionEventType(void *self)
{
    // Stub: Return event type
    return 0;
}

/**
 * CSession_SetTriggerSessionEventType - Set trigger event type
 */
void __cdecl CSession_SetTriggerSessionEventType(void *self, int type)
{
    // Stub: Set event type
}

/**
 * CSession_AddTriggerSessionEventType - Add trigger event type (bitwise OR)
 */
void __cdecl CSession_AddTriggerSessionEventType(void *self, int type)
{
    // Stub: Add event type flags
}

/**
 * CSession_Parsing - Parse incoming packet data
 *
 * 实现说明 (2025-11-11):
 * - 从会话接收缓冲区中解析完整的数据包
 * - DNF数据包格式：4字节头部（length+type） + 数据
 * - 支持粘包处理：一次可能接收多个数据包
 * - 移除已处理的数据，保留未完成的数据包
 *
 * @param fd_ptr socket文件描述符（转换为void*）
 * @param unused 未使用参数
 * @return 解析的数据包数量，-1表示错误
 */
int __cdecl CSession_Parsing(void *fd_ptr, char *unused)
{
    int fd = (int)(intptr_t)fd_ptr;
    int parsed_count = 0;

    // 查找会话
    SessionData *session = Session_FindByFd(fd);
    if (!session) {
        fprintf(stderr, "[CSession_Parsing] ERROR: Session not found for fd=%d\n", fd);
        return -1;
    }

    // 循环处理缓冲区中的所有完整数据包
    while (session->recv_size > 0) {
        // 至少需要4字节头部
        if (session->recv_size < sizeof(tagPacketHeader)) {
            // 数据不足，等待更多数据
            break;
        }

        // 读取数据包头部
        tagPacketHeader *header = (tagPacketHeader*)session->recv_buffer;
        uint16_t packet_length = header->length;
        uint16_t packet_type = header->type;

        // 验证数据包长度（最小4字节，最大64KB）
        if (packet_length < sizeof(tagPacketHeader) || packet_length > 65535) {
            fprintf(stderr, "[CSession_Parsing] ERROR: Invalid packet length=%d from fd=%d\n",
                    packet_length, fd);
            return -1;  // 协议错误，关闭连接
        }

        // 检查是否接收到完整的数据包
        if (session->recv_size < packet_length) {
            // 数据包不完整，等待更多数据
            printf("[CSession_Parsing] Incomplete packet: have=%d, need=%d, waiting...\n",
                   session->recv_size, packet_length);
            break;
        }

        // 完整的数据包已接收！
        printf("[CSession_Parsing] Complete packet received: type=%d, length=%d\n",
               packet_type, packet_length);

        // 打印数据包内容（调试用，前32字节）
        int print_len = (packet_length < 32) ? packet_length : 32;
        printf("[CSession_Parsing] Packet data (hex): ");
        int i;
        for (i = 0; i < print_len; i++) {
            printf("%02x ", (unsigned char)session->recv_buffer[i]);
        }
        if (packet_length > 32) printf("...");
        printf("\n");

        // 创建PacketBuf结构用于分发
        PacketBuf packet_buf;
        packet_buf.data = session->recv_buffer;
        packet_buf.size = packet_length;
        packet_buf.capacity = RECV_BUFFER_SIZE;
        packet_buf.read_pos = 0;
        packet_buf.write_pos = packet_length;
        packet_buf.finalized = 1;
        packet_buf.encrypted = 0;
        packet_buf.compressed = 0;
        packet_buf._reserved = 0;
        packet_buf.header = session->recv_buffer;

        // 分发数据包到处理器
        int dispatch_result = PacketDispatcher_Dispatch(fd, &packet_buf);
        if (dispatch_result < 0) {
            fprintf(stderr, "[CSession_Parsing] Handler returned error, closing connection fd=%d\n", fd);
            return -1;  // 处理器错误，关闭连接
        }

        // Week 5: 更新接收数据包计数
        session->state_info.stats.packets_received++;

        // 移除已处理的数据包
        int remaining = session->recv_size - packet_length;
        if (remaining > 0) {
            // 将剩余数据移到缓冲区开头
            memmove(session->recv_buffer, session->recv_buffer + packet_length, remaining);
        }
        session->recv_size = remaining;

        parsed_count++;

        // 继续处理下一个数据包（如果有）
    }

    if (parsed_count > 0) {
        printf("[CSession_Parsing] Parsed %d packet(s) from fd=%d, remaining=%d bytes\n",
               parsed_count, fd, session->recv_size);
    }

    return parsed_count;
}

/**
 * CSession_OnRecv - Handle receive event
 *
 * 实现说明 (2025-11-11):
 * - 从socket接收数据到缓冲区
 * - 边缘触发模式：循环recv直到EAGAIN
 * - 返回true=成功，false=连接关闭或错误
 *
 * @param fd_ptr socket文件描述符（作为void*传入）
 * @return true=成功，false=需要关闭连接
 */
_BOOL4 __cdecl CSession_OnRecv(void *fd_ptr)
{
    int fd = (int)(intptr_t)fd_ptr;

    // 查找会话
    SessionData *session = Session_FindByFd(fd);
    if (!session) {
        fprintf(stderr, "[CSession] ERROR: Session not found for fd=%d\n", fd);
        return 0;
    }

    // 边缘触发模式：循环接收直到EAGAIN
    while (1) {
        // 检查接收缓冲区是否已满
        if (session->recv_size >= RECV_BUFFER_SIZE) {
            fprintf(stderr, "[CSession] WARNING: Recv buffer full for fd=%d, need to process data\n", fd);
            /*
             * 缓冲区满时的处理策略:
             * 1. 返回1表示暂停接收，等待数据包处理
             * 2. 主循环应调用PacketDispatcher_ProcessSession()处理积压数据包
             * 3. 处理完成后空间释放，恢复接收
             *
             * 注意: 恶意客户端可能发送大量无效数据导致缓冲区满
             * 生产环境应增加: 检测无效数据包计数，超限则断开连接
             */
            return 1;
        }

        // 计算可接收的空间
        int space = RECV_BUFFER_SIZE - session->recv_size;
        char *recv_ptr = session->recv_buffer + session->recv_size;

        // 接收数据
        ssize_t n = recv(fd, recv_ptr, space, 0);

        if (n > 0) {
            // 成功接收到数据
            session->recv_size += n;
            session->bytes_received += n;

            // Week 5: 更新活动时间（统计字段）
            session->state_info.stats.bytes_received = session->bytes_received;
            Session_UpdateActivity(fd);

            printf("[CSession] Received %zd bytes from fd=%d (total in buffer: %d bytes, total received: %lu bytes)\n",
                   n, fd, session->recv_size, session->bytes_received);

            // 打印接收到的数据（十六进制，前32字节）
            if (n > 0) {
                int i;
                int print_len = (n < 32) ? n : 32;
                printf("[CSession] Data (hex): ");
                for (i = 0; i < print_len; i++) {
                    printf("%02x ", (unsigned char)recv_ptr[i]);
                }
                if (n > 32) printf("...");
                printf("\n");
            }

            // 如果接收的数据小于请求的大小，说明没有更多数据了
            if (n < space) {
                // 没有更多数据了，退出循环
                break;
            }

            // 继续接收
            continue;

        } else if (n == 0) {
            // 连接关闭
            printf("[CSession] Connection closed by peer (fd=%d)\n", fd);
            return 0;

        } else {
            // n < 0，错误
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 没有更多数据了（非阻塞模式下正常）
                break;
            } else if (errno == EINTR) {
                // 被信号中断，继续接收
                continue;
            } else {
                // 真正的错误
                perror("[CSession] recv() failed");
                return 0;
            }
        }
    }

    // 接收完成，解析接收缓冲区中的数据包
    if (session->recv_size > 0) {
        int parsed = CSession_Parsing((void*)(intptr_t)fd, NULL);
        if (parsed < 0) {
            // 解析错误（协议错误），关闭连接
            fprintf(stderr, "[CSession] Parsing error, closing connection fd=%d\n", fd);
            return 0;
        }
    }

    return 1;  // 成功
}

/**
 * CSession_OnSend - 处理发送事件（边缘触发模式）
 *
 * 实现说明 (2025-11-11):
 * - 从会话的发送缓冲区发送数据到socket
 * - 边缘触发模式：循环send直到EAGAIN或缓冲区清空
 * - 发送完毕后取消EPOLLOUT注册（避免忙轮询）
 * - 使用send_pos跟踪已发送的位置
 *
 * @param fd_ptr socket文件描述符（转换为void*）
 * @return 成功返回1，失败返回0
 */
_BOOL4 __cdecl CSession_OnSend(void *fd_ptr)
{
    int fd = (int)(intptr_t)fd_ptr;

    // 查找会话
    pthread_mutex_lock(&g_session_pool.lock);
    SessionData *session = Session_FindByFd(fd);
    if (!session) {
        pthread_mutex_unlock(&g_session_pool.lock);
        fprintf(stderr, "[CSession] ERROR: Session not found for fd=%d\n", fd);
        return 0;
    }

    // 边缘触发模式：循环发送直到EAGAIN或缓冲区清空
    while (session->send_size > session->send_pos) {
        // 计算剩余要发送的数据量
        int remaining = session->send_size - session->send_pos;
        char *send_ptr = session->send_buffer + session->send_pos;

        // 发送数据
        ssize_t n = send(fd, send_ptr, remaining, MSG_NOSIGNAL);  // MSG_NOSIGNAL避免SIGPIPE

        if (n > 0) {
            // 成功发送了n字节
            session->send_pos += n;
            session->bytes_sent += n;

            // Week 5: 更新统计字段
            session->state_info.stats.bytes_sent = session->bytes_sent;

            printf("[CSession] Sent %zd bytes to fd=%d (progress: %d/%d, total sent: %lu bytes)\n",
                   n, fd, session->send_pos, session->send_size, session->bytes_sent);

            // 如果发送的数据小于请求的大小，说明发送缓冲区满了
            if (n < remaining) {
                // 发送缓冲区满，等待下次EPOLLOUT事件
                pthread_mutex_unlock(&g_session_pool.lock);
                return 1;
            }

            // 继续发送剩余数据
            continue;

        } else if (n == 0) {
            // send返回0通常不会发生，但如果发生了就当作错误
            fprintf(stderr, "[CSession] WARNING: send() returned 0 for fd=%d\n", fd);
            pthread_mutex_unlock(&g_session_pool.lock);
            return 0;

        } else {
            // n < 0，错误
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送缓冲区满，等待下次EPOLLOUT事件
                pthread_mutex_unlock(&g_session_pool.lock);
                return 1;
            } else if (errno == EINTR) {
                // 被信号中断，继续发送
                continue;
            } else {
                // 真正的错误
                perror("[CSession] send() failed");
                pthread_mutex_unlock(&g_session_pool.lock);
                return 0;
            }
        }
    }

    // 所有数据发送完毕，重置发送缓冲区
    session->send_size = 0;
    session->send_pos = 0;

    printf("[CSession] All data sent on fd=%d, resetting send buffer\n", fd);

    pthread_mutex_unlock(&g_session_pool.lock);

    // 取消EPOLLOUT注册（避免忙轮询）
    // 只保留EPOLLIN | EPOLLET | EPOLLRDHUP
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    event.data.ptr = (void*)(intptr_t)fd;

    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0) {
        perror("[CSession] Failed to unregister EPOLLOUT");
        return 0;
    }

    printf("[CSession] Unregistered EPOLLOUT for fd=%d\n", fd);

    return 1;  // 成功
}

/**
 * CSession_OnError - Handle error event
 */
int __cdecl CSession_OnError(void *self)
{
    // Stub: Handle error
    return 0;
}

/**
 * CSession_OnAccept - Handle accept event
 *
 * 实现说明 (2025-11-11):
 * - 从监听socket接受新连接
 * - 设置新连接为非阻塞模式
 * - 打印客户端地址信息
 * - 返回新连接的socket fd
 *
 * 注意：在epoll边缘触发模式下，需要在循环中accept直到EAGAIN
 *
 * @param listen_fd 监听socket的文件描述符（通过self传入）
 * @return 新连接的socket fd（作为int*），失败返回NULL
 */
int *__cdecl CSession_OnAccept(void *listen_fd_ptr)
{
    // 将void*转换回int（listen socket的fd）
    int listen_fd = (int)(intptr_t)listen_fd_ptr;

    if (listen_fd < 0) {
        fprintf(stderr, "[CSession] ERROR: invalid listen_fd\n");
        return NULL;
    }

    // accept新连接
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 没有更多连接了（非阻塞模式下正常）
            return NULL;
        }
        perror("[CSession] accept() failed");
        return NULL;
    }

    // 获取客户端IP和端口
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);

    printf("[CSession] New connection from %s:%d (fd=%d)\n", client_ip, client_port, client_fd);

    // 设置新连接为非阻塞模式
    if (!CNetwork_SetNonBlock(NULL, client_fd)) {
        fprintf(stderr, "[CSession] Failed to set client socket to non-blocking\n");
        close(client_fd);
        return NULL;
    }

    // 设置TCP_NODELAY（禁用Nagle算法，减少延迟）
    int nodelay = 1;
    if (setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
        perror("[CSession] setsockopt TCP_NODELAY failed (non-fatal)");
        // 不是致命错误，继续
    }

    // 创建会话数据结构
    SessionData *session = Session_Create(client_fd, &client_addr);
    if (!session) {
        fprintf(stderr, "[CSession] Failed to create session for fd=%d\n", client_fd);
        close(client_fd);
        return NULL;
    }

    // 返回新连接的fd（转换为int*）
    return (int*)(intptr_t)client_fd;
}

/**
 * CSession_OnConnect - Handle connect event
 */
int __cdecl CSession_OnConnect(void *self)
{
    // Stub: Handle connect
    return 0;
}

/**
 * CSession_Send - Send packet to client
 */
_BOOL4 __cdecl CSession_Send(void *self, void **a2)
{
    // Stub: Send packet
    return 1;
}

/**
 * CSession_Listen - Start listening on address/port
 *
 * 实现说明 (2025-11-11):
 * - 创建TCP socket并绑定到指定地址和端口
 * - 开始监听连接（listen）
 * - 设置socket为非阻塞模式
 * - 设置SO_REUSEADDR选项
 *
 * @param self CSession实例指针
 * @param address 地址字符串，格式："0.0.0.0:7001"
 * @return socket指针（实际上是int*），失败返回NULL
 */
int *__cdecl CSession_Listen(void *self, char *address)
{
    if (!address) {
        fprintf(stderr, "[CSession] ERROR: address is NULL\n");
        return NULL;
    }

    // 解析地址和端口（格式：IP:PORT）
    char ip[64] = {0};
    int port = 0;
    char *colon = strchr(address, ':');
    if (colon) {
        int ip_len = colon - address;
        if (ip_len >= sizeof(ip)) ip_len = sizeof(ip) - 1;
        strncpy(ip, address, ip_len);
        ip[ip_len] = '\0';
        port = atoi(colon + 1);
    } else {
        fprintf(stderr, "[CSession] ERROR: invalid address format: %s (expected IP:PORT)\n", address);
        return NULL;
    }

    printf("[CSession] Starting listen on %s:%d\n", ip, port);

    // 创建TCP socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("[CSession] socket() failed");
        return NULL;
    }

    // 设置SO_REUSEADDR，允许快速重启服务器
    int reuse = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("[CSession] setsockopt SO_REUSEADDR failed");
        close(listen_fd);
        return NULL;
    }

    // 设置非阻塞模式
    if (!CNetwork_SetNonBlock(NULL, listen_fd)) {
        fprintf(stderr, "[CSession] Failed to set non-blocking mode\n");
        close(listen_fd);
        return NULL;
    }

    // 绑定地址和端口
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 如果IP是"0.0.0.0"，监听所有接口
    if (strcmp(ip, "0.0.0.0") == 0) {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            perror("[CSession] inet_pton() failed");
            close(listen_fd);
            return NULL;
        }
    }

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[CSession] bind() failed");
        close(listen_fd);
        return NULL;
    }

    // 开始监听（backlog=1024）
    if (listen(listen_fd, 1024) < 0) {
        perror("[CSession] listen() failed");
        close(listen_fd);
        return NULL;
    }

    printf("[CSession] Successfully listening on %s:%d (fd=%d)\n", ip, port, listen_fd);

    // 返回socket fd（转换为int*）
    // 注意：这里的返回值类型有点奇怪（int*），但这是反编译后的签名
    // 实际上可能是返回fd值本身，调用者会将其转换回int
    return (int*)(intptr_t)listen_fd;
}

/**
 * CSession_Connect - Connect to remote address
 */
int *__cdecl CSession_Connect(void *self, const char *a2)
{
    // Stub: Connect to address
    return NULL;
}

/**
 * CSession_Disconnect - Close session connection
 */
int __cdecl CSession_Disconnect(void *self)
{
    // Stub: Close connection
    return 0;
}

// ========================================================================
// CSessionManager Implementation (7 functions)
// ========================================================================

/**
 * CSessionManager_CSessionManager - Constructor
 */
void __cdecl CSessionManager_CSessionManager(void *this, void *a1)
{
    // Stub: Initialize session manager
}

/**
 * CSessionManager_destructor - Destructor
 */
void __cdecl CSessionManager_destructor(void *this)
{
    // Stub: Clean up session manager
}

/**
 * CSessionManager_WaitForSessionEvent - Wait for session events
 */
int __cdecl CSessionManager_WaitForSessionEvent(void *this, struct epoll_event *event, int timeout)
{
    // Stub: Wait for events
    return 0;
}

/**
 * CSessionManager_TrySend - Attempt to send packet through session
 */
int __cdecl CSessionManager_TrySend(void *this, void *a1, void *session, void **packet)
{
    // Stub: Try send
    return 0;
}

/**
 * CSessionManager_TryConnect - Attempt to connect to remote address
 */
int __cdecl CSessionManager_TryConnect(void *this, struct epoll_event *event, void *session, char *address)
{
    // Stub: Try connect
    return 0;
}

/**
 * CSessionManager_TryListen - Attempt to listen on address/port
 */
bool __cdecl CSessionManager_TryListen(void *this, struct epoll_event *event, void *session, char *address)
{
    // Stub: Try listen
    return 1;
}

/**
 * CSessionManager_TryDisconnect - Attempt to disconnect session
 */
int __cdecl CSessionManager_TryDisconnect(void *this, void *a1, void *session)
{
    // Stub: Try disconnect
    return 0;
}

// ========================================================================
// PacketBuf Implementation (50 functions)
// ========================================================================

/**
 * PacketBuf_put_byte - Put single byte into packet
 *
 * 实现说明 (2025-11-11):
 * - 将单字节写入数据包的write_pos位置
 * - 自动更新write_pos和size
 * - 检查缓冲区空间
 *
 * @param self PacketBuf指针
 * @param flag 要写入的字节
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_put_byte(PacketBuf *self, char flag)
{
    if (!self || !self->data) {
        return 0;
    }

    // 检查缓冲区空间
    if (self->write_pos + 1 > self->capacity) {
        fprintf(stderr, "[PacketBuf] put_byte: Buffer full (write_pos=%d, capacity=%d)\n",
                self->write_pos, self->capacity);
        return 0;
    }

    // 写入字节
    ((char*)self->data)[self->write_pos] = flag;
    self->write_pos += 1;

    // 更新size（size跟踪实际数据大小）
    if (self->write_pos > self->size) {
        self->size = self->write_pos;
    }

    return 1;
}

/**
 * PacketBuf_put_short - Put 16-bit integer into packet (小端序)
 *
 * 实现说明 (2025-11-11):
 * - 以小端序写入2字节短整数
 * - 自动更新write_pos和size
 *
 * @param self PacketBuf指针
 * @param value 要写入的16位整数
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_put_short(PacketBuf *self, __int16 value)
{
    if (!self || !self->data) {
        return 0;
    }

    // 检查缓冲区空间
    if (self->write_pos + 2 > self->capacity) {
        fprintf(stderr, "[PacketBuf] put_short: Buffer full (write_pos=%d, capacity=%d)\n",
                self->write_pos, self->capacity);
        return 0;
    }

    // 小端序写入（低字节在前）
    unsigned char *ptr = (unsigned char*)self->data + self->write_pos;
    ptr[0] = (unsigned char)(value & 0xFF);
    ptr[1] = (unsigned char)((value >> 8) & 0xFF);

    self->write_pos += 2;

    if (self->write_pos > self->size) {
        self->size = self->write_pos;
    }

    return 1;
}

/**
 * PacketBuf_put_int - Put 32-bit integer into packet (小端序)
 *
 * 实现说明 (2025-11-11):
 * - 以小端序写入4字节整数
 * - 自动更新write_pos和size
 *
 * @param self PacketBuf指针
 * @param value 要写入的32位整数
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_put_int(PacketBuf *self, int value)
{
    if (!self || !self->data) {
        return 0;
    }

    // 检查缓冲区空间
    if (self->write_pos + 4 > self->capacity) {
        fprintf(stderr, "[PacketBuf] put_int: Buffer full (write_pos=%d, capacity=%d)\n",
                self->write_pos, self->capacity);
        return 0;
    }

    // 小端序写入（低字节在前）
    unsigned char *ptr = (unsigned char*)self->data + self->write_pos;
    ptr[0] = (unsigned char)(value & 0xFF);
    ptr[1] = (unsigned char)((value >> 8) & 0xFF);
    ptr[2] = (unsigned char)((value >> 16) & 0xFF);
    ptr[3] = (unsigned char)((value >> 24) & 0xFF);

    self->write_pos += 4;

    if (self->write_pos > self->size) {
        self->size = self->write_pos;
    }

    return 1;
}

/**
 * PacketBuf_get_index - Get current buffer index
 */
int __cdecl PacketBuf_get_index(PacketBuf *self)
{
    return 0;
}

/**
 * PacketBuf_put_binary - Put binary data into packet
 *
 * 实现说明 (2025-11-11):
 * - 直接写入二进制数据（不包含长度前缀）
 * - 调用者负责提供正确的长度
 *
 * @param self PacketBuf指针
 * @param data 二进制数据指针
 * @param length 数据长度
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_put_binary(PacketBuf *self, const char *data, int length)
{
    if (!self || !self->data || !data || length <= 0) {
        return 0;
    }

    // 检查缓冲区空间
    if (self->write_pos + length > self->capacity) {
        fprintf(stderr, "[PacketBuf] put_binary: Buffer full (write_pos=%d, length=%d, capacity=%d)\n",
                self->write_pos, length, self->capacity);
        return 0;
    }

    // 复制二进制数据
    memcpy((char*)self->data + self->write_pos, data, length);
    self->write_pos += length;

    if (self->write_pos > self->size) {
        self->size = self->write_pos;
    }

    return 1;
}

/**
 * PacketBuf_set_index - Set buffer index
 */
PacketBuf *__cdecl PacketBuf_set_index(PacketBuf *self, int id)
{
    return self;
}

/**
 * PacketBuf_put_str - Put string into packet (DNF协议格式)
 *
 * 实现说明 (2025-11-11):
 * - DNF字符串格式：2字节长度 + 字符串内容（不包括\0）
 * - 长度字段使用小端序
 * - 如果字符串为NULL，写入长度0
 *
 * @param self PacketBuf指针
 * @param str 字符串指针（可以为NULL）
 * @param maxlen 最大长度限制（未使用，保留参数兼容性）
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_put_str(PacketBuf *self, char *str, int maxlen)
{
    if (!self || !self->data) {
        return 0;
    }

    // 计算字符串长度（不包括\0）
    uint16_t len = 0;
    if (str) {
        len = (uint16_t)strlen(str);
    }

    // 先写入2字节长度（小端序）
    if (!PacketBuf_put_short(self, (int16_t)len)) {
        return 0;
    }

    // 如果字符串非空，写入字符串内容
    if (len > 0) {
        if (!PacketBuf_put_binary(self, str, len)) {
            return 0;
        }
    }

    return 1;
}

/**
 * PacketBuf_put_item_idx - Put item index
 */
int __cdecl PacketBuf_put_item_idx(PacketBuf *self, unsigned int id)
{
    return 0;
}

/**
 * PacketBuf_at - Get byte at position
 */
int __cdecl PacketBuf_at(PacketBuf *self, int id)
{
    return 0;
}

/**
 * PacketBuf_get_item_idx - Get item index from packet
 */
int __cdecl PacketBuf_get_item_idx(PacketBuf *self, unsigned int *a2)
{
    return 0;
}

/**
 * PacketBuf_get_packet - Get packet data
 */
int __cdecl PacketBuf_get_packet(PacketBuf *self, int id)
{
    return 0;
}

/**
 * PacketBuf_is_finallized - Check if packet is finalized
 */
int __cdecl PacketBuf_is_finallized(PacketBuf *self)
{
    return 0;
}

/**
 * PacketBuf_PacketBuf - Constructor
 */
void __cdecl PacketBuf_PacketBuf(PacketBuf *self)
{
}

/**
 * PacketBuf_clear - Clear packet buffer
 */
PacketBuf *__cdecl PacketBuf_clear(PacketBuf *self)
{
    return self;
}

/**
 * PacketBuf_clear_ptr - Clear pointer-based buffer
 */
PacketBuf *__cdecl PacketBuf_clear_ptr(PacketBuf *self)
{
    return self;
}

/**
 * PacketBuf_put_header - Put packet header
 */
int __cdecl PacketBuf_put_header(PacketBuf *self, char flag, __int16 a3)
{
    return 0;
}

/**
 * PacketBuf_put_byte with pointer - Put byte at offset
 */
int __cdecl PacketBuf_put_byte_ptr(PacketBuf *self, int *a2, char flag)
{
    return 0;
}

/**
 * PacketBuf_put_short with pointer - Put short at offset
 */
int __cdecl PacketBuf_put_short_ptr(PacketBuf *self, int *a2, __int16 a3)
{
    return 0;
}

/**
 * PacketBuf_put_int with pointer - Put int at offset
 */
int __cdecl PacketBuf_put_int_ptr(PacketBuf *self, int *a2, int id)
{
    return 0;
}

/**
 * PacketBuf_put_str with pointer - Put string at offset
 */
int __cdecl PacketBuf_put_str_ptr(PacketBuf *self, int *a2, char *a3, int id)
{
    return 0;
}

/**
 * PacketBuf_put_binary_ptr - Put binary at offset with pointer
 */
int __cdecl PacketBuf_put_binary_ptr(PacketBuf *self, int *a2, const char *a3, int id)
{
    return 0;
}

/**
 * PacketBuf_is_ptr_end - Check if pointer at end
 */
int __cdecl PacketBuf_is_ptr_end(PacketBuf *self, int id, int value)
{
    return 0;
}

/**
 * PacketBuf_is_len_end - Check if length at end
 */
_BOOL4 __cdecl PacketBuf_is_len_end(PacketBuf *self, int id)
{
    return 0;
}

/**
 * PacketBuf_get_byte - Get byte from packet
 *
 * 实现说明 (2025-11-11):
 * - 从read_pos位置读取1字节
 * - 自动更新read_pos
 * - 检查缓冲区边界
 *
 * @param self PacketBuf指针
 * @param out_value 输出字节的指针
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_get_byte(PacketBuf *self, char *out_value)
{
    if (!self || !self->data || !out_value) {
        return 0;
    }

    // 检查是否有足够数据可读
    if (self->read_pos + 1 > self->size) {
        fprintf(stderr, "[PacketBuf] get_byte: Not enough data (read_pos=%d, size=%d)\n",
                self->read_pos, self->size);
        return 0;
    }

    // 读取字节
    *out_value = ((char*)self->data)[self->read_pos];
    self->read_pos += 1;

    return 1;
}

/**
 * PacketBuf_get_byte (uint8_t variant) - Get byte as unsigned
 */
int __cdecl PacketBuf_get_byte_u8(PacketBuf *self, uint8_t *out_value)
{
    return PacketBuf_get_byte(self, (char*)out_value);
}

/**
 * PacketBuf_get_short - Get short from packet (小端序)
 *
 * 实现说明 (2025-11-11):
 * - 从read_pos位置读取2字节，小端序转换
 * - 自动更新read_pos
 *
 * @param self PacketBuf指针
 * @param out_value 输出16位整数的指针
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_get_short(PacketBuf *self, __int16 *out_value)
{
    if (!self || !self->data || !out_value) {
        return 0;
    }

    // 检查是否有足够数据可读
    if (self->read_pos + 2 > self->size) {
        fprintf(stderr, "[PacketBuf] get_short: Not enough data (read_pos=%d, size=%d)\n",
                self->read_pos, self->size);
        return 0;
    }

    // 小端序读取（低字节在前）
    unsigned char *ptr = (unsigned char*)self->data + self->read_pos;
    *out_value = (__int16)(ptr[0] | (ptr[1] << 8));
    self->read_pos += 2;

    return 1;
}

/**
 * PacketBuf_get_short (uint16_t variant) - Get short as unsigned
 */
int __cdecl PacketBuf_get_short_u16(PacketBuf *self, uint16_t *out_value)
{
    return PacketBuf_get_short(self, (__int16*)out_value);
}

/**
 * PacketBuf_get_int - Get int from packet (小端序)
 *
 * 实现说明 (2025-11-11):
 * - 从read_pos位置读取4字节，小端序转换
 * - 自动更新read_pos
 *
 * @param self PacketBuf指针
 * @param out_value 输出32位整数的指针
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_get_int(PacketBuf *self, int *out_value)
{
    if (!self || !self->data || !out_value) {
        return 0;
    }

    // 检查是否有足够数据可读
    if (self->read_pos + 4 > self->size) {
        fprintf(stderr, "[PacketBuf] get_int: Not enough data (read_pos=%d, size=%d)\n",
                self->read_pos, self->size);
        return 0;
    }

    // 小端序读取（低字节在前）
    unsigned char *ptr = (unsigned char*)self->data + self->read_pos;
    *out_value = (int)(ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24));
    self->read_pos += 4;

    return 1;
}

/**
 * PacketBuf_get_int (unsigned variant) - Get int as unsigned
 */
int __cdecl PacketBuf_get_int_u(PacketBuf *self, unsigned int *out_value)
{
    return PacketBuf_get_int(self, (int*)out_value);
}

/**
 * PacketBuf_get_str - Get string from packet (DNF协议格式)
 *
 * 实现说明 (2025-11-11):
 * - DNF字符串格式：2字节长度 + 字符串内容
 * - 自动添加\0终止符
 * - 检查缓冲区大小防止溢出
 *
 * @param self PacketBuf指针
 * @param out_buffer 输出字符串缓冲区
 * @param buffer_size 缓冲区大小
 * @param unused 未使用参数（保留兼容性）
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_get_str(PacketBuf *self, char *out_buffer, int buffer_size, int unused)
{
    if (!self || !self->data || !out_buffer || buffer_size <= 0) {
        return 0;
    }

    // 先读取2字节长度
    uint16_t len;
    if (!PacketBuf_get_short(self, (int16_t*)&len)) {
        return 0;
    }

    // 检查缓冲区大小（需要容纳字符串+\0）
    if (len + 1 > buffer_size) {
        fprintf(stderr, "[PacketBuf] get_str: Buffer too small (need=%d, have=%d)\n",
                len + 1, buffer_size);
        return 0;
    }

    // 检查数据包中是否有足够数据
    if (self->read_pos + len > self->size) {
        fprintf(stderr, "[PacketBuf] get_str: Not enough data for string (read_pos=%d, len=%d, size=%d)\n",
                self->read_pos, len, self->size);
        return 0;
    }

    // 读取字符串内容
    if (len > 0) {
        memcpy(out_buffer, (char*)self->data + self->read_pos, len);
        self->read_pos += len;
    }

    // 添加\0终止符
    out_buffer[len] = '\0';

    return 1;
}

/**
 * PacketBuf_get_binary - Get binary data from packet
 *
 * 实现说明 (2025-11-11):
 * - 直接读取指定长度的二进制数据
 * - 不包含长度前缀（调用者需要知道长度）
 *
 * @param self PacketBuf指针
 * @param out_buffer 输出缓冲区
 * @param length 要读取的字节数
 * @return 成功返回1，失败返回0
 */
int __cdecl PacketBuf_get_binary(PacketBuf *self, char *out_buffer, int length)
{
    if (!self || !self->data || !out_buffer || length <= 0) {
        return 0;
    }

    // 检查是否有足够数据可读
    if (self->read_pos + length > self->size) {
        fprintf(stderr, "[PacketBuf] get_binary: Not enough data (read_pos=%d, length=%d, size=%d)\n",
                self->read_pos, length, self->size);
        return 0;
    }

    // 复制二进制数据
    memcpy(out_buffer, (char*)self->data + self->read_pos, length);
    self->read_pos += length;

    return 1;
}

/**
 * PacketBuf_set_packet - Set packet data
 */
void __cdecl PacketBuf_set_packet(void **self, const char *src, int n)
{
}

/**
 * PacketBuf_bind_packet - Bind packet to buffer
 */
int __cdecl PacketBuf_bind_packet(PacketBuf *self, char *a2, int id)
{
    return 0;
}

/**
 * PacketBuf_finalize - Finalize packet for transmission
 *
 * 实现说明 (2025-11-11):
 * - 填写数据包头部的length字段
 * - 设置finalized标志
 * - header指针应该已经指向data的开头
 *
 * 使用流程：
 * 1. 调用put系列函数构造数据包
 * 2. 调用finalize完成打包
 * 3. 将data发送到网络
 *
 * @param self PacketBuf指针
 * @param encrypt 是否需要加密（暂未实现）
 */
void __cdecl PacketBuf_finalize(PacketBuf *self, bool encrypt)
{
    if (!self || !self->data) {
        return;
    }

    // 确保header指向data开头
    if (!self->header) {
        self->header = self->data;
    }

    // 填写数据包头部
    tagPacketHeader *header = (tagPacketHeader*)self->header;
    header->length = (uint16_t)self->size;  // 总长度（包含头部）

    // 设置finalized标志
    self->finalized = 1;

    printf("[PacketBuf] Finalized packet: type=%d, length=%d\n",
           header->type, header->length);
}

/**
 * PacketBuf_encrypt_packet - Encrypt packet
 */
void __cdecl PacketBuf_encrypt_packet(PacketBuf *self)
{
}

/**
 * PacketBuf_isEncryptRequired - Check if encryption required
 */
int __cdecl PacketBuf_isEncryptRequired(PacketBuf *self)
{
    return 0;
}

/**
 * PacketBuf_get_buf_ptr - Get buffer pointer
 */
int __cdecl PacketBuf_get_buf_ptr(PacketBuf *self, int id)
{
    return 0;
}

/**
 * PacketBuf_get_len - Get packet length
 */
int __cdecl PacketBuf_get_len(PacketBuf *self)
{
    return 0;
}

/**
 * PacketBuf_compress_packet - Compress packet
 */
int __cdecl PacketBuf_compress_packet(PacketBuf *self)
{
    return 0;
}

/**
 * PacketBuf_put_packet - Put packet (item variant)
 */
int __cdecl PacketBuf_put_packet_item(PacketBuf *self, const void *item)
{
    return 0;
}

/**
 * PacketBuf_copy - Copy packet
 */
void *__cdecl PacketBuf_copy(PacketBuf *self, const PacketBuf *a2)
{
    return NULL;
}

/**
 * PacketBuf_encFinalize - Finalize encryption
 */
void __cdecl PacketBuf_encFinalize(PacketBuf *self)
{
}

/**
 * PacketBuf_isCompressRequired - Check if compression required
 */
_BOOL4 __cdecl PacketBuf_isCompressRequired(PacketBuf *self, char *a2)
{
    return 0;
}

/**
 * PacketBuf_setLastError - Set last error code
 */
int __cdecl PacketBuf_setLastError(int id, int value)
{
    return 0;
}

/**
 * PacketBuf_getLastError - Get last error code
 */
int __cdecl PacketBuf_getLastError(PacketBuf *self)
{
    return 0;
}

// ========================================================================
// Module Lifecycle Functions
// ========================================================================

/**
 * Network_Module_Init - Initialize network module
 */
int __cdecl Network_Module_Init(void)
{
    // Initialize network subsystem
    CNetwork_CNetwork(NULL, 0);
    return 0;
}

// ========================================================================
// 主事件循环 (Main Event Loop) - 新增 (2025-11-11)
// ========================================================================

// 全局运行标志
static volatile int g_network_running = 0;

/**
 * Network_EventLoop - 主事件循环
 *
 * 实现说明 (2025-11-11):
 * - 使用epoll_wait等待事件
 * - 处理新连接（EPOLLIN on listen socket）
 * - 处理数据接收（EPOLLIN on client socket）
 * - 处理数据发送（EPOLLOUT）
 * - 处理错误和断开（EPOLLERR, EPOLLHUP, EPOLLRDHUP）
 *
 * 使用方法：
 * 1. 调用Network_Start()启动监听
 * 2. 在单独线程中调用Network_EventLoop()
 * 3. 调用Network_Stop()停止循环
 *
 * @param listen_fd 监听socket的文件描述符
 * @return 0=正常退出，-1=错误
 */
int Network_EventLoop(int listen_fd)
{
    if (g_epoll_fd < 0) {
        fprintf(stderr, "[EventLoop] ERROR: epoll not initialized\n");
        return -1;
    }

    if (listen_fd < 0) {
        fprintf(stderr, "[EventLoop] ERROR: invalid listen_fd: %d\n", listen_fd);
        return -1;
    }

    printf("[EventLoop] Starting main event loop (listen_fd=%d)\n", listen_fd);

    // 将监听socket注册到epoll
    struct epoll_event listen_event;
    if (!CEpoll_RegisterSession(NULL, &listen_event, (void*)(intptr_t)listen_fd, listen_fd)) {
        fprintf(stderr, "[EventLoop] Failed to register listen socket\n");
        return -1;
    }

    // 事件数组（最多1024个并发事件）
    struct epoll_event events[1024];
    g_network_running = 1;

    printf("[EventLoop] Event loop running...\n");

    while (g_network_running) {
        // 等待事件（100ms超时）
        int n = CEpoll_WaitForEvent(NULL, events, 100);

        if (n < 0) {
            fprintf(stderr, "[EventLoop] epoll_wait failed\n");
            break;
        }

        if (n == 0) {
            // 超时，没有事件（这是正常的，继续循环）
            // Week 6: 在空闲时检查会话超时
            extern int SessionTimer_ShouldCheck(void);
            extern int SessionTimer_ProcessTimeouts(void);

            if (SessionTimer_ShouldCheck()) {
                int closed = SessionTimer_ProcessTimeouts();
                if (closed > 0) {
                    printf("[EventLoop] Closed %d timed-out sessions\n", closed);
                }
            }
            continue;
        }

        // 处理就绪的事件
        int i;
        for (i = 0; i < n; i++) {
            int fd = (int)(intptr_t)events[i].data.ptr;
            uint32_t ev = events[i].events;

            // 检查是否是监听socket的事件
            if (fd == listen_fd) {
                // 新连接到达
                if (ev & EPOLLIN) {
                    // 在边缘触发模式下，需要循环accept直到EAGAIN
                    while (1) {
                        int *client_fd_ptr = CSession_OnAccept((void*)(intptr_t)listen_fd);
                        if (!client_fd_ptr) {
                            break;  // 没有更多连接
                        }

                        int client_fd = (int)(intptr_t)client_fd_ptr;

                        // 将新连接注册到epoll
                        struct epoll_event client_event;
                        if (!CEpoll_RegisterSession(NULL, &client_event, (void*)(intptr_t)client_fd, client_fd)) {
                            fprintf(stderr, "[EventLoop] Failed to register client socket %d\n", client_fd);
                            close(client_fd);
                        }
                    }
                }
            } else {
                // 客户端socket的事件
                if (ev & EPOLLIN) {
                    // 数据可读 - 调用接收处理
                    if (!CSession_OnRecv((void*)(intptr_t)fd)) {
                        // 接收失败或连接关闭
                        printf("[EventLoop] Connection closed by CSession_OnRecv on fd=%d\n", fd);
                        CEpoll_UnregisterSession(NULL, NULL, fd);
                        Session_Destroy(fd);  // 清理会话数据
                        close(fd);
                        continue;  // 跳过后续处理
                    }
                }

                if (ev & EPOLLOUT) {
                    // 可写（发送缓冲区有空间）
                    if (!CSession_OnSend((void*)(intptr_t)fd)) {
                        // 发送失败或连接关闭
                        printf("[EventLoop] Connection closed by CSession_OnSend on fd=%d\n", fd);
                        CEpoll_UnregisterSession(NULL, NULL, fd);
                        Session_Destroy(fd);
                        close(fd);
                        continue;
                    }
                }

                if (ev & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    // 错误或连接关闭
                    printf("[EventLoop] Connection closed or error on fd=%d\n", fd);
                    CEpoll_UnregisterSession(NULL, NULL, fd);
                    Session_Destroy(fd);  // 清理会话数据
                    close(fd);
                }
            }
        }
    }

    printf("[EventLoop] Event loop stopped\n");

    // 清理监听socket
    CEpoll_UnregisterSession(NULL, NULL, listen_fd);
    close(listen_fd);

    return 0;
}

/**
 * Network_StopEventLoop - 停止事件循环
 *
 * 实现说明 (2025-11-11):
 * - 设置运行标志为0，使事件循环退出
 */
void Network_StopEventLoop(void)
{
    printf("[EventLoop] Stopping event loop...\n");
    g_network_running = 0;
}

/**
 * Network_Module_Cleanup - Cleanup network module
 */
void __cdecl Network_Module_Cleanup(void)
{
    // Cleanup network subsystem
    if (g_epoll_fd > 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }
}
