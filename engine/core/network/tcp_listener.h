/**
 * @file tcp_listener.h
 * @brief TCP server listener
 *
 * Manages TCP server socket and accepts incoming connections.
 */

#ifndef ENGINE_CORE_NETWORK_TCP_LISTENER_H
#define ENGINE_CORE_NETWORK_TCP_LISTENER_H

#include "../types.h"
#include "epoll_reactor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LISTENER_DEFAULT_BACKLOG 128

/* Forward declaration */
struct TcpListener;
typedef struct TcpListener TcpListener;

/**
 * Connection accept callback
 * @param listener   The listener instance
 * @param client_fd  New client socket FD
 * @param client_ip  Client IP address (network byte order)
 * @param client_port Client port (host byte order)
 * @param user_data  User-provided context
 * @return 0 to accept connection, non-zero to reject (closes FD)
 */
typedef int (*ListenerAcceptCallback)(TcpListener* listener, int client_fd,
                                      u32 client_ip, u16 client_port,
                                      void* user_data);

/* Listener structure */
struct TcpListener {
    int                    fd;
    u16                    port;
    u32                    bind_ip;
    EpollReactor*          reactor;
    ListenerAcceptCallback accept_callback;
    void*                  user_data;
    /* Statistics */
    u64                    accept_count;
    u64                    reject_count;
};

/**
 * Initialize TCP listener
 * @param listener  Listener instance
 * @param reactor   Reactor for event handling
 * @return ENGINE_OK on success
 */
int tcp_listener_init(TcpListener* listener, EpollReactor* reactor);

/**
 * Cleanup listener resources
 * @param listener  Listener instance
 */
void tcp_listener_cleanup(TcpListener* listener);

/**
 * Bind and listen on a port
 * @param listener  Listener instance
 * @param bind_ip   IP to bind (0 for INADDR_ANY)
 * @param port      Port number
 * @param backlog   Listen backlog (0 for default)
 * @return ENGINE_OK on success
 */
int tcp_listener_bind(TcpListener* listener, u32 bind_ip, u16 port, int backlog);

/**
 * Set accept callback
 * @param listener  Listener instance
 * @param callback  Callback function
 * @param user_data User context
 */
void tcp_listener_set_callback(TcpListener* listener,
                               ListenerAcceptCallback callback,
                               void* user_data);

/**
 * Get listener socket FD
 * @param listener  Listener instance
 * @return Socket FD or -1
 */
int tcp_listener_get_fd(const TcpListener* listener);

/**
 * Get listener port
 * @param listener  Listener instance
 * @return Port number
 */
u16 tcp_listener_get_port(const TcpListener* listener);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_NETWORK_TCP_LISTENER_H */
