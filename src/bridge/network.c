/*
 * network.c - Bridge 网络处理
 *
 * 实现 TCP/UDP 监听和 epoll 事件循环
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "bridge.h"

#define MAX_EVENTS 256

/* 内部函数声明 */
static int set_nonblocking(int fd);
static int set_socket_options(int fd);

/*
 * bridge_setup_tcp - 设置 TCP 监听套接字
 */
int bridge_setup_tcp(bridge_server_t* server)
{
    struct sockaddr_in addr;
    int opt = 1;

    server->tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->tcp_fd < 0) {
        bridge_log(LOG_ERROR, "network: socket() failed: %s", strerror(errno));
        return -1;
    }

    /* 设置 SO_REUSEADDR */
    if (setsockopt(server->tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        bridge_log(LOG_WARN, "network: setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
    }

    /* 设置其他套接字选项 */
    set_socket_options(server->tcp_fd);

    /* 绑定地址 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server->config.tcp_port);

    if (strcmp(server->config.listen_ip, "0.0.0.0") == 0 ||
        strlen(server->config.listen_ip) == 0) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, server->config.listen_ip, &addr.sin_addr) <= 0) {
            bridge_log(LOG_ERROR, "network: invalid IP address: %s",
                      server->config.listen_ip);
            close(server->tcp_fd);
            return -1;
        }
    }

    if (bind(server->tcp_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        bridge_log(LOG_ERROR, "network: bind() failed on port %d: %s",
                  server->config.tcp_port, strerror(errno));
        close(server->tcp_fd);
        return -1;
    }

    /* 监听 */
    if (listen(server->tcp_fd, SOMAXCONN) < 0) {
        bridge_log(LOG_ERROR, "network: listen() failed: %s", strerror(errno));
        close(server->tcp_fd);
        return -1;
    }

    /* 设置非阻塞 */
    if (set_nonblocking(server->tcp_fd) < 0) {
        bridge_log(LOG_ERROR, "network: set_nonblocking() failed");
        close(server->tcp_fd);
        return -1;
    }

    bridge_log(LOG_INFO, "network: TCP listening on %s:%d",
              server->config.listen_ip, server->config.tcp_port);

    return 0;
}

/*
 * bridge_setup_udp - 设置 UDP 套接字
 */
int bridge_setup_udp(bridge_server_t* server)
{
    struct sockaddr_in addr;
    int opt = 1;

    server->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->udp_fd < 0) {
        bridge_log(LOG_ERROR, "network: socket(UDP) failed: %s", strerror(errno));
        return -1;
    }

    /* 设置 SO_REUSEADDR */
    if (setsockopt(server->udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        bridge_log(LOG_WARN, "network: setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
    }

    /* 绑定地址 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server->config.udp_port);

    if (strcmp(server->config.listen_ip, "0.0.0.0") == 0 ||
        strlen(server->config.listen_ip) == 0) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, server->config.listen_ip, &addr.sin_addr);
    }

    if (bind(server->udp_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        bridge_log(LOG_ERROR, "network: bind(UDP) failed on port %d: %s",
                  server->config.udp_port, strerror(errno));
        close(server->udp_fd);
        return -1;
    }

    /* 设置非阻塞 */
    if (set_nonblocking(server->udp_fd) < 0) {
        bridge_log(LOG_ERROR, "network: set_nonblocking(UDP) failed");
        close(server->udp_fd);
        return -1;
    }

    bridge_log(LOG_INFO, "network: UDP listening on %s:%d",
              server->config.listen_ip, server->config.udp_port);

    return 0;
}

/*
 * bridge_setup_epoll - 设置 epoll
 */
int bridge_setup_epoll(bridge_server_t* server)
{
    struct epoll_event ev;

    server->epoll_fd = epoll_create1(0);
    if (server->epoll_fd < 0) {
        bridge_log(LOG_ERROR, "network: epoll_create1() failed: %s", strerror(errno));
        return -1;
    }

    /* 添加 TCP 监听套接字 */
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = server->tcp_fd;
    if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->tcp_fd, &ev) < 0) {
        bridge_log(LOG_ERROR, "network: epoll_ctl(TCP) failed: %s", strerror(errno));
        close(server->epoll_fd);
        return -1;
    }

    /* 添加 UDP 套接字 */
    ev.events = EPOLLIN;
    ev.data.fd = server->udp_fd;
    if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->udp_fd, &ev) < 0) {
        bridge_log(LOG_ERROR, "network: epoll_ctl(UDP) failed: %s", strerror(errno));
        close(server->epoll_fd);
        return -1;
    }

    bridge_log(LOG_INFO, "network: epoll initialized");

    return 0;
}

/*
 * bridge_accept_client - 接受新客户端连接
 */
client_conn_t* bridge_accept_client(bridge_server_t* server)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd;
    client_conn_t* client = NULL;
    struct epoll_event ev;

    client_fd = accept(server->tcp_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            bridge_log(LOG_ERROR, "network: accept() failed: %s", strerror(errno));
        }
        return NULL;
    }

    /* 检查客户端数量限制 */
    pthread_mutex_lock(&server->clients_lock);
    if (server->client_count >= server->config.max_clients) {
        pthread_mutex_unlock(&server->clients_lock);
        bridge_log(LOG_WARN, "network: max clients reached, rejecting connection");
        close(client_fd);
        return NULL;
    }

    /* 查找空闲槽位 */
    for (int i = 0; i < server->config.max_clients; i++) {
        if (server->clients[i].state == CLIENT_STATE_DISCONNECTED) {
            client = &server->clients[i];
            break;
        }
    }

    if (!client) {
        pthread_mutex_unlock(&server->clients_lock);
        bridge_log(LOG_ERROR, "network: no free client slot");
        close(client_fd);
        return NULL;
    }

    /* 初始化客户端连接 */
    memset(client, 0, sizeof(client_conn_t));
    client->fd = client_fd;
    client->client_id = (uint32_t)(client - server->clients) + 1;
    client->state = CLIENT_STATE_CONNECTING;
    memcpy(&client->addr, &client_addr, sizeof(client_addr));
    client->connect_time = time(NULL);
    client->last_activity = client->connect_time;
    pthread_mutex_init(&client->lock, NULL);

    server->client_count++;
    server->total_connections++;
    pthread_mutex_unlock(&server->clients_lock);

    /* 设置非阻塞 */
    set_nonblocking(client_fd);
    set_socket_options(client_fd);

    /* 添加到 epoll */
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = client;
    if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
        bridge_log(LOG_ERROR, "network: epoll_ctl(client) failed: %s", strerror(errno));
        bridge_disconnect_client(server, client);
        return NULL;
    }

    bridge_log(LOG_INFO, "network: client connected from %s:%d (id=%u, fd=%d)",
              inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
              client->client_id, client_fd);

    client->state = CLIENT_STATE_CONNECTED;

    return client;
}

/*
 * bridge_disconnect_client - 断开客户端连接
 */
void bridge_disconnect_client(bridge_server_t* server, client_conn_t* client)
{
    if (!client || client->state == CLIENT_STATE_DISCONNECTED) {
        return;
    }

    bridge_log(LOG_INFO, "network: disconnecting client id=%u fd=%d",
              client->client_id, client->fd);

    /* 从 epoll 移除 */
    epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);

    /* 关闭套接字 */
    if (client->fd >= 0) {
        close(client->fd);
    }

    /* 更新计数 */
    pthread_mutex_lock(&server->clients_lock);
    server->client_count--;
    pthread_mutex_unlock(&server->clients_lock);

    /* 清理 */
    pthread_mutex_destroy(&client->lock);
    client->fd = -1;
    client->state = CLIENT_STATE_DISCONNECTED;
}

/*
 * bridge_recv_tcp - 接收 TCP 数据
 */
int bridge_recv_tcp(bridge_server_t* server, client_conn_t* client)
{
    int n;
    int space = RECV_BUFFER_SIZE - client->recv_len;

    if (space <= 0) {
        bridge_log(LOG_WARN, "network: recv buffer full for client %u", client->client_id);
        return -1;
    }

    n = recv(client->fd, client->recv_buffer + client->recv_len, space, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        bridge_log(LOG_ERROR, "network: recv() failed for client %u: %s",
                  client->client_id, strerror(errno));
        return -1;
    }

    if (n == 0) {
        bridge_log(LOG_INFO, "network: client %u closed connection", client->client_id);
        return -1;
    }

    client->recv_len += n;
    client->last_activity = time(NULL);
    server->bytes_received += n;

    return n;
}

/*
 * bridge_send_tcp - 发送 TCP 数据
 */
int bridge_send_tcp(client_conn_t* client, const uint8_t* data, int len)
{
    int sent = 0;
    int n;

    while (sent < len) {
        n = send(client->fd, data + sent, len - sent, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* 暂时无法发送，稍后重试 */
                break;
            }
            bridge_log(LOG_ERROR, "network: send() failed: %s", strerror(errno));
            return -1;
        }
        sent += n;
    }

    return sent;
}

/*
 * bridge_recv_udp - 接收 UDP 数据
 */
int bridge_recv_udp(bridge_server_t* server)
{
    uint8_t buffer[RECV_BUFFER_SIZE];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    int n;

    n = recvfrom(server->udp_fd, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&from_addr, &addr_len);
    if (n < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            bridge_log(LOG_ERROR, "network: recvfrom() failed: %s", strerror(errno));
        }
        return -1;
    }

    server->bytes_received += n;

    bridge_log(LOG_DEBUG, "network: UDP packet from %s:%d, len=%d",
              inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), n);

    /* TODO: 处理 UDP 数据包 */

    return n;
}

/*
 * bridge_send_udp - 发送 UDP 数据
 */
int bridge_send_udp(bridge_server_t* server, struct sockaddr_in* addr,
                    const uint8_t* data, int len)
{
    int n;

    n = sendto(server->udp_fd, data, len, 0, (struct sockaddr*)addr, sizeof(*addr));
    if (n < 0) {
        bridge_log(LOG_ERROR, "network: sendto() failed: %s", strerror(errno));
        return -1;
    }

    server->bytes_sent += n;

    return n;
}

/*
 * set_nonblocking - 设置套接字为非阻塞模式
 */
static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*
 * set_socket_options - 设置套接字选项
 */
static int set_socket_options(int fd)
{
    int opt;

    /* TCP_NODELAY - 禁用 Nagle 算法 */
    opt = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    /* SO_KEEPALIVE - 启用心跳 */
    opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    return 0;
}
