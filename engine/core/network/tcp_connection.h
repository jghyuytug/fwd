/**
 * @file tcp_connection.h
 * @brief TCP connection abstraction
 *
 * Manages a single TCP connection with buffered I/O.
 */

#ifndef ENGINE_CORE_NETWORK_TCP_CONNECTION_H
#define ENGINE_CORE_NETWORK_TCP_CONNECTION_H

#include "../types.h"
#include "epoll_reactor.h"
#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_CONN_DEFAULT_RECV_SIZE  65536
#define TCP_CONN_DEFAULT_SEND_SIZE  65536
#define TCP_CONN_MAX_PACKET_SIZE    8192

/* Connection state */
typedef enum {
    TCP_CONN_STATE_NONE = 0,
    TCP_CONN_STATE_CONNECTING,
    TCP_CONN_STATE_CONNECTED,
    TCP_CONN_STATE_CLOSING,
    TCP_CONN_STATE_CLOSED
} TcpConnState;

/* Forward declaration */
struct TcpConnection;
typedef struct TcpConnection TcpConnection;

/**
 * Connection event callback
 * @param conn      Connection instance
 * @param event     Event type (connected, closed, error)
 * @param user_data User context
 */
typedef void (*TcpConnEventCallback)(TcpConnection* conn, int event,
                                     void* user_data);

/**
 * Data received callback
 * @param conn      Connection instance
 * @param data      Received data
 * @param len       Data length
 * @param user_data User context
 * @return Number of bytes consumed (0 to keep data in buffer)
 */
typedef int (*TcpConnDataCallback)(TcpConnection* conn,
                                   const u8* data, int len,
                                   void* user_data);

/* Connection events */
#define TCP_CONN_EVENT_CONNECTED  1
#define TCP_CONN_EVENT_CLOSED     2
#define TCP_CONN_EVENT_ERROR      3

/* Connection structure */
struct TcpConnection {
    int               fd;
    TcpConnState      state;
    u32               remote_ip;
    u16               remote_port;
    EpollReactor*     reactor;
    RingBuffer        recv_buffer;
    RingBuffer        send_buffer;
    TcpConnEventCallback event_callback;
    TcpConnDataCallback  data_callback;
    void*             user_data;
    u32               connect_time;
    u32               last_recv_time;
    u32               last_send_time;
    /* Statistics */
    u64               bytes_recv;
    u64               bytes_sent;
    u64               packets_recv;
    u64               packets_sent;
};

/**
 * Initialize TCP connection
 * @param conn     Connection instance
 * @param reactor  Reactor for I/O events
 * @return ENGINE_OK on success
 */
int tcp_conn_init(TcpConnection* conn, EpollReactor* reactor);

/**
 * Initialize from accepted socket
 * @param conn       Connection instance
 * @param reactor    Reactor for I/O events
 * @param fd         Accepted socket FD
 * @param remote_ip  Client IP
 * @param remote_port Client port
 * @return ENGINE_OK on success
 */
int tcp_conn_init_accepted(TcpConnection* conn, EpollReactor* reactor,
                           int fd, u32 remote_ip, u16 remote_port);

/**
 * Cleanup connection resources
 * @param conn  Connection instance
 */
void tcp_conn_cleanup(TcpConnection* conn);

/**
 * Connect to remote host
 * @param conn  Connection instance
 * @param ip    Remote IP (network byte order)
 * @param port  Remote port
 * @return ENGINE_OK on success (may be async)
 */
int tcp_conn_connect(TcpConnection* conn, u32 ip, u16 port);

/**
 * Close connection
 * @param conn  Connection instance
 */
void tcp_conn_close(TcpConnection* conn);

/**
 * Set callbacks
 * @param conn           Connection instance
 * @param event_callback Event callback
 * @param data_callback  Data received callback
 * @param user_data      User context
 */
void tcp_conn_set_callbacks(TcpConnection* conn,
                            TcpConnEventCallback event_callback,
                            TcpConnDataCallback data_callback,
                            void* user_data);

/**
 * Send data
 * @param conn  Connection instance
 * @param data  Data to send
 * @param len   Data length
 * @return ENGINE_OK on success
 */
int tcp_conn_send(TcpConnection* conn, const u8* data, int len);

/**
 * Get connection state
 * @param conn  Connection instance
 * @return Connection state
 */
TcpConnState tcp_conn_get_state(const TcpConnection* conn);

/**
 * Check if connected
 * @param conn  Connection instance
 * @return 1 if connected, 0 otherwise
 */
int tcp_conn_is_connected(const TcpConnection* conn);

/**
 * Get remote address
 * @param conn  Connection instance
 * @param ip    Output IP
 * @param port  Output port
 */
void tcp_conn_get_remote(const TcpConnection* conn, u32* ip, u16* port);

/**
 * Get connection FD
 * @param conn  Connection instance
 * @return Socket FD or -1
 */
int tcp_conn_get_fd(const TcpConnection* conn);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_NETWORK_TCP_CONNECTION_H */
