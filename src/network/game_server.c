/**
 * @file game_server.c
 * @brief DNF Game Server - Real Network Implementation
 *
 * @version 1.0
 * @date 2025-11-26
 */

#include "game_server.h"
#include "dispatcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

/*==============================================================================
 * Internal Constants
 *============================================================================*/

#define MAX_EPOLL_EVENTS    256
#define MAX_PACKET_HANDLERS 256

/*==============================================================================
 * Protocol Mode (0 = legacy 4-byte, 1 = full 13-byte)
 *============================================================================*/
static int g_use_full_header = 0;  /* Default to legacy mode for compatibility */
static uint16_t g_sequence_counter = 0;

/*==============================================================================
 * Module State
 *============================================================================*/

typedef struct {
    int initialized;
    int running;

    /* Sockets */
    int listen_fd;
    int epoll_fd;
    int port;

    /* Sessions */
    ClientSession* sessions;
    int session_count;

    /* Packet handlers */
    PacketHandlerFunc handlers[MAX_PACKET_HANDLERS];

    /* Statistics */
    ServerStats stats;
} ServerState;

static ServerState g_server = {0};

/*==============================================================================
 * Forward Declarations
 *============================================================================*/

static int SetNonBlocking(int fd);
static int HandleNewConnection(void);
static int HandleClientData(ClientSession* session);
static int ProcessPacket(ClientSession* session, const uint8_t* packet, int len);
static void CloseSession(ClientSession* session);
static ClientSession* AllocateSession(int fd);


/*==============================================================================
 * Utility Functions
 *============================================================================*/

static int SetNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

const char* GameServer_GetPacketTypeName(uint16_t type)
{
    static const char* names[] = {
        "CHECK_CONNECTION",     /* 0 */
        "LOGIN",                /* 1 */
        "SET_UDP_IP_PORT",      /* 2 */
        "EXIT",                 /* 3 */
        "SELECT_CHARACTER",     /* 4 */
        "CREATE_CHARACTER",     /* 5 */
        "DELETE_CHARACTER",     /* 6 */
        "RETURN_SELECT_CHAR",   /* 7 */
        "GET_USERINFO",         /* 8 */
        "RECOVER_STAMINA",      /* 9 */
    };

    if (type < sizeof(names) / sizeof(names[0])) {
        return names[type];
    }
    return "UNKNOWN";
}

char* GameServer_FormatIP(uint32_t ip, char* buffer, int size)
{
    struct in_addr addr;
    addr.s_addr = ip;
    snprintf(buffer, size, "%s", inet_ntoa(addr));
    return buffer;
}

/*==============================================================================
 * Protocol Utility Functions
 *============================================================================*/

/* CRC32 lookup table */
static uint32_t crc32_table[256];
static int crc32_table_initialized = 0;

static void InitCRC32Table(void)
{
    if (crc32_table_initialized) return;

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;  /* Standard CRC32 polynomial */
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = 1;
}

uint32_t GameServer_CalculateChecksum(const uint8_t* data, int len)
{
    InitCRC32Table();

    uint32_t crc = 0xFFFFFFFF;
    for (int i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

int GameServer_VerifyChecksum(const PacketHeader* header, const uint8_t* payload, int payload_len)
{
    if (!g_use_full_header) {
        /* Legacy mode doesn't use checksum */
        return 1;
    }

    /* Calculate checksum over: classification + length + msg_no + padding + payload */
    uint8_t temp[PACKET_HEADER_SIZE + PACKET_MAX_SIZE];
    int offset = 0;

    temp[offset++] = header->classification;
    memcpy(temp + offset, &header->length, 4);
    offset += 4;
    temp[offset++] = header->msg_no;
    temp[offset++] = header->padding;

    if (payload && payload_len > 0) {
        memcpy(temp + offset, payload, payload_len);
        offset += payload_len;
    }

    uint32_t calc_checksum = GameServer_CalculateChecksum(temp, offset);
    return (calc_checksum == header->checksum);
}

void GameServer_SetProtocolMode(int use_full_header)
{
    g_use_full_header = use_full_header ? 1 : 0;
    printf("[Server] Protocol mode: %s header (%d bytes)\n",
           g_use_full_header ? "full" : "legacy",
           g_use_full_header ? PACKET_HEADER_SIZE : PACKET_HEADER_SIZE_SIMPLE);
}

int GameServer_GetHeaderSize(void)
{
    return g_use_full_header ? PACKET_HEADER_SIZE : PACKET_HEADER_SIZE_SIMPLE;
}

/*==============================================================================
 * Session Management
 *============================================================================*/

static ClientSession* AllocateSession(int fd)
{
    for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
        if (!g_server.sessions[i].is_active) {
            ClientSession* session = &g_server.sessions[i];
            memset(session, 0, sizeof(ClientSession));
            session->fd = fd;
            session->is_active = 1;
            session->state = SESSION_STATE_CONNECTED;
            session->connect_time = (uint32_t)time(NULL);
            session->last_activity = session->connect_time;
            g_server.session_count++;
            return session;
        }
    }
    return NULL;
}

static void CloseSession(ClientSession* session)
{
    if (!session || !session->is_active) return;

    char ip_str[16];
    printf("[Server] Client disconnected: %s:%d (fd=%d, rx=%lu, tx=%lu)\n",
           GameServer_FormatIP(session->ip_addr, ip_str, sizeof(ip_str)),
           session->port,
           session->fd,
           (unsigned long)session->bytes_received,
           (unsigned long)session->bytes_sent);

    /* Remove from epoll */
    epoll_ctl(g_server.epoll_fd, EPOLL_CTL_DEL, session->fd, NULL);

    /* Close socket */
    close(session->fd);

    /* Update stats */
    g_server.stats.total_bytes_received += session->bytes_received;
    g_server.stats.total_bytes_sent += session->bytes_sent;
    g_server.stats.current_connections--;

    /* Mark inactive */
    session->is_active = 0;
    session->fd = -1;
    g_server.session_count--;
}

ClientSession* GameServer_GetSession(int fd)
{
    for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
        if (g_server.sessions[i].is_active && g_server.sessions[i].fd == fd) {
            return &g_server.sessions[i];
        }
    }
    return NULL;
}

int GameServer_GetSessionCount(void)
{
    return g_server.session_count;
}

void GameServer_ForEachSession(void (*callback)(ClientSession*, void*), void* context)
{
    for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
        if (g_server.sessions[i].is_active) {
            callback(&g_server.sessions[i], context);
        }
    }
}

/*==============================================================================
 * Packet Sending
 *============================================================================*/

int GameServer_SendPacket(ClientSession* session, uint16_t type, const void* data, int len)
{
    if (!session || !session->is_active) return -1;

    int header_size = GameServer_GetHeaderSize();
    int total_len = header_size + len;

    if (total_len > PACKET_MAX_SIZE) {
        fprintf(stderr, "[Server] Packet too large: %d bytes\n", total_len);
        return -1;
    }

    /* Check send buffer space */
    if (session->send_len + total_len > SERVER_SEND_BUFFER_SIZE) {
        fprintf(stderr, "[Server] Send buffer full for fd=%d\n", session->fd);
        return -1;
    }

    uint8_t* buf = session->send_buffer + session->send_len;

    if (g_use_full_header) {
        /* Build full 13-byte header */
        PacketHeader* header = (PacketHeader*)buf;
        header->classification = 0;  /* Default classification */
        header->length = (uint32_t)total_len;
        header->msg_no = (uint8_t)(type & 0xFF);
        header->padding = 0;
        header->sequence = g_sequence_counter++;

        /* Copy payload first */
        if (data && len > 0) {
            memcpy(buf + header_size, data, len);
        }

        /* Calculate checksum over header fields + payload */
        uint8_t temp[PACKET_HEADER_SIZE + PACKET_MAX_SIZE];
        int offset = 0;
        temp[offset++] = header->classification;
        memcpy(temp + offset, &header->length, 4);
        offset += 4;
        temp[offset++] = header->msg_no;
        temp[offset++] = header->padding;
        if (data && len > 0) {
            memcpy(temp + offset, data, len);
            offset += len;
        }
        header->checksum = GameServer_CalculateChecksum(temp, offset);
    } else {
        /* Build legacy 4-byte header */
        PacketHeaderSimple* header = (PacketHeaderSimple*)buf;
        header->length = (uint16_t)total_len;
        header->type = type;

        /* Copy payload */
        if (data && len > 0) {
            memcpy(buf + header_size, data, len);
        }
    }

    session->send_len += total_len;

    /* Try to send immediately */
    while (session->send_pos < session->send_len) {
        int sent = send(session->fd,
                       session->send_buffer + session->send_pos,
                       session->send_len - session->send_pos,
                       MSG_NOSIGNAL);
        if (sent <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Would block, will send later */
                break;
            }
            /* Error */
            return -1;
        }
        session->send_pos += sent;
        session->bytes_sent += sent;
    }

    /* If all sent, reset buffer */
    if (session->send_pos >= session->send_len) {
        session->send_pos = 0;
        session->send_len = 0;
    }

    session->packets_sent++;
    return 0;
}

/*==============================================================================
 * Packet Processing
 *============================================================================*/

static int ProcessPacket(ClientSession* session, const uint8_t* packet, int len)
{
    int header_size = GameServer_GetHeaderSize();

    if (len < header_size) {
        return -1;
    }

    uint16_t pkt_type;
    const uint8_t* payload;
    int payload_len;

    if (g_use_full_header) {
        /* Parse full 13-byte header */
        const PacketHeader* header = (const PacketHeader*)packet;
        pkt_type = header->msg_no;
        payload = packet + header_size;
        payload_len = len - header_size;

        /* Verify checksum */
        if (!GameServer_VerifyChecksum(header, payload, payload_len)) {
            fprintf(stderr, "[Server] Checksum mismatch for packet type=%d\n", pkt_type);
            /* For now, continue processing (may need adjustment based on actual protocol) */
        }

        char ip_str[16];
        printf("[Server] Packet: class=%d, msg=%d (%s), len=%u, seq=%u, from=%s\n",
               header->classification,
               pkt_type,
               GameServer_GetPacketTypeName(pkt_type),
               header->length,
               header->sequence,
               GameServer_FormatIP(session->ip_addr, ip_str, sizeof(ip_str)));
    } else {
        /* Parse legacy 4-byte header */
        const PacketHeaderSimple* header = (const PacketHeaderSimple*)packet;
        pkt_type = header->type;
        payload = packet + header_size;
        payload_len = len - header_size;

        char ip_str[16];
        printf("[Server] Packet: type=%d (%s), len=%d, from=%s\n",
               pkt_type,
               GameServer_GetPacketTypeName(pkt_type),
               len,
               GameServer_FormatIP(session->ip_addr, ip_str, sizeof(ip_str)));
    }

    session->packets_received++;
    session->last_activity = (uint32_t)time(NULL);
    g_server.stats.total_packets_processed++;

    /* Use new dispatcher system */
    return Dispatcher_Dispatch(session, pkt_type, payload, payload_len);
}

static int HandleClientData(ClientSession* session)
{
    /* Receive data */
    int space = SERVER_RECV_BUFFER_SIZE - session->recv_len;
    if (space <= 0) {
        fprintf(stderr, "[Server] Recv buffer full for fd=%d\n", session->fd);
        return -1;
    }

    int received = recv(session->fd,
                       session->recv_buffer + session->recv_len,
                       space,
                       0);

    if (received <= 0) {
        if (received == 0) {
            /* Clean disconnect */
            return -1;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        /* Error */
        return -1;
    }

    session->recv_len += received;
    session->bytes_received += received;

    int header_size = GameServer_GetHeaderSize();

    /* Process complete packets */
    while (session->recv_len >= header_size) {
        uint32_t pkt_len;

        if (g_use_full_header) {
            /* Full 13-byte header: length is at offset 1, 4 bytes */
            const PacketHeader* header = (const PacketHeader*)session->recv_buffer;
            pkt_len = header->length;
        } else {
            /* Legacy 4-byte header: length is at offset 0, 2 bytes */
            const PacketHeaderSimple* header = (const PacketHeaderSimple*)session->recv_buffer;
            pkt_len = header->length;
        }

        /* Validate packet length */
        if (pkt_len < (uint32_t)header_size || pkt_len > PACKET_MAX_SIZE) {
            fprintf(stderr, "[Server] Invalid packet length: %u (header_size=%d)\n",
                    pkt_len, header_size);
            return -1;
        }

        /* Check if complete packet received */
        if ((uint32_t)session->recv_len < pkt_len) {
            /* Wait for more data */
            break;
        }

        /* Process packet */
        if (ProcessPacket(session, session->recv_buffer, pkt_len) < 0) {
            return -1;
        }

        /* Remove processed packet from buffer */
        int remaining = session->recv_len - pkt_len;
        if (remaining > 0) {
            memmove(session->recv_buffer, session->recv_buffer + pkt_len, remaining);
        }
        session->recv_len = remaining;
    }

    return 0;
}

/*==============================================================================
 * Connection Handling
 *============================================================================*/

static int HandleNewConnection(void)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(g_server.listen_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("[Server] accept failed");
        }
        return -1;
    }

    /* Set non-blocking */
    if (SetNonBlocking(client_fd) < 0) {
        close(client_fd);
        return -1;
    }

    /* Set TCP_NODELAY */
    int flag = 1;
    setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    /* Allocate session */
    ClientSession* session = AllocateSession(client_fd);
    if (!session) {
        fprintf(stderr, "[Server] Max connections reached\n");
        close(client_fd);
        return -1;
    }

    session->ip_addr = client_addr.sin_addr.s_addr;
    session->port = ntohs(client_addr.sin_port);

    /* Add to epoll */
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(g_server.epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
        perror("[Server] epoll_ctl ADD failed");
        CloseSession(session);
        return -1;
    }

    /* Update stats */
    g_server.stats.total_connections++;
    g_server.stats.current_connections++;
    if (g_server.stats.current_connections > g_server.stats.peak_connections) {
        g_server.stats.peak_connections = g_server.stats.current_connections;
    }

    char ip_str[16];
    printf("[Server] New connection: %s:%d (fd=%d, total=%d)\n",
           GameServer_FormatIP(session->ip_addr, ip_str, sizeof(ip_str)),
           session->port,
           client_fd,
           g_server.stats.current_connections);

    return 0;
}

/*==============================================================================
 * Public API
 *============================================================================*/

int GameServer_Initialize(int port)
{
    if (g_server.initialized) {
        fprintf(stderr, "[Server] Already initialized\n");
        return -1;
    }

    memset(&g_server, 0, sizeof(ServerState));
    g_server.port = port > 0 ? port : SERVER_DEFAULT_PORT;

    /* Allocate sessions */
    g_server.sessions = calloc(SERVER_MAX_CONNECTIONS, sizeof(ClientSession));
    if (!g_server.sessions) {
        perror("[Server] Failed to allocate sessions");
        return -1;
    }

    /* Create listen socket */
    g_server.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server.listen_fd < 0) {
        perror("[Server] socket failed");
        free(g_server.sessions);
        return -1;
    }

    /* Set socket options */
    int opt = 1;
    setsockopt(g_server.listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_server.port);

    if (bind(g_server.listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[Server] bind failed");
        close(g_server.listen_fd);
        free(g_server.sessions);
        return -1;
    }

    /* Listen */
    if (listen(g_server.listen_fd, SERVER_BACKLOG) < 0) {
        perror("[Server] listen failed");
        close(g_server.listen_fd);
        free(g_server.sessions);
        return -1;
    }

    /* Set non-blocking */
    SetNonBlocking(g_server.listen_fd);

    /* Create epoll */
    g_server.epoll_fd = epoll_create1(0);
    if (g_server.epoll_fd < 0) {
        perror("[Server] epoll_create1 failed");
        close(g_server.listen_fd);
        free(g_server.sessions);
        return -1;
    }

    /* Add listen socket to epoll */
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_server.listen_fd;
    if (epoll_ctl(g_server.epoll_fd, EPOLL_CTL_ADD, g_server.listen_fd, &ev) < 0) {
        perror("[Server] epoll_ctl failed");
        close(g_server.epoll_fd);
        close(g_server.listen_fd);
        free(g_server.sessions);
        return -1;
    }

    /* Initialize dispatcher system */
    if (Dispatcher_Initialize() < 0) {
        fprintf(stderr, "[Server] Failed to initialize dispatcher\n");
        close(g_server.epoll_fd);
        close(g_server.listen_fd);
        free(g_server.sessions);
        return -1;
    }

    g_server.stats.start_time = (uint32_t)time(NULL);
    g_server.initialized = 1;

    printf("[Server] Initialized on port %d\n", g_server.port);
    printf("[Server] Max connections: %d\n", SERVER_MAX_CONNECTIONS);

    /* Print registered handlers (debug) */
    Dispatcher_PrintRegistry();

    return 0;
}

int GameServer_Run(void)
{
    if (!g_server.initialized) {
        fprintf(stderr, "[Server] Not initialized\n");
        return -1;
    }

    g_server.running = 1;
    printf("[Server] Starting main loop...\n");

    struct epoll_event events[MAX_EPOLL_EVENTS];

    while (g_server.running) {
        int nfds = epoll_wait(g_server.epoll_fd, events, MAX_EPOLL_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("[Server] epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == g_server.listen_fd) {
                /* New connection */
                while (HandleNewConnection() == 0) {
                    /* Accept all pending connections */
                }
            } else {
                /* Client data */
                ClientSession* session = GameServer_GetSession(fd);
                if (session) {
                    if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                        CloseSession(session);
                    } else if (events[i].events & EPOLLIN) {
                        if (HandleClientData(session) < 0) {
                            CloseSession(session);
                        }
                    }
                }
            }
        }
    }

    printf("[Server] Main loop ended\n");
    return 0;
}

void GameServer_Stop(void)
{
    g_server.running = 0;
    printf("[Server] Stop requested\n");
}

void GameServer_Cleanup(void)
{
    if (!g_server.initialized) return;

    printf("[Server] Cleaning up...\n");

    /* Close all sessions */
    for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
        if (g_server.sessions[i].is_active) {
            CloseSession(&g_server.sessions[i]);
        }
    }

    /* Cleanup dispatcher */
    Dispatcher_Cleanup();

    /* Close sockets */
    if (g_server.epoll_fd >= 0) close(g_server.epoll_fd);
    if (g_server.listen_fd >= 0) close(g_server.listen_fd);

    /* Free memory */
    free(g_server.sessions);

    /* Print final stats */
    printf("[Server] Final Statistics:\n");
    printf("  Total connections: %u\n", g_server.stats.total_connections);
    printf("  Peak connections:  %u\n", g_server.stats.peak_connections);
    printf("  Bytes received:    %lu\n", (unsigned long)g_server.stats.total_bytes_received);
    printf("  Bytes sent:        %lu\n", (unsigned long)g_server.stats.total_bytes_sent);
    printf("  Packets processed: %lu\n", (unsigned long)g_server.stats.total_packets_processed);
    printf("  Login attempts:    %u\n", g_server.stats.login_attempts);
    printf("  Login successes:   %u\n", g_server.stats.login_successes);

    memset(&g_server, 0, sizeof(ServerState));
    printf("[Server] Cleanup complete\n");
}

void GameServer_GetStats(ServerStats* stats)
{
    if (stats) {
        memcpy(stats, &g_server.stats, sizeof(ServerStats));
    }
}

int GameServer_RegisterHandler(uint16_t packet_type, PacketHandlerFunc handler)
{
    if (packet_type >= MAX_PACKET_HANDLERS) {
        return -1;
    }
    g_server.handlers[packet_type] = handler;
    return 0;
}

void GameServer_DisconnectClient(ClientSession* session, int reason)
{
    (void)reason;
    if (session) {
        CloseSession(session);
    }
}
