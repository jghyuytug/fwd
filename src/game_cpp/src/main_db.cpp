/*
 * main_db.cpp - DNF Game Server with MySQL Database Integration
 *
 * Extends main.cpp with real database queries:
 * - Account authentication from d_taiwan.accounts
 * - Character list from taiwan_cain.charac_info
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include <zlib.h>

#include "game_server_context.h"
#include "db/db_config.h"
#include "db/db_init.h"
#include "db/db_characters.h"
#include "db/db_economy.h"
#include "db/db_inventory.h"
#include "db/db_skills.h"
#include "db/db_social.h"
#include "db/db_quest.h"
#include "db/db_types.h"
#include "db/db_game.h"
#include "game_packet_utils.h"
#include "handlers/handler_dispatch.h"
#include "handlers/handler_login.h"
#include "handlers/handler_inventory.h"
#include "systems/session_manager.h"
#include "systems/combat_system.h"
#include "systems/buff_system.h"
#include "systems/monster_ai.h"
#include "systems/dungeon_manager.h"
#include "systems/time_utils.h"
#include "packet_dump.h"

/* ORM Adapter - conditional compilation */
#ifdef USE_ORM_ADAPTER
#include "orm_adapter.h"
#endif

/*==============================================================================
 * Configuration
 *============================================================================*/

#define PORT 10011
#define MAX_EVENTS 64
#define BUFFER_SIZE 65536
#define PACKET_HEADER_SIZE 13

#define MAX_SESSIONS 256
#define MAX_CHARAC_NAME 20
#define MAX_ACCOUNT_NAME 256
#define MAX_CHARACTERS 16

/* Shared job identifiers (matches legacy enum in main.cpp) */
#ifndef JOB_SLAYER
#define JOB_SLAYER 0
#endif
#ifndef JOB_FIGHTER
#define JOB_FIGHTER 1
#endif
#ifndef JOB_GUNNER
#define JOB_GUNNER 2
#endif
#ifndef JOB_MAGE
#define JOB_MAGE 3
#endif
#ifndef JOB_PRIEST
#define JOB_PRIEST 4
#endif
#ifndef JOB_THIEF
#define JOB_THIEF 5
#endif
#ifndef JOB_KNIGHT
#define JOB_KNIGHT 6
#endif
#ifndef JOB_DEMONIC_LANCER
#define JOB_DEMONIC_LANCER 7
#endif

/* Database configuration */

/* Macro to suppress "unused function" warnings for functions reserved for future use */
#ifdef __GNUC__
#define UNUSED_FUNC __attribute__((unused))
#else
#define UNUSED_FUNC
#endif


/*==============================================================================
 * Global State
 *============================================================================*/

GameServerState g_server_state = {1, -1, -1, 0, 0};

DBConnectionManager g_db_manager;

/* Database connections */
MYSQL* g_db_account = NULL;
MYSQL* g_db_charac = NULL;
MYSQL* g_db_charac2 = NULL;

static int initialize_orm_manager(const char* config_path) {
    if (!config_path || config_path[0] == '\0') {
        printf("[ORM] Configuration path missing, skipping ORM initialization\n");
        return -1;
    }

    if (DBConnectionManager_Initialize(&g_db_manager, config_path) != 0) {
        printf("[ORM] Failed to initialize DBConnectionManager with %s\n", config_path);
        return -1;
    }

    if (DBConnectionManager_ConnectAll(&g_db_manager) != 0) {
        printf("[ORM] Failed to connect ORM database pools\n");
        DBConnectionManager_Cleanup(&g_db_manager);
        return -1;
    }

    printf("[ORM] Initialized ORM pools via %s\n", config_path);
    return 0;
}
/*==============================================================================
 * Utility Functions
 *============================================================================*/

/* Set socket non-blocking */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*==============================================================================
 * Packet Functions
 *============================================================================*/


int handle_packet(ClientSession* s, const uint8_t* packet, int pkt_len) {
    if (pkt_len < PACKET_HEADER_SIZE) {
        return -1;
    }

    /* Parse header */
    uint8_t classification = packet[0];
    uint32_t length;
    memcpy(&length, packet + 1, 4);
    uint16_t msg_no;
    memcpy(&msg_no, packet + 5, 2);
    uint16_t sequence;
    memcpy(&sequence, packet + 11, 2);

    const uint8_t* data = packet + PACKET_HEADER_SIZE;
    int data_len = pkt_len - PACKET_HEADER_SIZE;

    printf("[GAME] Packet: class=%d len=%u msg=%d seq=%u data_len=%d\n",
           classification, length, msg_no, sequence, data_len);

    s->last_activity = (uint32_t)time(NULL);

    /* Dispatch - Protocol numbers match production binary (df_game_r)
     * Extracted via: strings df_game_r | grep ENUM_CMDPACKET_ | nl -v 0
     */
    return dispatch_message(s, msg_no, data, data_len);
}

/*==============================================================================
 * Client Handler
 *============================================================================*/

int handle_client(ClientSession* s) {
    int n = recv(s->fd, s->recv_buffer + s->recv_len, BUFFER_SIZE - s->recv_len, 0);
    printf("[DEBUG] recv fd=%d n=%d errno=%d\n", s->fd, n, errno);

    /* 原始包落盘 - 记录刚收到的数据 */
    if (n > 0) {
        PACKET_DUMP_RECV(s->recv_buffer + s->recv_len, n, s->fd);
    }

    if (n <= 0) {
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            printf("[GAME] Client disconnected: fd=%d\n", s->fd);
            return -1;
        }
        return 0;
    }

    s->recv_len += n;

    /* Process complete packets */
    int max_iterations = 100;  /* Prevent infinite loop */
    while (s->recv_len >= PACKET_HEADER_SIZE && max_iterations-- > 0) {
        uint8_t pkt_class = s->recv_buffer[0];

        /* Validate class byte - must be 0 or 1 */
        if (pkt_class > 1) {
            /* Invalid packet start, scan for valid header */
            int skip = 1;
            while (skip < s->recv_len && (unsigned char)s->recv_buffer[skip] > 1) {
                skip++;
            }
            if (skip > 0) {
                memmove(s->recv_buffer, s->recv_buffer + skip, s->recv_len - skip);
                s->recv_len -= skip;
            }
            continue;
        }

        uint32_t pkt_len;
        memcpy(&pkt_len, s->recv_buffer + 1, 4);

        /* Validate packet length - reasonable bounds */
        if (pkt_len > 8192 || pkt_len < PACKET_HEADER_SIZE) {
            /* Skip one byte and try to resync */
            memmove(s->recv_buffer, s->recv_buffer + 1, s->recv_len - 1);
            s->recv_len--;
            continue;
        }

        if ((int)pkt_len > s->recv_len) {
            break;  /* Wait for more data */
        }

        int result = handle_packet(s, s->recv_buffer, pkt_len);

        int remaining = s->recv_len - pkt_len;
        if (remaining > 0) {
            memmove(s->recv_buffer, s->recv_buffer + pkt_len, remaining);
        }
        s->recv_len = remaining;

        if (result < 0) {
            return -1;
        }
    }

    return 0;
}

/*==============================================================================
 * Main
 *============================================================================*/

int main(int argc, char* argv[]) {
    setlinebuf(stdout);
    setlinebuf(stderr);

    /* 初始化包落盘功能 */
    packet_dump_init();

    int port = PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("=== DNF Game Server (Database Integrated) ===\n");
    printf("Port: %d\n", port);
    printf("Database: %s@%s\n", DB_USER, DB_HOST);
    printf("============================================\n");
    g_server_state.start_time = (uint32_t)time(NULL);

    /* Initialize ORM pools before legacy DB handles */
    if (initialize_orm_manager(DB_CONFIG_FILE) != 0) {
        printf("[GAME] ORM initialization failed. Aborting startup.\n");
        return 1;
    }

    /* Initialize database */
    if (db_init(DB_CONFIG_FILE) < 0) {
        printf("[GAME] Database initialization failed!\n");
        printf("[GAME] Continuing without database (mock mode)\n");
    }

    /* Initialize sessions */
    memset(g_sessions, 0, sizeof(g_sessions));

    /* Create listen socket */
    g_server_state.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_state.listen_fd < 0) {
        perror("socket");
        db_cleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(g_server_state.listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    set_nonblocking(g_server_state.listen_fd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(g_server_state.listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(g_server_state.listen_fd);
        db_cleanup();
        return 1;
    }

    if (listen(g_server_state.listen_fd, 128) < 0) {
        perror("listen");
        close(g_server_state.listen_fd);
        db_cleanup();
        return 1;
    }

    printf("[GAME] Listening on 0.0.0.0:%d\n", port);

    /* Create epoll */
    g_server_state.epoll_fd = epoll_create(MAX_EVENTS);
    if (g_server_state.epoll_fd < 0) {
        perror("epoll_create");
        close(g_server_state.listen_fd);
        db_cleanup();
        return 1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_server_state.listen_fd;
    epoll_ctl(g_server_state.epoll_fd, EPOLL_CTL_ADD, g_server_state.listen_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    /* Main loop */
    while (g_server_state.running) {
        int nfds = epoll_wait(g_server_state.epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == g_server_state.listen_fd) {
                /* New connection */
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(g_server_state.listen_fd, (struct sockaddr*)&client_addr, &client_len);

                if (client_fd >= 0) {
                    set_nonblocking(client_fd);

                    ClientSession* s = create_session(client_fd,
                                                      client_addr.sin_addr.s_addr,
                                                      ntohs(client_addr.sin_port));
                    if (s) {
                        ev.events = EPOLLIN;
                        ev.data.fd = client_fd;
                        epoll_ctl(g_server_state.epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                        printf("[GAME] New connection: fd=%d from %s:%d (sessions=%d)\n",
                               client_fd,
                               inet_ntoa(client_addr.sin_addr),
                               ntohs(client_addr.sin_port),
                               g_session_count);
                    } else {
                        printf("[GAME] Max sessions reached\n");
                        close(client_fd);
                    }
                }
            } else {
                /* Client data */
                ClientSession* s = find_session(fd);
                if (!s) {
                    epoll_ctl(g_server_state.epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }

                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    printf("[GAME] Client error/hup: fd=%d\n", fd);
                    epoll_ctl(g_server_state.epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    destroy_session(s);
                } else if (events[i].events & EPOLLIN) {
                    if (handle_client(s) < 0) {
                        epoll_ctl(g_server_state.epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        destroy_session(s);
                    }
                }
            }
        }
    }

    close(g_server_state.epoll_fd);
    close(g_server_state.listen_fd);
    db_cleanup();
    printf("[GAME] Server stopped\n");

    return 0;
}
