/**
 * @file tcp_connection.c
 * @brief TCP connection implementation
 */

#include "tcp_connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

/* Get current timestamp */
static u32 get_timestamp(void)
{
    return (u32)time(NULL);
}

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

/* Try to flush send buffer */
static int try_flush_send(TcpConnection* conn)
{
    const u8* data;
    u32 len;
    int sent;

    while (!ring_buffer_is_empty(&conn->send_buffer)) {
        data = ring_buffer_read_ptr(&conn->send_buffer, &len);
        if (!data || len == 0) {
            break;
        }

        sent = send(conn->fd, data, len, MSG_NOSIGNAL);

        if (sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Would block, wait for EPOLLOUT */
                return 0;
            }
            if (errno == EINTR) {
                continue;
            }
            /* Real error */
            return -1;
        }

        ring_buffer_commit_read(&conn->send_buffer, sent);
        conn->bytes_sent += sent;
        conn->last_send_time = get_timestamp();
    }

    /* If buffer is empty, disable EPOLLOUT */
    if (ring_buffer_is_empty(&conn->send_buffer)) {
        reactor_mod(conn->reactor, conn->fd, REACTOR_EVENT_READ | REACTOR_EVENT_ET);
    }

    return 0;
}

/* Handle incoming data */
static int handle_recv(TcpConnection* conn)
{
    u8* write_ptr;
    u32 space;
    int received;
    const u8* read_ptr;
    u32 available;
    int consumed;

    while (1) {
        /* Get write space */
        write_ptr = ring_buffer_write_ptr(&conn->recv_buffer, &space);
        if (!write_ptr || space == 0) {
            /* Buffer full */
            fprintf(stderr, "[TCP_CONN] recv buffer full, fd=%d\n", conn->fd);
            return -1;
        }

        received = recv(conn->fd, write_ptr, space, 0);

        if (received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  /* No more data */
            }
            if (errno == EINTR) {
                continue;
            }
            /* Real error */
            return -1;
        }

        if (received == 0) {
            /* Connection closed by peer */
            return -1;
        }

        ring_buffer_commit_write(&conn->recv_buffer, received);
        conn->bytes_recv += received;
        conn->last_recv_time = get_timestamp();
    }

    /* Call data callback to process received data */
    if (conn->data_callback) {
        while (!ring_buffer_is_empty(&conn->recv_buffer)) {
            read_ptr = ring_buffer_read_ptr(&conn->recv_buffer, &available);
            if (!read_ptr || available == 0) {
                break;
            }

            consumed = conn->data_callback(conn, read_ptr, available,
                                           conn->user_data);

            if (consumed <= 0) {
                break;  /* Need more data or error */
            }

            ring_buffer_commit_read(&conn->recv_buffer, consumed);
            conn->packets_recv++;
        }
    }

    return 0;
}

/* Reactor event handler */
static void conn_on_event(EpollReactor* reactor, int fd,
                          u32 events, void* user_data)
{
    TcpConnection* conn;

    UNUSED(reactor);
    UNUSED(fd);

    conn = (TcpConnection*)user_data;
    if (!conn) {
        return;
    }

    /* Handle error/close */
    if (events & (REACTOR_EVENT_ERROR | REACTOR_EVENT_CLOSE)) {
        conn->state = TCP_CONN_STATE_CLOSED;
        if (conn->event_callback) {
            conn->event_callback(conn, TCP_CONN_EVENT_CLOSED, conn->user_data);
        }
        return;
    }

    /* Handle connecting state */
    if (conn->state == TCP_CONN_STATE_CONNECTING) {
        int error = 0;
        socklen_t len = sizeof(error);

        getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, &error, &len);

        if (error != 0) {
            conn->state = TCP_CONN_STATE_CLOSED;
            if (conn->event_callback) {
                conn->event_callback(conn, TCP_CONN_EVENT_ERROR, conn->user_data);
            }
            return;
        }

        conn->state = TCP_CONN_STATE_CONNECTED;
        reactor_mod(conn->reactor, conn->fd,
                    REACTOR_EVENT_READ | REACTOR_EVENT_ET);

        if (conn->event_callback) {
            conn->event_callback(conn, TCP_CONN_EVENT_CONNECTED, conn->user_data);
        }
        return;
    }

    /* Handle write ready */
    if (events & REACTOR_EVENT_WRITE) {
        if (try_flush_send(conn) < 0) {
            conn->state = TCP_CONN_STATE_CLOSED;
            if (conn->event_callback) {
                conn->event_callback(conn, TCP_CONN_EVENT_ERROR, conn->user_data);
            }
            return;
        }
    }

    /* Handle read ready */
    if (events & REACTOR_EVENT_READ) {
        if (handle_recv(conn) < 0) {
            conn->state = TCP_CONN_STATE_CLOSED;
            if (conn->event_callback) {
                conn->event_callback(conn, TCP_CONN_EVENT_CLOSED, conn->user_data);
            }
            return;
        }
    }
}

int tcp_conn_init(TcpConnection* conn, EpollReactor* reactor)
{
    if (!conn || !reactor) {
        return ENGINE_ERROR_PARAM;
    }

    memset(conn, 0, sizeof(TcpConnection));
    conn->fd = -1;
    conn->state = TCP_CONN_STATE_NONE;
    conn->reactor = reactor;

    if (ring_buffer_init(&conn->recv_buffer, TCP_CONN_DEFAULT_RECV_SIZE) != ENGINE_OK) {
        return ENGINE_ERROR_MEMORY;
    }

    if (ring_buffer_init(&conn->send_buffer, TCP_CONN_DEFAULT_SEND_SIZE) != ENGINE_OK) {
        ring_buffer_cleanup(&conn->recv_buffer);
        return ENGINE_ERROR_MEMORY;
    }

    return ENGINE_OK;
}

int tcp_conn_init_accepted(TcpConnection* conn, EpollReactor* reactor,
                           int fd, u32 remote_ip, u16 remote_port)
{
    int result;

    result = tcp_conn_init(conn, reactor);
    if (result != ENGINE_OK) {
        return result;
    }

    conn->fd = fd;
    conn->remote_ip = remote_ip;
    conn->remote_port = remote_port;
    conn->state = TCP_CONN_STATE_CONNECTED;
    conn->connect_time = get_timestamp();

    /* Add to reactor */
    result = reactor_add(reactor, fd, REACTOR_EVENT_READ | REACTOR_EVENT_ET,
                         conn_on_event, conn);
    if (result != ENGINE_OK) {
        ring_buffer_cleanup(&conn->recv_buffer);
        ring_buffer_cleanup(&conn->send_buffer);
        return result;
    }

    return ENGINE_OK;
}

void tcp_conn_cleanup(TcpConnection* conn)
{
    if (!conn) {
        return;
    }

    if (conn->fd >= 0) {
        if (conn->reactor) {
            reactor_del(conn->reactor, conn->fd);
        }
        close(conn->fd);
        conn->fd = -1;
    }

    ring_buffer_cleanup(&conn->recv_buffer);
    ring_buffer_cleanup(&conn->send_buffer);

    conn->state = TCP_CONN_STATE_CLOSED;
    conn->reactor = NULL;
}

int tcp_conn_connect(TcpConnection* conn, u32 ip, u16 port)
{
    struct sockaddr_in addr;
    int fd;
    int result;
    int opt;

    if (!conn || !conn->reactor) {
        return ENGINE_ERROR_PARAM;
    }

    /* Create socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return ENGINE_ERROR_IO;
    }

    /* Set non-blocking */
    if (set_nonblocking(fd) < 0) {
        close(fd);
        return ENGINE_ERROR_IO;
    }

    /* Set TCP_NODELAY */
    opt = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    /* Connect */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port = htons(port);

    result = connect(fd, (struct sockaddr*)&addr, sizeof(addr));

    if (result < 0 && errno != EINPROGRESS) {
        close(fd);
        return ENGINE_ERROR_IO;
    }

    conn->fd = fd;
    conn->remote_ip = ip;
    conn->remote_port = port;
    conn->state = TCP_CONN_STATE_CONNECTING;
    conn->connect_time = get_timestamp();

    /* Add to reactor (wait for write ready) */
    result = reactor_add(conn->reactor, fd,
                         REACTOR_EVENT_WRITE | REACTOR_EVENT_ET,
                         conn_on_event, conn);
    if (result != ENGINE_OK) {
        close(fd);
        conn->fd = -1;
        return result;
    }

    return ENGINE_OK;
}

void tcp_conn_close(TcpConnection* conn)
{
    if (!conn) {
        return;
    }

    if (conn->state == TCP_CONN_STATE_CLOSED ||
        conn->state == TCP_CONN_STATE_CLOSING) {
        return;
    }

    conn->state = TCP_CONN_STATE_CLOSING;

    /* Try to flush remaining data */
    if (!ring_buffer_is_empty(&conn->send_buffer)) {
        try_flush_send(conn);
    }

    /* Close socket */
    if (conn->fd >= 0) {
        if (conn->reactor) {
            reactor_del(conn->reactor, conn->fd);
        }
        close(conn->fd);
        conn->fd = -1;
    }

    conn->state = TCP_CONN_STATE_CLOSED;
}

void tcp_conn_set_callbacks(TcpConnection* conn,
                            TcpConnEventCallback event_callback,
                            TcpConnDataCallback data_callback,
                            void* user_data)
{
    if (conn) {
        conn->event_callback = event_callback;
        conn->data_callback = data_callback;
        conn->user_data = user_data;
    }
}

int tcp_conn_send(TcpConnection* conn, const u8* data, int len)
{
    u32 written;

    if (!conn || !data || len <= 0) {
        return ENGINE_ERROR_PARAM;
    }

    if (conn->state != TCP_CONN_STATE_CONNECTED) {
        return ENGINE_ERROR_IO;
    }

    /* Try to send directly if buffer is empty */
    if (ring_buffer_is_empty(&conn->send_buffer)) {
        int sent = send(conn->fd, data, len, MSG_NOSIGNAL);

        if (sent < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                return ENGINE_ERROR_IO;
            }
            sent = 0;
        }

        if (sent == len) {
            conn->bytes_sent += sent;
            conn->packets_sent++;
            conn->last_send_time = get_timestamp();
            return ENGINE_OK;
        }

        /* Buffer remaining data */
        data += sent;
        len -= sent;
        conn->bytes_sent += sent;
    }

    /* Write to buffer */
    written = ring_buffer_write(&conn->send_buffer, data, len);
    if (written < (u32)len) {
        return ENGINE_ERROR_FULL;
    }

    conn->packets_sent++;

    /* Enable EPOLLOUT */
    reactor_mod(conn->reactor, conn->fd,
                REACTOR_EVENT_READ | REACTOR_EVENT_WRITE | REACTOR_EVENT_ET);

    return ENGINE_OK;
}

TcpConnState tcp_conn_get_state(const TcpConnection* conn)
{
    return conn ? conn->state : TCP_CONN_STATE_NONE;
}

int tcp_conn_is_connected(const TcpConnection* conn)
{
    return conn && conn->state == TCP_CONN_STATE_CONNECTED;
}

void tcp_conn_get_remote(const TcpConnection* conn, u32* ip, u16* port)
{
    if (!conn) {
        if (ip) *ip = 0;
        if (port) *port = 0;
        return;
    }

    if (ip) *ip = conn->remote_ip;
    if (port) *port = conn->remote_port;
}

int tcp_conn_get_fd(const TcpConnection* conn)
{
    return conn ? conn->fd : -1;
}
