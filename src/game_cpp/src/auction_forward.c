/*
 * auction_forward.c - Auction Service Forwarding for Game Server
 * Phase 16 P4: Forward auction requests to independent Auction service
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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
#endif

#include "../include/auction_forward.h"

/* Connection to Auction service */
typedef struct {
    int fd;
    uint8_t state;
    char host[256];
    uint16_t port;
    uint32_t last_connect_attempt;
    uint8_t recv_buffer[65536];
    int recv_len;
} AuctionConnection;

static AuctionConnection g_auction_conn;

/* Pending request tracking */
typedef struct {
    uint32_t request_id;
    int client_fd;
    uint16_t msg_type;
    uint32_t timestamp;
    uint8_t active;
} PendingRequest;

#define MAX_PENDING_REQUESTS 256
static PendingRequest g_pending_requests[MAX_PENDING_REQUESTS];
static uint32_t g_next_request_id = 1;

/* External: get session by fd (from game server) */
extern ClientSession* get_session_by_fd(int fd);
extern int send_response(ClientSession* s, uint16_t msg_type, const uint8_t* data, int len);

/*============================================================================
 * Initialization
 *============================================================================*/

int auction_forward_init(const char* auction_host, uint16_t auction_port) {
    memset(&g_auction_conn, 0, sizeof(g_auction_conn));
    g_auction_conn.fd = -1;
    g_auction_conn.state = AUCTION_CONN_DISCONNECTED;

    if (auction_host) {
        strncpy(g_auction_conn.host, auction_host, sizeof(g_auction_conn.host) - 1);
    } else {
        strcpy(g_auction_conn.host, "127.0.0.1");
    }
    g_auction_conn.port = auction_port > 0 ? auction_port : 30603;

    memset(g_pending_requests, 0, sizeof(g_pending_requests));
    g_next_request_id = 1;

    printf("[AUCTION_FWD] Initialized, target: %s:%d\n",
           g_auction_conn.host, g_auction_conn.port);

    return 0;
}

void auction_forward_cleanup(void) {
    auction_forward_disconnect();
    printf("[AUCTION_FWD] Cleanup complete\n");
}

/*============================================================================
 * Connection Management
 *============================================================================*/

int auction_forward_connect(void) {
    if (g_auction_conn.state == AUCTION_CONN_CONNECTED) {
        return 0;
    }

    /* Rate limit reconnection attempts */
    uint32_t now = (uint32_t)time(NULL);
    if (now - g_auction_conn.last_connect_attempt < 5) {
        return -1;
    }
    g_auction_conn.last_connect_attempt = now;

    /* Create socket */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("[AUCTION_FWD] Failed to create socket\n");
        return -1;
    }

    /* Connect */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_auction_conn.port);
    addr.sin_addr.s_addr = inet_addr(g_auction_conn.host);

    g_auction_conn.state = AUCTION_CONN_CONNECTING;

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[AUCTION_FWD] Failed to connect to Auction service %s:%d\n",
               g_auction_conn.host, g_auction_conn.port);
        closesocket(fd);
        g_auction_conn.state = AUCTION_CONN_DISCONNECTED;
        return -1;
    }

    /* Set non-blocking */
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(fd, FIONBIO, &mode);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif

    g_auction_conn.fd = fd;
    g_auction_conn.state = AUCTION_CONN_CONNECTED;
    g_auction_conn.recv_len = 0;

    printf("[AUCTION_FWD] Connected to Auction service %s:%d (fd=%d)\n",
           g_auction_conn.host, g_auction_conn.port, fd);

    return 0;
}

void auction_forward_disconnect(void) {
    if (g_auction_conn.fd >= 0) {
        closesocket(g_auction_conn.fd);
        g_auction_conn.fd = -1;
    }
    g_auction_conn.state = AUCTION_CONN_DISCONNECTED;
    g_auction_conn.recv_len = 0;

    /* Clear pending requests */
    for (int i = 0; i < MAX_PENDING_REQUESTS; i++) {
        g_pending_requests[i].active = 0;
    }

    printf("[AUCTION_FWD] Disconnected from Auction service\n");
}

int auction_forward_is_connected(void) {
    return g_auction_conn.state == AUCTION_CONN_CONNECTED;
}

/*============================================================================
 * Check if message should be forwarded
 *============================================================================*/

int auction_should_forward(uint16_t msg_type) {
    /* Auction messages: 170-178 */
    return (msg_type >= 170 && msg_type <= 178);
}

/*============================================================================
 * Forward Request
 *============================================================================*/

int auction_forward_request(ClientSession* s, uint16_t msg_type,
                            const uint8_t* data, int len)
{
    if (!s) return -1;

    /* Try to connect if not connected */
    if (!auction_forward_is_connected()) {
        if (auction_forward_connect() < 0) {
            /* Fall back to local handling */
            printf("[AUCTION_FWD] Cannot forward, Auction service unavailable\n");
            return -1;
        }
    }

    /* Allocate request ID */
    uint32_t request_id = g_next_request_id++;

    /* Find free pending slot */
    PendingRequest* pr = NULL;
    for (int i = 0; i < MAX_PENDING_REQUESTS; i++) {
        if (!g_pending_requests[i].active) {
            pr = &g_pending_requests[i];
            break;
        }
    }

    if (!pr) {
        printf("[AUCTION_FWD] Too many pending requests\n");
        return -1;
    }

    /* Record pending request */
    pr->request_id = request_id;
    pr->client_fd = s->fd;
    pr->msg_type = msg_type;
    pr->timestamp = (uint32_t)time(NULL);
    pr->active = 1;

    /* Build IPC request packet */
    /* Format: [player_id:4][request_id:4][player_name:32][gold:8][msg_type:2][data_len:2][data:N] */
    uint8_t packet[65536];
    int offset = 0;

    memcpy(packet + offset, &s->character_id, 4);
    offset += 4;
    memcpy(packet + offset, &request_id, 4);
    offset += 4;

    /* Player name (32 bytes) */
    char name_buf[32];
    memset(name_buf, 0, 32);
    strncpy(name_buf, s->character_name, 31);
    memcpy(packet + offset, name_buf, 32);
    offset += 32;

    /* Gold (8 bytes) */
    uint64_t gold = s->inventory.money;
    memcpy(packet + offset, &gold, 8);
    offset += 8;

    /* Message type and length */
    memcpy(packet + offset, &msg_type, 2);
    offset += 2;
    uint16_t data_len = (uint16_t)len;
    memcpy(packet + offset, &data_len, 2);
    offset += 2;

    /* Data */
    if (data && len > 0) {
        memcpy(packet + offset, data, len);
        offset += len;
    }

    /* Send to Auction service */
    int sent = send(g_auction_conn.fd, (char*)packet, offset, 0);
    if (sent <= 0) {
        printf("[AUCTION_FWD] Send failed\n");
        auction_forward_disconnect();
        pr->active = 0;
        return -1;
    }

    printf("[AUCTION_FWD] Forwarded msg_type=%d from player %u (request_id=%u)\n",
           msg_type, s->character_id, request_id);

    return 0;
}

/*============================================================================
 * Process Responses
 *============================================================================*/

int auction_forward_process_responses(void) {
    if (!auction_forward_is_connected()) {
        return 0;
    }

    /* Receive data */
    int space = sizeof(g_auction_conn.recv_buffer) - g_auction_conn.recv_len;
    if (space <= 0) {
        return -1;
    }

    int n = recv(g_auction_conn.fd,
                 (char*)(g_auction_conn.recv_buffer + g_auction_conn.recv_len),
                 space, 0);

    if (n < 0) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) return 0;
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
#endif
        printf("[AUCTION_FWD] Recv error\n");
        auction_forward_disconnect();
        return -1;
    }

    if (n == 0) {
        printf("[AUCTION_FWD] Auction service closed connection\n");
        auction_forward_disconnect();
        return -1;
    }

    g_auction_conn.recv_len += n;

    /* Process complete responses */
    /* Response Format: [request_id:4][msg_type:2][data_len:2][data:N] */
    while (g_auction_conn.recv_len >= 8) {
        uint32_t request_id;
        uint16_t msg_type, data_len;

        memcpy(&request_id, g_auction_conn.recv_buffer, 4);
        memcpy(&msg_type, g_auction_conn.recv_buffer + 4, 2);
        memcpy(&data_len, g_auction_conn.recv_buffer + 6, 2);

        int total_len = 8 + data_len;
        if (g_auction_conn.recv_len < total_len) {
            break;
        }

        /* Find pending request */
        PendingRequest* pr = NULL;
        for (int i = 0; i < MAX_PENDING_REQUESTS; i++) {
            if (g_pending_requests[i].active &&
                g_pending_requests[i].request_id == request_id) {
                pr = &g_pending_requests[i];
                break;
            }
        }

        if (pr) {
            /* Find client session and send response */
            ClientSession* s = get_session_by_fd(pr->client_fd);
            if (s && s->active) {
                send_response(s, msg_type, g_auction_conn.recv_buffer + 8, data_len);
                printf("[AUCTION_FWD] Sent response to player fd=%d, msg_type=%d\n",
                       pr->client_fd, msg_type);
            } else {
                printf("[AUCTION_FWD] Client fd=%d no longer active\n", pr->client_fd);
            }

            pr->active = 0;
        } else {
            printf("[AUCTION_FWD] Unknown request_id=%u\n", request_id);
        }

        /* Remove from buffer */
        if (g_auction_conn.recv_len > total_len) {
            memmove(g_auction_conn.recv_buffer,
                    g_auction_conn.recv_buffer + total_len,
                    g_auction_conn.recv_len - total_len);
        }
        g_auction_conn.recv_len -= total_len;
    }

    /* Cleanup expired pending requests (older than 30 seconds) */
    uint32_t now = (uint32_t)time(NULL);
    for (int i = 0; i < MAX_PENDING_REQUESTS; i++) {
        if (g_pending_requests[i].active &&
            now - g_pending_requests[i].timestamp > 30) {
            printf("[AUCTION_FWD] Request %u timed out\n",
                   g_pending_requests[i].request_id);
            g_pending_requests[i].active = 0;
        }
    }

    return 0;
}
