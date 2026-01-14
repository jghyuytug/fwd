/*
 * main.c - Auction Service Entry Point
 * Phase 16 P4: Independent Auction Service
 *
 * Usage: ./df_auction [port] [game_host] [game_port]
 * Default: port=30603, game_host=127.0.0.1, game_port=20011
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#define sleep(x) Sleep((x) * 1000)
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define closesocket close
#endif

#include "auction_types.h"
#include "auction_manager.h"
#include "auction_handler.h"
#include "auction_ipc.h"

/* Configuration */
#define DEFAULT_PORT 30603
#define DEFAULT_GAME_HOST "127.0.0.1"
#define DEFAULT_GAME_PORT 20011
#define MAX_CLIENTS 64
#define EXPIRE_CHECK_INTERVAL 60  /* Check for expired listings every 60 seconds */
#define HEARTBEAT_INTERVAL 30

/* Client connection (direct connections from Game services) */
typedef struct {
    int fd;
    uint32_t ip;
    uint32_t connect_time;
    uint8_t recv_buffer[65536];
    int recv_len;
    int active;
} AuctionClient;

/* Global state */
static int g_running = 1;
static int g_listen_fd = -1;
static AuctionClient g_clients[MAX_CLIENTS];
static int g_client_count = 0;

/* Signal handler */
void signal_handler(int sig) {
    (void)sig;
    printf("\n[AUCTION] Received shutdown signal\n");
    g_running = 0;
}

/* Initialize listening socket */
int init_server(uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("[AUCTION] Failed to create socket\n");
        return -1;
    }

    /* Reuse address */
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[AUCTION] Failed to bind to port %d\n", port);
        closesocket(fd);
        return -1;
    }

    if (listen(fd, 16) < 0) {
        printf("[AUCTION] Failed to listen\n");
        closesocket(fd);
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

    g_listen_fd = fd;
    printf("[AUCTION] Listening on port %d\n", port);

    return 0;
}

/* Accept new client connection */
void accept_client(void) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(g_listen_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        return;
    }

    /* Find free slot */
    AuctionClient* c = NULL;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!g_clients[i].active) {
            c = &g_clients[i];
            break;
        }
    }

    if (!c) {
        printf("[AUCTION] Max clients reached, rejecting connection\n");
        closesocket(client_fd);
        return;
    }

    /* Initialize client */
    memset(c, 0, sizeof(AuctionClient));
    c->fd = client_fd;
    c->ip = ntohl(client_addr.sin_addr.s_addr);
    c->connect_time = (uint32_t)time(NULL);
    c->active = 1;
    g_client_count++;

    /* Set non-blocking */
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(client_fd, FIONBIO, &mode);
#else
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
#endif

    printf("[AUCTION] New connection from %u.%u.%u.%u (fd=%d)\n",
           (c->ip >> 24) & 0xFF, (c->ip >> 16) & 0xFF,
           (c->ip >> 8) & 0xFF, c->ip & 0xFF, client_fd);
}

/* Process client data */
void process_client(AuctionClient* c) {
    if (!c->active) return;

    /* Receive data */
    int space = sizeof(c->recv_buffer) - c->recv_len;
    if (space <= 0) {
        printf("[AUCTION] Client %d buffer full\n", c->fd);
        return;
    }

    int n = recv(c->fd, (char*)(c->recv_buffer + c->recv_len), space, 0);

    if (n < 0) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) return;
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
#endif
        /* Error */
        printf("[AUCTION] Client %d recv error\n", c->fd);
        closesocket(c->fd);
        c->active = 0;
        g_client_count--;
        return;
    }

    if (n == 0) {
        /* Client disconnected */
        printf("[AUCTION] Client %d disconnected\n", c->fd);
        closesocket(c->fd);
        c->active = 0;
        g_client_count--;
        return;
    }

    c->recv_len += n;

    /* Process messages */
    /* IPC Request Format: [player_id:4][request_id:4][player_name:32][gold:8][msg_type:2][data_len:2][data:N] */
    while (c->recv_len >= 48) {
        uint32_t player_id, request_id;
        char player_name[33];
        uint64_t gold;
        uint16_t msg_type, data_len;

        memcpy(&player_id, c->recv_buffer, 4);
        memcpy(&request_id, c->recv_buffer + 4, 4);
        memcpy(player_name, c->recv_buffer + 8, 32);
        player_name[32] = '\0';
        memcpy(&gold, c->recv_buffer + 40, 8);
        memcpy(&msg_type, c->recv_buffer + 48, 2);
        memcpy(&data_len, c->recv_buffer + 50, 2);

        int total_len = 52 + data_len;
        if (c->recv_len < total_len) {
            break; /* Wait for more data */
        }

        /* Build context */
        AuctionClientContext ctx;
        memset(&ctx, 0, sizeof(ctx));
        ctx.player_id = player_id;
        strncpy(ctx.player_name, player_name, sizeof(ctx.player_name) - 1);
        ctx.gold = gold;
        ctx.request_id = request_id;
        ctx.request_fd = c->fd;

        printf("[AUCTION] Request from player %u (%s), msg_type=%d, len=%d\n",
               player_id, player_name, msg_type, data_len);

        /* Process message */
        uint8_t response[1024];
        int resp_len = 0;

        auction_dispatch_message(msg_type, &ctx,
                                 c->recv_buffer + 52, data_len,
                                 response, &resp_len);

        /* Send response */
        /* Response Format: [request_id:4][msg_type:2][data_len:2][data:N] */
        uint8_t resp_packet[1040];
        int resp_offset = 0;
        memcpy(resp_packet + resp_offset, &request_id, 4);
        resp_offset += 4;
        memcpy(resp_packet + resp_offset, &msg_type, 2);
        resp_offset += 2;
        uint16_t rlen = (uint16_t)resp_len;
        memcpy(resp_packet + resp_offset, &rlen, 2);
        resp_offset += 2;
        if (resp_len > 0) {
            memcpy(resp_packet + resp_offset, response, resp_len);
            resp_offset += resp_len;
        }

        send(c->fd, (char*)resp_packet, resp_offset, 0);

        /* Remove processed message from buffer */
        if (c->recv_len > total_len) {
            memmove(c->recv_buffer, c->recv_buffer + total_len, c->recv_len - total_len);
        }
        c->recv_len -= total_len;
    }
}

/* Main loop */
void run_server(void) {
    uint32_t last_expire_check = (uint32_t)time(NULL);
    uint32_t last_heartbeat = (uint32_t)time(NULL);

    printf("[AUCTION] Entering main loop\n");

    while (g_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        int max_fd = g_listen_fd;
        FD_SET(g_listen_fd, &read_fds);

        /* Add client sockets */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (g_clients[i].active) {
                FD_SET(g_clients[i].fd, &read_fds);
                if (g_clients[i].fd > max_fd) {
                    max_fd = g_clients[i].fd;
                }
            }
        }

        /* Select with 1 second timeout */
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ready < 0) {
            continue;
        }

        /* Check for new connections */
        if (FD_ISSET(g_listen_fd, &read_fds)) {
            accept_client();
        }

        /* Process client data */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (g_clients[i].active && FD_ISSET(g_clients[i].fd, &read_fds)) {
                process_client(&g_clients[i]);
            }
        }

        /* Periodic tasks */
        uint32_t now = (uint32_t)time(NULL);

        /* Check for expired listings */
        if (now - last_expire_check >= EXPIRE_CHECK_INTERVAL) {
            int expired = auction_process_expired();
            if (expired > 0) {
                printf("[AUCTION] Processed %d expired listings\n", expired);
            }
            last_expire_check = now;
        }

        /* Heartbeat/stats */
        if (now - last_heartbeat >= HEARTBEAT_INTERVAL) {
            printf("[AUCTION] Status: %d clients, %d active listings\n",
                   g_client_count, auction_get_active_count());
            last_heartbeat = now;
        }
    }

    printf("[AUCTION] Main loop exited\n");
}

/* Cleanup */
void cleanup(void) {
    /* Close client connections */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].active) {
            closesocket(g_clients[i].fd);
            g_clients[i].active = 0;
        }
    }

    /* Close listen socket */
    if (g_listen_fd >= 0) {
        closesocket(g_listen_fd);
        g_listen_fd = -1;
    }

    /* Cleanup modules */
    auction_ipc_cleanup();
    auction_manager_cleanup();

    printf("[AUCTION] Cleanup complete\n");
}

/* Entry point */
int main(int argc, char** argv) {
    uint16_t port = DEFAULT_PORT;
    const char* game_host = DEFAULT_GAME_HOST;
    uint16_t game_port = DEFAULT_GAME_PORT;

    /* Parse arguments */
    if (argc > 1) {
        port = (uint16_t)atoi(argv[1]);
    }
    if (argc > 2) {
        game_host = argv[2];
    }
    if (argc > 3) {
        game_port = (uint16_t)atoi(argv[3]);
    }

    printf("=====================================================\n");
    printf("  DNF Auction Service\n");
    printf("  Phase 16 P4: Independent Auction Service\n");
    printf("=====================================================\n");
    printf("  Port: %d\n", port);
    printf("  Game Service: %s:%d\n", game_host, game_port);
    printf("=====================================================\n\n");

    /* Install signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize Winsock */
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[AUCTION] WSAStartup failed\n");
        return 1;
    }
#endif

    /* Initialize modules */
    if (auction_manager_init() < 0) {
        printf("[AUCTION] Failed to initialize auction manager\n");
        return 1;
    }

    if (auction_ipc_init(game_host, game_port) < 0) {
        printf("[AUCTION] Failed to initialize IPC\n");
        auction_manager_cleanup();
        return 1;
    }

    /* Initialize server */
    if (init_server(port) < 0) {
        printf("[AUCTION] Failed to initialize server\n");
        cleanup();
        return 1;
    }

    memset(g_clients, 0, sizeof(g_clients));

    /* Run main loop */
    run_server();

    /* Cleanup */
    cleanup();

#ifdef _WIN32
    WSACleanup();
#endif

    printf("[AUCTION] Shutdown complete\n");
    return 0;
}
