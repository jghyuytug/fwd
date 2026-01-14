/*
 * auction_ipc.c - IPC Communication with Game Service
 * Phase 16 P4: Independent Auction Service
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define closesocket close
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

#include "auction_ipc.h"
#include "auction_handler.h"

/* Global Game service connection */
static GameServiceConnection g_game_conn;
static char g_game_host[256] = "127.0.0.1";
static uint16_t g_game_port = 20011;

/*============================================================================
 * Initialization
 *============================================================================*/

int auction_ipc_init(const char* game_host, uint16_t game_port) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[AUCTION_IPC] WSAStartup failed\n");
        return -1;
    }
#endif

    memset(&g_game_conn, 0, sizeof(g_game_conn));
    g_game_conn.fd = -1;
    g_game_conn.state = IPC_STATE_DISCONNECTED;

    if (game_host) {
        strncpy(g_game_host, game_host, sizeof(g_game_host) - 1);
    }
    g_game_port = game_port;

    printf("[AUCTION_IPC] Initialized, target Game service: %s:%d\n",
           g_game_host, g_game_port);

    return 0;
}

void auction_ipc_cleanup(void) {
    auction_ipc_disconnect();

#ifdef _WIN32
    WSACleanup();
#endif

    printf("[AUCTION_IPC] Cleanup complete\n");
}

/*============================================================================
 * Connection Management
 *============================================================================*/

int auction_ipc_connect(void) {
    if (g_game_conn.state == IPC_STATE_CONNECTED) {
        return 0; /* Already connected */
    }

    /* Create socket */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("[AUCTION_IPC] Failed to create socket\n");
        return -1;
    }

    /* Connect to Game service */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_game_port);
    addr.sin_addr.s_addr = inet_addr(g_game_host);

    g_game_conn.state = IPC_STATE_CONNECTING;

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[AUCTION_IPC] Failed to connect to Game service %s:%d\n",
               g_game_host, g_game_port);
        closesocket(fd);
        g_game_conn.state = IPC_STATE_DISCONNECTED;
        return -1;
    }

    g_game_conn.fd = fd;
    g_game_conn.state = IPC_STATE_CONNECTED;
    g_game_conn.last_heartbeat = (uint32_t)time(NULL);

    printf("[AUCTION_IPC] Connected to Game service %s:%d (fd=%d)\n",
           g_game_host, g_game_port, fd);

    return 0;
}

void auction_ipc_disconnect(void) {
    if (g_game_conn.fd >= 0) {
        closesocket(g_game_conn.fd);
        g_game_conn.fd = -1;
    }
    g_game_conn.state = IPC_STATE_DISCONNECTED;
    g_game_conn.recv_len = 0;

    printf("[AUCTION_IPC] Disconnected from Game service\n");
}

int auction_ipc_is_connected(void) {
    return g_game_conn.state == IPC_STATE_CONNECTED;
}

/*============================================================================
 * Send Response
 *============================================================================*/

int auction_ipc_send_response(uint32_t request_id, uint16_t msg_type,
                               const uint8_t* data, int len)
{
    if (!auction_ipc_is_connected()) {
        printf("[AUCTION_IPC] Not connected, cannot send response\n");
        return -1;
    }

    /* Build IPC response packet */
    /* Format: [request_id:4][msg_type:2][data_len:2][data:N] */
    uint8_t packet[65536];
    int offset = 0;

    memcpy(packet + offset, &request_id, 4);
    offset += 4;
    memcpy(packet + offset, &msg_type, 2);
    offset += 2;
    uint16_t data_len = (uint16_t)len;
    memcpy(packet + offset, &data_len, 2);
    offset += 2;

    if (data && len > 0) {
        memcpy(packet + offset, data, len);
        offset += len;
    }

    int sent = send(g_game_conn.fd, (char*)packet, offset, 0);
    if (sent <= 0) {
        printf("[AUCTION_IPC] Send failed\n");
        auction_ipc_disconnect();
        return -1;
    }

    printf("[AUCTION_IPC] Sent response: request_id=%u, msg_type=%d, len=%d\n",
           request_id, msg_type, len);

    return 0;
}

/*============================================================================
 * Process Incoming Messages
 *============================================================================*/

int auction_ipc_process(void) {
    if (!auction_ipc_is_connected()) {
        return 0;
    }

    /* Try to receive data */
    int space = sizeof(g_game_conn.recv_buffer) - g_game_conn.recv_len;
    if (space <= 0) {
        printf("[AUCTION_IPC] Receive buffer full\n");
        return -1;
    }

    /* Non-blocking receive */
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(g_game_conn.fd, FIONBIO, &mode);
#else
    int flags = fcntl(g_game_conn.fd, F_GETFL, 0);
    fcntl(g_game_conn.fd, F_SETFL, flags | O_NONBLOCK);
#endif

    int n = recv(g_game_conn.fd,
                 (char*)(g_game_conn.recv_buffer + g_game_conn.recv_len),
                 space, 0);

    if (n < 0) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            return 0; /* No data available */
        }
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
#endif
        printf("[AUCTION_IPC] Receive error\n");
        auction_ipc_disconnect();
        return -1;
    }

    if (n == 0) {
        printf("[AUCTION_IPC] Game service closed connection\n");
        auction_ipc_disconnect();
        return -1;
    }

    g_game_conn.recv_len += n;

    /* Process complete messages */
    /* IPC Request Format: [player_id:4][request_id:4][msg_type:2][data_len:2][data:N] */
    while (g_game_conn.recv_len >= 12) {
        uint32_t player_id, request_id;
        uint16_t msg_type, data_len;

        memcpy(&player_id, g_game_conn.recv_buffer, 4);
        memcpy(&request_id, g_game_conn.recv_buffer + 4, 4);
        memcpy(&msg_type, g_game_conn.recv_buffer + 8, 2);
        memcpy(&data_len, g_game_conn.recv_buffer + 10, 2);

        int total_len = 12 + data_len;
        if (g_game_conn.recv_len < total_len) {
            break; /* Wait for more data */
        }

        /* Build context */
        AuctionClientContext ctx;
        memset(&ctx, 0, sizeof(ctx));
        ctx.player_id = player_id;
        ctx.request_id = request_id;
        /* Note: player_name and gold should be passed in extended header */

        /* Process message */
        uint8_t response[1024];
        int resp_len = 0;

        auction_dispatch_message(msg_type, &ctx,
                                 g_game_conn.recv_buffer + 12, data_len,
                                 response, &resp_len);

        /* Send response back */
        auction_ipc_send_response(request_id, msg_type, response, resp_len);

        /* Remove processed message from buffer */
        if (g_game_conn.recv_len > total_len) {
            memmove(g_game_conn.recv_buffer,
                    g_game_conn.recv_buffer + total_len,
                    g_game_conn.recv_len - total_len);
        }
        g_game_conn.recv_len -= total_len;
    }

    return 0;
}

/*============================================================================
 * Heartbeat
 *============================================================================*/

int auction_ipc_send_heartbeat(void) {
    if (!auction_ipc_is_connected()) {
        return -1;
    }

    /* Simple heartbeat packet */
    uint8_t heartbeat[8] = {0};
    uint32_t timestamp = (uint32_t)time(NULL);
    memcpy(heartbeat, &timestamp, 4);

    int sent = send(g_game_conn.fd, (char*)heartbeat, 8, 0);
    if (sent <= 0) {
        printf("[AUCTION_IPC] Heartbeat send failed\n");
        auction_ipc_disconnect();
        return -1;
    }

    g_game_conn.last_heartbeat = timestamp;
    return 0;
}

/*============================================================================
 * Monitor Registration
 *============================================================================*/

int auction_ipc_register_with_monitor(const char* monitor_host, uint16_t monitor_port,
                                       const char* service_name, uint16_t service_port)
{
    /* Create temporary connection to Monitor */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("[AUCTION_IPC] Failed to create monitor socket\n");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(monitor_port);
    addr.sin_addr.s_addr = inet_addr(monitor_host);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[AUCTION_IPC] Failed to connect to Monitor %s:%d\n",
               monitor_host, monitor_port);
        closesocket(fd);
        return -1;
    }

    /* Send registration message */
    /* Format: [type:1][name_len:1][name:N][port:2] */
    uint8_t reg[256];
    int offset = 0;
    reg[offset++] = 1; /* Register type */

    size_t name_len = strlen(service_name);
    if (name_len > 32) name_len = 32;
    reg[offset++] = (uint8_t)name_len;
    memcpy(reg + offset, service_name, name_len);
    offset += (int)name_len;

    memcpy(reg + offset, &service_port, 2);
    offset += 2;

    send(fd, (char*)reg, offset, 0);
    closesocket(fd);

    printf("[AUCTION_IPC] Registered with Monitor as '%s' on port %d\n",
           service_name, service_port);

    return 0;
}
