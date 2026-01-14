/**
 * @file tcp_listener.c
 * @brief TCP server listener implementation
 */

#include "tcp_listener.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

/* Set socket to non-blocking */
static int set_nonblocking(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* Internal accept handler */
static void listener_on_accept(EpollReactor* reactor, int fd,
                               u32 events, void* user_data)
{
    TcpListener* listener;
    struct sockaddr_in addr;
    socklen_t addr_len;
    int client_fd;
    int result;

    UNUSED(reactor);
    UNUSED(events);

    listener = (TcpListener*)user_data;
    if (!listener) {
        return;
    }

    /* Accept all pending connections */
    while (1) {
        addr_len = sizeof(addr);
        client_fd = accept(fd, (struct sockaddr*)&addr, &addr_len);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  /* No more connections */
            }
            if (errno == EINTR) {
                continue;  /* Interrupted, retry */
            }
            fprintf(stderr, "[LISTENER] accept failed: %s\n", strerror(errno));
            break;
        }

        /* Set non-blocking */
        if (set_nonblocking(client_fd) < 0) {
            fprintf(stderr, "[LISTENER] set_nonblocking failed for fd=%d\n",
                    client_fd);
            close(client_fd);
            listener->reject_count++;
            continue;
        }

        /* Enable TCP_NODELAY */
        {
            int opt = 1;
            setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY,
                       &opt, sizeof(opt));
        }

        /* Call user callback */
        if (listener->accept_callback) {
            result = listener->accept_callback(
                listener, client_fd,
                ntohl(addr.sin_addr.s_addr),
                ntohs(addr.sin_port),
                listener->user_data);

            if (result != 0) {
                /* Callback rejected connection */
                close(client_fd);
                listener->reject_count++;
                continue;
            }
        }

        listener->accept_count++;
    }
}

int tcp_listener_init(TcpListener* listener, EpollReactor* reactor)
{
    if (!listener || !reactor) {
        return ENGINE_ERROR_PARAM;
    }

    memset(listener, 0, sizeof(TcpListener));
    listener->fd = -1;
    listener->reactor = reactor;

    return ENGINE_OK;
}

void tcp_listener_cleanup(TcpListener* listener)
{
    if (!listener) {
        return;
    }

    if (listener->fd >= 0) {
        if (listener->reactor) {
            reactor_del(listener->reactor, listener->fd);
        }
        close(listener->fd);
        listener->fd = -1;
    }

    listener->reactor = NULL;
}

int tcp_listener_bind(TcpListener* listener, u32 bind_ip, u16 port, int backlog)
{
    struct sockaddr_in addr;
    int opt;
    int fd;

    if (!listener || !listener->reactor || port == 0) {
        return ENGINE_ERROR_PARAM;
    }

    if (backlog <= 0) {
        backlog = LISTENER_DEFAULT_BACKLOG;
    }

    /* Create socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "[LISTENER] socket() failed: %s\n", strerror(errno));
        return ENGINE_ERROR_IO;
    }

    /* Set SO_REUSEADDR */
    opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Set non-blocking */
    if (set_nonblocking(fd) < 0) {
        fprintf(stderr, "[LISTENER] set_nonblocking failed\n");
        close(fd);
        return ENGINE_ERROR_IO;
    }

    /* Bind */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(bind_ip);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[LISTENER] bind() failed on port %d: %s\n",
                port, strerror(errno));
        close(fd);
        return ENGINE_ERROR_IO;
    }

    /* Listen */
    if (listen(fd, backlog) < 0) {
        fprintf(stderr, "[LISTENER] listen() failed: %s\n", strerror(errno));
        close(fd);
        return ENGINE_ERROR_IO;
    }

    /* Add to reactor */
    if (reactor_add(listener->reactor, fd,
                    REACTOR_EVENT_READ | REACTOR_EVENT_ET,
                    listener_on_accept, listener) != ENGINE_OK) {
        fprintf(stderr, "[LISTENER] reactor_add failed\n");
        close(fd);
        return ENGINE_ERROR_IO;
    }

    listener->fd = fd;
    listener->port = port;
    listener->bind_ip = bind_ip;

    printf("[LISTENER] Listening on port %d (fd=%d)\n", port, fd);

    return ENGINE_OK;
}

void tcp_listener_set_callback(TcpListener* listener,
                               ListenerAcceptCallback callback,
                               void* user_data)
{
    if (listener) {
        listener->accept_callback = callback;
        listener->user_data = user_data;
    }
}

int tcp_listener_get_fd(const TcpListener* listener)
{
    return listener ? listener->fd : -1;
}

u16 tcp_listener_get_port(const TcpListener* listener)
{
    return listener ? listener->port : 0;
}
