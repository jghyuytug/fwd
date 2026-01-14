/*
 * bridge.c - DNF Bridge Service 核心实现
 *
 * Bridge 是客户端连接的入口点，负责:
 * 1. 接受客户端连接
 * 2. 解析协议头
 * 3. 路由到 Channel 或 Game 服务
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/epoll.h>
#include "bridge.h"

#define MAX_EVENTS 256

/* 全局服务器指针，用于信号处理 */
static bridge_server_t* g_server = NULL;

/* 外部函数声明 */
extern int bridge_process_client_data(bridge_server_t* server, client_conn_t* client);

/* 日志级别名称 */
static const char* log_level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

/* 当前日志级别 */
static int g_log_level = LOG_INFO;

/*
 * bridge_log - 日志输出
 */
void bridge_log(int level, const char* fmt, ...)
{
    if (level < g_log_level) {
        return;
    }

    va_list args;
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    char time_buf[32];

    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm);

    fprintf(stdout, "[%s] [%s] ", time_buf, log_level_names[level]);

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fprintf(stdout, "\n");
    fflush(stdout);
}

/*
 * signal_handler - 信号处理
 */
static void signal_handler(int sig)
{
    bridge_log(LOG_INFO, "signal: received signal %d", sig);

    if (g_server) {
        g_server->running = 0;
    }
}

/*
 * bridge_init - 初始化 Bridge 服务
 *
 * @server: 服务器结构
 * @config_path: 配置文件路径
 *
 * 返回: 0成功, -1失败
 */
int bridge_init(bridge_server_t* server, const char* config_path)
{
    if (!server || !config_path) {
        bridge_log(LOG_ERROR, "init: invalid parameters");
        return -1;
    }

    memset(server, 0, sizeof(bridge_server_t));
    server->tcp_fd = -1;
    server->udp_fd = -1;
    server->epoll_fd = -1;

    bridge_log(LOG_INFO, "============================================");
    bridge_log(LOG_INFO, "DNF Bridge Service v%d.%d.%d",
              BRIDGE_VERSION_MAJOR, BRIDGE_VERSION_MINOR, BRIDGE_VERSION_PATCH);
    bridge_log(LOG_INFO, "============================================");

    /* 加载配置 */
    if (bridge_load_config(config_path, &server->config) < 0) {
        bridge_log(LOG_ERROR, "init: failed to load config");
        return -1;
    }

    /* 分配客户端数组 */
    server->clients = (client_conn_t*)calloc(server->config.max_clients,
                                              sizeof(client_conn_t));
    if (!server->clients) {
        bridge_log(LOG_ERROR, "init: failed to allocate client array");
        return -1;
    }

    /* 初始化客户端槽位 */
    for (int i = 0; i < server->config.max_clients; i++) {
        server->clients[i].fd = -1;
        server->clients[i].state = CLIENT_STATE_DISCONNECTED;
    }

    /* 初始化锁 */
    pthread_mutex_init(&server->clients_lock, NULL);

    /* 设置 TCP 监听 */
    if (bridge_setup_tcp(server) < 0) {
        bridge_log(LOG_ERROR, "init: failed to setup TCP");
        bridge_cleanup(server);
        return -1;
    }

    /* 设置 UDP */
    if (bridge_setup_udp(server) < 0) {
        bridge_log(LOG_ERROR, "init: failed to setup UDP");
        bridge_cleanup(server);
        return -1;
    }

    /* 设置 epoll */
    if (bridge_setup_epoll(server) < 0) {
        bridge_log(LOG_ERROR, "init: failed to setup epoll");
        bridge_cleanup(server);
        return -1;
    }

    /* 设置信号处理 */
    g_server = server;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    server->running = 1;

    bridge_log(LOG_INFO, "init: Bridge service initialized successfully");

    return 0;
}

/*
 * bridge_cleanup - 清理 Bridge 服务
 */
void bridge_cleanup(bridge_server_t* server)
{
    if (!server) {
        return;
    }

    bridge_log(LOG_INFO, "cleanup: shutting down Bridge service...");

    /* 断开所有客户端 */
    if (server->clients) {
        for (int i = 0; i < server->config.max_clients; i++) {
            if (server->clients[i].state != CLIENT_STATE_DISCONNECTED) {
                bridge_disconnect_client(server, &server->clients[i]);
            }
        }
        free(server->clients);
        server->clients = NULL;
    }

    /* 关闭套接字 */
    if (server->epoll_fd >= 0) {
        close(server->epoll_fd);
        server->epoll_fd = -1;
    }

    if (server->tcp_fd >= 0) {
        close(server->tcp_fd);
        server->tcp_fd = -1;
    }

    if (server->udp_fd >= 0) {
        close(server->udp_fd);
        server->udp_fd = -1;
    }

    pthread_mutex_destroy(&server->clients_lock);

    /* 打印统计 */
    bridge_log(LOG_INFO, "cleanup: Statistics:");
    bridge_log(LOG_INFO, "cleanup:   Total connections: %lu", server->total_connections);
    bridge_log(LOG_INFO, "cleanup:   Total packets: %lu", server->total_packets);
    bridge_log(LOG_INFO, "cleanup:   Bytes received: %lu", server->bytes_received);
    bridge_log(LOG_INFO, "cleanup:   Bytes sent: %lu", server->bytes_sent);

    bridge_log(LOG_INFO, "cleanup: Bridge service stopped");
}

/*
 * bridge_run - 主事件循环
 */
int bridge_run(bridge_server_t* server)
{
    struct epoll_event events[MAX_EVENTS];
    int nfds;

    bridge_log(LOG_INFO, "run: entering main event loop");

    while (server->running) {
        nfds = epoll_wait(server->epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            bridge_log(LOG_ERROR, "run: epoll_wait() failed: %s", strerror(errno));
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            /* TCP 监听套接字 - 新连接 */
            if (fd == server->tcp_fd) {
                while (1) {
                    client_conn_t* client = bridge_accept_client(server);
                    if (!client) {
                        break;
                    }
                }
            }
            /* UDP 套接字 */
            else if (fd == server->udp_fd) {
                bridge_recv_udp(server);
            }
            /* 客户端数据 */
            else {
                client_conn_t* client = (client_conn_t*)events[i].data.ptr;

                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    bridge_log(LOG_DEBUG, "run: client error/hangup, id=%u",
                              client->client_id);
                    bridge_disconnect_client(server, client);
                    continue;
                }

                if (events[i].events & EPOLLIN) {
                    int n = bridge_recv_tcp(server, client);
                    if (n < 0) {
                        bridge_disconnect_client(server, client);
                        continue;
                    }

                    if (n > 0) {
                        /* 处理接收到的数据 */
                        bridge_process_client_data(server, client);
                    }
                }
            }
        }

        /* 定期任务: 检查超时连接等 */
        /* TODO: 实现超时检查 */
    }

    bridge_log(LOG_INFO, "run: exiting main event loop");

    return 0;
}

/*
 * bridge_stop - 停止 Bridge 服务
 */
void bridge_stop(bridge_server_t* server)
{
    if (server) {
        server->running = 0;
    }
}

/*
 * bridge_find_client - 查找客户端
 */
client_conn_t* bridge_find_client(bridge_server_t* server, uint32_t client_id)
{
    if (!server || client_id == 0 || client_id > (uint32_t)server->config.max_clients) {
        return NULL;
    }

    client_conn_t* client = &server->clients[client_id - 1];
    if (client->state == CLIENT_STATE_DISCONNECTED) {
        return NULL;
    }

    return client;
}
