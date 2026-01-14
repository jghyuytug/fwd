/*
 * main.c - Relay Service (P2P/Cross-Server Communication)
 *
 * Relay service (df_relay_r) runs as an independent service on port 7200.
 * It handles:
 * - P2P relay for clients behind NAT
 * - Cross-server message routing
 * - Match queue management
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

/*==============================================================================
 * Configuration
 *============================================================================*/

#define DEFAULT_PORT        7200
#define MAX_EVENTS          256
#define MAX_CLIENTS         1024
#define MAX_SESSIONS        512
#define BUFFER_SIZE         8192
#define PACKET_HEADER_SIZE  13

/* Message types for relay service */
#define MSG_RELAY_REGISTER      1
#define MSG_RELAY_UNREGISTER    2
#define MSG_RELAY_DATA          3
#define MSG_RELAY_PING          4
#define MSG_RELAY_PONG          5
#define MSG_MATCH_QUEUE_JOIN    10
#define MSG_MATCH_QUEUE_LEAVE   11
#define MSG_MATCH_FOUND         12
#define MSG_MATCH_ACCEPT        13
#define MSG_MATCH_DECLINE       14
#define MSG_CROSS_SERVER        20

/*==============================================================================
 * Data Structures
 *============================================================================*/

typedef struct {
    int fd;
    uint32_t client_id;
    uint32_t external_ip;
    uint16_t external_port;
    uint32_t internal_ip;
    uint16_t internal_port;
    uint8_t active;
    time_t last_ping;
    uint8_t recv_buf[BUFFER_SIZE];
    int recv_len;
} RelayClient;

typedef struct {
    uint32_t session_id;
    uint32_t client_a;
    uint32_t client_b;
    uint8_t active;
    time_t created_at;
} RelaySession;

/* Match queue entry */
typedef struct {
    uint32_t client_id;
    uint32_t charac_no;
    uint8_t match_type;    /* 0=dungeon, 1=pvp, 2=abyss */
    uint16_t rating;       /* MMR/ELO rating */
    uint32_t queue_data;   /* dungeon_id or pvp_mode */
    time_t queue_time;
    uint8_t active;
} MatchQueueEntry;

#define MAX_QUEUE_ENTRIES 256

/*==============================================================================
 * Global State
 *============================================================================*/

static volatile int g_running = 1;
static int g_listen_fd = -1;
static int g_epoll_fd = -1;
static uint16_t g_port = DEFAULT_PORT;

static RelayClient g_clients[MAX_CLIENTS];
static int g_client_count = 0;

static RelaySession g_sessions[MAX_SESSIONS];
static int g_session_count = 0;
static uint32_t g_next_session_id = 1;

static MatchQueueEntry g_match_queue[MAX_QUEUE_ENTRIES];

/*==============================================================================
 * Signal Handling
 *============================================================================*/

static void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        printf("[RELAY] Received signal %d, shutting down...\n", signo);
        g_running = 0;
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
}

/*==============================================================================
 * Network Utilities
 *============================================================================*/

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static RelayClient* find_client(int fd) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].active && g_clients[i].fd == fd) {
            return &g_clients[i];
        }
    }
    return NULL;
}

static RelayClient* find_client_by_id(uint32_t client_id) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].active && g_clients[i].client_id == client_id) {
            return &g_clients[i];
        }
    }
    return NULL;
}

static RelayClient* add_client(int fd) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (!g_clients[i].active) {
            memset(&g_clients[i], 0, sizeof(RelayClient));
            g_clients[i].fd = fd;
            g_clients[i].active = 1;
            g_clients[i].last_ping = time(NULL);
            g_client_count++;
            return &g_clients[i];
        }
    }
    return NULL;
}

static void remove_client(RelayClient* c) {
    int i;
    if (!c) return;

    /* Clean up match queue entries */
    for (i = 0; i < MAX_QUEUE_ENTRIES; i++) {
        if (g_match_queue[i].active && g_match_queue[i].client_id == c->client_id) {
            g_match_queue[i].active = 0;
        }
    }

    /* Clean up sessions */
    for (i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active) {
            if (g_sessions[i].client_a == c->client_id ||
                g_sessions[i].client_b == c->client_id) {
                g_sessions[i].active = 0;
                g_session_count--;
            }
        }
    }

    close(c->fd);
    c->active = 0;
    g_client_count--;
}

/*==============================================================================
 * Packet Handling
 *============================================================================*/

static int send_packet(RelayClient* c, uint8_t msg_no, const uint8_t* data, int len) {
    uint8_t packet[BUFFER_SIZE];
    int total_len = PACKET_HEADER_SIZE + len;

    if (total_len > BUFFER_SIZE) return -1;

    memset(packet, 0, PACKET_HEADER_SIZE);
    packet[0] = 0;  /* classification */
    memcpy(packet + 1, &total_len, 4);  /* length */
    packet[5] = msg_no;
    /* checksum and seq are 0 */

    if (len > 0 && data) {
        memcpy(packet + PACKET_HEADER_SIZE, data, len);
    }

    return send(c->fd, packet, total_len, 0);
}

/*==============================================================================
 * Message Handlers
 *============================================================================*/

static int handle_register(RelayClient* c, const uint8_t* data, int len) {
    if (len < 12) return -1;

    memcpy(&c->client_id, data, 4);
    memcpy(&c->external_ip, data + 4, 4);
    memcpy(&c->external_port, data + 8, 2);
    memcpy(&c->internal_ip, data + 10, 4);
    memcpy(&c->internal_port, data + 14, 2);

    printf("[RELAY] Client %u registered (ext=%s:%u)\n",
           c->client_id,
           inet_ntoa(*(struct in_addr*)&c->external_ip),
           c->external_port);

    /* Send acknowledgment */
    uint8_t resp[8];
    uint32_t result = 0;
    uint32_t session_token = rand();
    memcpy(resp, &result, 4);
    memcpy(resp + 4, &session_token, 4);
    return send_packet(c, MSG_RELAY_REGISTER, resp, 8);
}

static int handle_relay_data(RelayClient* c, const uint8_t* data, int len) {
    if (len < 4) return -1;

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    RelayClient* target = find_client_by_id(target_id);
    if (!target) {
        uint8_t resp[4] = {1, 0, 0, 0};  /* target not found */
        return send_packet(c, MSG_RELAY_DATA, resp, 4);
    }

    /* Forward data to target */
    uint8_t fwd[BUFFER_SIZE];
    uint32_t source_id = c->client_id;
    memcpy(fwd, &source_id, 4);
    if (len > 4) {
        memcpy(fwd + 4, data + 4, len - 4);
    }
    return send_packet(target, MSG_RELAY_DATA, fwd, len);
}

static int handle_match_queue_join(RelayClient* c, const uint8_t* data, int len) {
    int i;
    MatchQueueEntry* entry = NULL;
    if (len < 11) return -1;

    uint32_t charac_no;
    uint8_t match_type;
    uint16_t rating;
    uint32_t queue_data;
    memcpy(&charac_no, data, 4);
    match_type = data[4];
    memcpy(&rating, data + 5, 2);
    memcpy(&queue_data, data + 7, 4);

    /* Find empty slot */
    for (i = 0; i < MAX_QUEUE_ENTRIES; i++) {
        if (!g_match_queue[i].active) {
            entry = &g_match_queue[i];
            break;
        }
    }

    if (!entry) {
        uint8_t resp[4] = {2, 0, 0, 0};  /* queue full */
        return send_packet(c, MSG_MATCH_QUEUE_JOIN, resp, 4);
    }

    entry->client_id = c->client_id;
    entry->charac_no = charac_no;
    entry->match_type = match_type;
    entry->rating = rating;
    entry->queue_data = queue_data;
    entry->queue_time = time(NULL);
    entry->active = 1;

    printf("[RELAY] Client %u joined match queue (type=%d, rating=%d)\n",
           c->client_id, match_type, rating);

    /* Attempt to find a match */
    for (i = 0; i < MAX_QUEUE_ENTRIES; i++) {
        MatchQueueEntry* other = &g_match_queue[i];
        if (!other->active || other->client_id == c->client_id) continue;

        /* Match criteria: same type, similar rating (+/- 200), same queue_data */
        if (other->match_type == match_type &&
            other->queue_data == queue_data &&
            abs(other->rating - rating) <= 200) {

            /* Match found! Notify both clients */
            printf("[RELAY] Match found: %u vs %u\n", c->client_id, other->client_id);

            uint8_t match_msg[16];
            uint32_t match_id = g_next_session_id++;
            memcpy(match_msg, &match_id, 4);
            memcpy(match_msg + 4, &other->charac_no, 4);
            memcpy(match_msg + 8, &other->rating, 2);
            match_msg[10] = match_type;

            send_packet(c, MSG_MATCH_FOUND, match_msg, 11);

            memcpy(match_msg + 4, &entry->charac_no, 4);
            memcpy(match_msg + 8, &entry->rating, 2);
            RelayClient* other_client = find_client_by_id(other->client_id);
            if (other_client) {
                send_packet(other_client, MSG_MATCH_FOUND, match_msg, 11);
            }

            /* Remove from queue */
            entry->active = 0;
            other->active = 0;

            uint8_t resp[4] = {0, 0, 0, 0};
            return send_packet(c, MSG_MATCH_QUEUE_JOIN, resp, 4);
        }
    }

    /* No match yet, stay in queue */
    uint8_t resp[4] = {0, 0, 0, 0};
    return send_packet(c, MSG_MATCH_QUEUE_JOIN, resp, 4);
}

static int handle_match_queue_leave(RelayClient* c, const uint8_t* data, int len) {
    int i;
    for (i = 0; i < MAX_QUEUE_ENTRIES; i++) {
        if (g_match_queue[i].active && g_match_queue[i].client_id == c->client_id) {
            g_match_queue[i].active = 0;
            printf("[RELAY] Client %u left match queue\n", c->client_id);
            break;
        }
    }

    uint8_t resp[4] = {0, 0, 0, 0};
    return send_packet(c, MSG_MATCH_QUEUE_LEAVE, resp, 4);
}

static int handle_ping(RelayClient* c, const uint8_t* data, int len) {
    c->last_ping = time(NULL);
    return send_packet(c, MSG_RELAY_PONG, data, len);
}

static int handle_message(RelayClient* c, uint8_t msg_no, const uint8_t* data, int len) {
    switch (msg_no) {
        case MSG_RELAY_REGISTER:
            return handle_register(c, data, len);
        case MSG_RELAY_DATA:
            return handle_relay_data(c, data, len);
        case MSG_RELAY_PING:
            return handle_ping(c, data, len);
        case MSG_MATCH_QUEUE_JOIN:
            return handle_match_queue_join(c, data, len);
        case MSG_MATCH_QUEUE_LEAVE:
            return handle_match_queue_leave(c, data, len);
        default:
            printf("[RELAY] Unknown message %d from client %u\n", msg_no, c->client_id);
            return 0;
    }
}

/*==============================================================================
 * Connection Handling
 *============================================================================*/

static void handle_client_data(RelayClient* c) {
    int n = recv(c->fd, c->recv_buf + c->recv_len, BUFFER_SIZE - c->recv_len, 0);

    if (n <= 0) {
        printf("[RELAY] Client %u disconnected\n", c->client_id);
        remove_client(c);
        return;
    }

    c->recv_len += n;

    /* Process complete packets */
    while (c->recv_len >= PACKET_HEADER_SIZE) {
        uint32_t pkt_len;
        memcpy(&pkt_len, c->recv_buf + 1, 4);

        if (pkt_len > BUFFER_SIZE || pkt_len < PACKET_HEADER_SIZE) {
            printf("[RELAY] Invalid packet length %u\n", pkt_len);
            remove_client(c);
            return;
        }

        if ((uint32_t)c->recv_len < pkt_len) break;

        uint8_t msg_no = c->recv_buf[5];
        const uint8_t* data = c->recv_buf + PACKET_HEADER_SIZE;
        int data_len = pkt_len - PACKET_HEADER_SIZE;

        handle_message(c, msg_no, data, data_len);

        /* Shift buffer */
        c->recv_len -= pkt_len;
        if (c->recv_len > 0) {
            memmove(c->recv_buf, c->recv_buf + pkt_len, c->recv_len);
        }
    }
}

static void accept_connection(void) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept(g_listen_fd, (struct sockaddr*)&addr, &len);

    if (fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("accept");
        }
        return;
    }

    set_nonblocking(fd);

    RelayClient* c = add_client(fd);
    if (!c) {
        printf("[RELAY] Max clients reached, rejecting\n");
        close(fd);
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, fd, &ev);

    printf("[RELAY] New connection from %s (fd=%d)\n",
           inet_ntoa(addr.sin_addr), fd);
}

/*==============================================================================
 * Main Loop
 *============================================================================*/

static int server_init(void) {
    /* Create listening socket */
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    set_nonblocking(g_listen_fd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_port);

    if (bind(g_listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(g_listen_fd, 128) < 0) {
        perror("listen");
        return -1;
    }

    /* Create epoll instance */
    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        perror("epoll_create");
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_listen_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_listen_fd, &ev);

    printf("[RELAY] Server started on port %d\n", g_port);
    return 0;
}

static void cleanup_stale_clients(void) {
    int i;
    time_t now = time(NULL);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].active && now - g_clients[i].last_ping > 60) {
            printf("[RELAY] Client %u timed out\n", g_clients[i].client_id);
            remove_client(&g_clients[i]);
        }
    }
}

static void run_server(void) {
    struct epoll_event events[MAX_EVENTS];
    time_t last_cleanup = time(NULL);
    int i, n;

    while (g_running) {
        n = epoll_wait(g_epoll_fd, events, MAX_EVENTS, 1000);

        for (i = 0; i < n; i++) {
            if (events[i].data.fd == g_listen_fd) {
                accept_connection();
            } else {
                RelayClient* c = find_client(events[i].data.fd);
                if (c) {
                    handle_client_data(c);
                }
            }
        }

        /* Periodic cleanup */
        time_t now = time(NULL);
        if (now - last_cleanup >= 30) {
            cleanup_stale_clients();
            last_cleanup = now;
        }
    }
}

static void server_shutdown(void) {
    int i;
    /* Close all client connections */
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].active) {
            close(g_clients[i].fd);
        }
    }

    if (g_epoll_fd >= 0) close(g_epoll_fd);
    if (g_listen_fd >= 0) close(g_listen_fd);

    printf("[RELAY] Server shutdown complete\n");
}

int main(int argc, char* argv[]) {
    printf("===========================================\n");
    printf("  DNF Relay Service (df_relay_r)\n");
    printf("  P2P Relay & Cross-Server Communication\n");
    printf("===========================================\n");

    if (argc > 1) {
        g_port = atoi(argv[1]);
    }

    setup_signals();
    memset(g_clients, 0, sizeof(g_clients));
    memset(g_sessions, 0, sizeof(g_sessions));
    memset(g_match_queue, 0, sizeof(g_match_queue));

    if (server_init() < 0) {
        return 1;
    }

    run_server();
    server_shutdown();

    return 0;
}
