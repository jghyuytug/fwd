/*
 * main.c - Guild Service Main Entry Point
 *
 * Guild service (df_guild_r) runs as an independent service on port 30403.
 * It handles all guild operations across the server cluster.
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
#include <mysql/mysql.h>

#include "guild.h"

/*==============================================================================
 * Configuration
 *============================================================================*/

#define DEFAULT_PORT        30403
#define MAX_EVENTS          256
#define MAX_GAME_SERVERS    16
#define BUFFER_SIZE         8192

/* Database configuration */
#define DB_HOST "192.168.200.131"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_PASS "88888888"
#define DB_NAME "d_guild"

/*==============================================================================
 * Global State
 *============================================================================*/

static volatile int g_running = 1;
static int g_listen_fd = -1;
static int g_epoll_fd = -1;
static uint16_t g_port = DEFAULT_PORT;
static MYSQL* g_db = NULL;

/* Connected game servers */
typedef struct {
    int fd;
    uint32_t ip;
    uint16_t port;
    uint8_t active;
    char name[32];
} GameServerConnection;

static GameServerConnection g_game_servers[MAX_GAME_SERVERS];
static int g_game_server_count = 0;

/*==============================================================================
 * Signal Handling
 *============================================================================*/

static void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        printf("[GUILD] Received signal %d, shutting down...\n", signo);
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
 * Database Functions
 *============================================================================*/

static int db_init(void) {
    printf("[GUILD] Connecting to database %s...\n", DB_NAME);

    g_db = mysql_init(NULL);
    if (!g_db) {
        printf("[GUILD] Failed to init MySQL handle\n");
        return -1;
    }

    unsigned int timeout = 5;
    mysql_options(g_db, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db, DB_HOST, DB_USER, DB_PASS,
                            DB_NAME, DB_PORT, NULL, 0)) {
        printf("[GUILD] Database connect failed: %s\n", mysql_error(g_db));
        mysql_close(g_db);
        g_db = NULL;
        return -1;
    }

    mysql_set_character_set(g_db, "utf8");
    printf("[GUILD] Database connected successfully\n");
    return 0;
}

static void db_cleanup(void) {
    if (g_db) {
        mysql_close(g_db);
        g_db = NULL;
    }
}

/*==============================================================================
 * Network Functions
 *============================================================================*/

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int init_server(void) {
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        printf("[GUILD] Failed to create socket: %s\n", strerror(errno));
        return -1;
    }

    int opt = 1;
    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_port);

    if (bind(g_listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[GUILD] Failed to bind port %d: %s\n", g_port, strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    if (listen(g_listen_fd, 32) < 0) {
        printf("[GUILD] Failed to listen: %s\n", strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    set_nonblocking(g_listen_fd);

    /* Create epoll instance */
    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        printf("[GUILD] Failed to create epoll: %s\n", strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_listen_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_listen_fd, &ev);

    printf("[GUILD] Server listening on port %d\n", g_port);
    return 0;
}

static void cleanup_server(void) {
    if (g_epoll_fd >= 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }
    if (g_listen_fd >= 0) {
        close(g_listen_fd);
        g_listen_fd = -1;
    }

    /* Close all game server connections */
    for (int i = 0; i < MAX_GAME_SERVERS; i++) {
        if (g_game_servers[i].active && g_game_servers[i].fd >= 0) {
            close(g_game_servers[i].fd);
            g_game_servers[i].active = 0;
        }
    }
}

/*==============================================================================
 * Connection Handling
 *============================================================================*/

static void accept_connection(void) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(g_listen_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("[GUILD] Accept failed: %s\n", strerror(errno));
        }
        return;
    }

    set_nonblocking(client_fd);

    /* Find free slot for game server connection */
    int slot = -1;
    for (int i = 0; i < MAX_GAME_SERVERS; i++) {
        if (!g_game_servers[i].active) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        printf("[GUILD] Max game server connections reached\n");
        close(client_fd);
        return;
    }

    g_game_servers[slot].fd = client_fd;
    g_game_servers[slot].ip = client_addr.sin_addr.s_addr;
    g_game_servers[slot].port = ntohs(client_addr.sin_port);
    g_game_servers[slot].active = 1;
    g_game_server_count++;

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

    printf("[GUILD] Game server connected from %s:%d (slot %d)\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), slot);
}

static GameServerConnection* find_game_server(int fd) {
    for (int i = 0; i < MAX_GAME_SERVERS; i++) {
        if (g_game_servers[i].active && g_game_servers[i].fd == fd) {
            return &g_game_servers[i];
        }
    }
    return NULL;
}

static void close_game_server(int fd) {
    GameServerConnection* gs = find_game_server(fd);
    if (gs) {
        printf("[GUILD] Game server disconnected (fd=%d)\n", fd);
        epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        gs->active = 0;
        gs->fd = -1;
        g_game_server_count--;
    }
}

/*==============================================================================
 * Message Processing
 *============================================================================*/

static int send_response(int fd, uint8_t msg_type, uint32_t result,
                         const uint8_t* data, int data_len) {
    uint8_t buffer[BUFFER_SIZE];
    int offset = 0;

    /* Header: msg_type (1) + result (4) */
    buffer[offset++] = msg_type;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    /* Payload */
    if (data && data_len > 0) {
        memcpy(buffer + offset, data, data_len);
        offset += data_len;
    }

    return send(fd, buffer, offset, 0);
}

static void process_message(int fd, const uint8_t* data, int len) {
    if (len < 5) return;  /* Minimum: msg_type (1) + some data (4) */

    uint8_t msg_type = data[0];
    const uint8_t* payload = data + 1;
    int payload_len = len - 1;

    printf("[GUILD] Received message type=%d from fd=%d\n", msg_type, fd);

    switch (msg_type) {
        case GMSG_GET_GUILD_INFO: {
            if (payload_len < 4) break;
            uint32_t guild_id;
            memcpy(&guild_id, payload, 4);

            GuildInfo info;
            int result = guild_get_info(guild_id, &info);

            uint8_t response[512];
            int resp_len = 0;
            if (result == 0) {
                memcpy(response + resp_len, &info.guild_id, 4); resp_len += 4;
                memcpy(response + resp_len, info.guild_name, 32); resp_len += 32;
                memcpy(response + resp_len, info.master_name, 21); resp_len += 21;
                memcpy(response + resp_len, &info.level, 4); resp_len += 4;
                memcpy(response + resp_len, &info.member_count, 4); resp_len += 4;
            }
            send_response(fd, msg_type, result, response, resp_len);
            break;
        }

        case GMSG_CREATE_GUILD: {
            if (payload_len < 57) break;  /* master_id(4) + master_charac(4) + name(32) + master_name(17) */
            uint32_t master_id, master_charac;
            char guild_name[33], master_name[22];

            memcpy(&master_id, payload, 4);
            memcpy(&master_charac, payload + 4, 4);
            memcpy(guild_name, payload + 8, 32);
            guild_name[32] = '\0';
            memcpy(master_name, payload + 40, 21);
            master_name[21] = '\0';

            uint32_t new_guild_id = 0;
            int result = guild_create(master_id, master_charac, master_name,
                                       guild_name, &new_guild_id);

            uint8_t response[8];
            memcpy(response, &new_guild_id, 4);
            send_response(fd, msg_type, result, response, 4);
            break;
        }

        case GMSG_JOIN_GUILD: {
            if (payload_len < 12) break;
            uint32_t guild_id, charac_no;
            char charac_name[22];

            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            memcpy(charac_name, payload + 8, 21);
            charac_name[21] = '\0';

            int result = guild_add_member(guild_id, charac_no, charac_name,
                                           GUILD_RANK_MEMBER);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_LEAVE_GUILD: {
            if (payload_len < 8) break;
            uint32_t guild_id, charac_no;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);

            int result = guild_remove_member(guild_id, charac_no);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_DISBAND_GUILD: {
            if (payload_len < 8) break;
            uint32_t guild_id, requester_id;
            memcpy(&guild_id, payload, 4);
            memcpy(&requester_id, payload + 4, 4);

            int result = guild_disband(guild_id, requester_id);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_GET_MEMBER_LIST: {
            if (payload_len < 4) break;
            uint32_t guild_id;
            memcpy(&guild_id, payload, 4);

            GuildMember members[GUILD_MAX_MEMBERS];
            int count = guild_get_members(guild_id, members, GUILD_MAX_MEMBERS);

            uint8_t response[2048];
            int resp_len = 0;

            uint8_t member_count = (count > 0) ? (uint8_t)count : 0;
            response[resp_len++] = member_count;

            for (int i = 0; i < member_count && i < 50; i++) {
                memcpy(response + resp_len, &members[i].charac_no, 4); resp_len += 4;
                memcpy(response + resp_len, members[i].charac_name, 21); resp_len += 21;
                response[resp_len++] = members[i].rank;
                response[resp_len++] = members[i].is_online;
            }

            send_response(fd, msg_type, 0, response, resp_len);
            break;
        }

        case GMSG_CHANGE_RANK: {
            if (payload_len < 9) break;
            uint32_t guild_id, charac_no;
            int8_t new_rank;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            new_rank = (int8_t)payload[8];

            int result = guild_change_member_rank(guild_id, charac_no, new_rank);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_MEMBER_ONLINE: {
            if (payload_len < 8) break;
            uint32_t guild_id, charac_no;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);

            guild_set_member_online(guild_id, charac_no);
            /* No response needed */
            break;
        }

        case GMSG_MEMBER_OFFLINE: {
            if (payload_len < 8) break;
            uint32_t guild_id, charac_no;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);

            guild_set_member_offline(guild_id, charac_no);
            /* No response needed */
            break;
        }

        case GMSG_ADD_EXP: {
            if (payload_len < 8) break;
            uint32_t guild_id, exp_amount;
            memcpy(&guild_id, payload, 4);
            memcpy(&exp_amount, payload + 4, 4);

            uint8_t new_level = 0;
            uint8_t leveled_up = 0;
            int result = guild_add_exp_full(guild_id, exp_amount, &new_level, &leveled_up);

            uint8_t response[8];
            int resp_len = 0;
            memcpy(response + resp_len, &exp_amount, 4); resp_len += 4;
            response[resp_len++] = new_level;
            response[resp_len++] = leveled_up;

            send_response(fd, msg_type, result, response, resp_len);
            printf("[GUILD] ADD_EXP guild=%u exp=%u new_level=%d leveled_up=%d\n",
                   guild_id, exp_amount, new_level, leveled_up);
            break;
        }

        case GMSG_UPDATE_NOTICE: {
            if (payload_len < 4) break;
            uint32_t guild_id;
            memcpy(&guild_id, payload, 4);
            const char* notice = (const char*)(payload + 4);

            int result = guild_set_notice(guild_id, notice);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_UPDATE_INTRO: {
            if (payload_len < 4) break;
            uint32_t guild_id;
            memcpy(&guild_id, payload, 4);
            const char* intro = (const char*)(payload + 4);

            int result = guild_set_introduction(guild_id, intro);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_KICK_MEMBER: {
            if (payload_len < 12) break;
            uint32_t guild_id, requester_id, target_id;
            memcpy(&guild_id, payload, 4);
            memcpy(&requester_id, payload + 4, 4);
            memcpy(&target_id, payload + 8, 4);

            /* Check requester has permission */
            int8_t requester_rank = guild_get_member_rank(guild_id, requester_id);
            int8_t target_rank = guild_get_member_rank(guild_id, target_id);

            int result = -1;
            if (requester_rank >= 0 && requester_rank <= GUILD_RANK_VICE_MASTER &&
                target_rank > requester_rank) {
                result = guild_remove_member(guild_id, target_id);
            }
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_LEARN_SKILL: {
            if (payload_len < 6) break;
            uint32_t guild_id;
            uint16_t skill_id;
            memcpy(&guild_id, payload, 4);
            memcpy(&skill_id, payload + 4, 2);

            int result = guild_learn_skill(guild_id, skill_id);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_DEPOSIT_ITEM: {
            if (payload_len < 16) break;
            uint32_t guild_id, charac_no, item_id;
            int32_t count;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            memcpy(&item_id, payload + 8, 4);
            memcpy(&count, payload + 12, 4);

            int32_t slot = -1;
            int result = guild_deposit_item(guild_id, charac_no, item_id, count, &slot);

            uint8_t response[4];
            memcpy(response, &slot, 4);
            send_response(fd, msg_type, result, response, 4);
            break;
        }

        case GMSG_WITHDRAW_ITEM: {
            if (payload_len < 16) break;
            uint32_t guild_id, charac_no;
            int32_t slot, count;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            memcpy(&slot, payload + 8, 4);
            memcpy(&count, payload + 12, 4);

            int result = guild_withdraw_item(guild_id, charac_no, slot, count);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_DEPOSIT_FUND: {
            if (payload_len < 12) break;
            uint32_t guild_id, charac_no, amount;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            memcpy(&amount, payload + 8, 4);

            int result = guild_deposit_fund(guild_id, charac_no, amount);

            uint8_t response[4];
            memcpy(response, &amount, 4);
            send_response(fd, msg_type, result, response, 4);
            break;
        }

        case GMSG_WITHDRAW_FUND: {
            if (payload_len < 12) break;
            uint32_t guild_id, charac_no, amount;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);
            memcpy(&amount, payload + 8, 4);

            int result = guild_withdraw_fund(guild_id, charac_no, amount);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_GET_JOIN_REQUESTS: {
            if (payload_len < 4) break;
            uint32_t guild_id;
            memcpy(&guild_id, payload, 4);

            GuildJoinRequest requests[50];
            int count = guild_get_join_requests(guild_id, requests, 50);

            uint8_t response[2048];
            int resp_len = 0;
            uint8_t req_count = (count > 0) ? (uint8_t)count : 0;
            response[resp_len++] = req_count;

            for (int i = 0; i < req_count && i < 50; i++) {
                memcpy(response + resp_len, &requests[i].charac_no, 4); resp_len += 4;
                memcpy(response + resp_len, requests[i].charac_name, 21); resp_len += 21;
                response[resp_len++] = requests[i].level;
                response[resp_len++] = requests[i].job;
            }

            send_response(fd, msg_type, 0, response, resp_len);
            break;
        }

        case GMSG_ACCEPT_JOIN: {
            if (payload_len < 8) break;
            uint32_t guild_id, charac_no;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);

            int result = guild_accept_join(guild_id, charac_no);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_REJECT_JOIN: {
            if (payload_len < 8) break;
            uint32_t guild_id, charac_no;
            memcpy(&guild_id, payload, 4);
            memcpy(&charac_no, payload + 4, 4);

            int result = guild_reject_join(guild_id, charac_no);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case GMSG_SEARCH_GUILD: {
            const char* keyword = (const char*)payload;
            GuildInfo results[20];
            int count = guild_search(keyword, results, 20);

            uint8_t response[2048];
            int resp_len = 0;
            uint8_t guild_count = (count > 0) ? (uint8_t)count : 0;
            response[resp_len++] = guild_count;

            for (int i = 0; i < guild_count && i < 20; i++) {
                memcpy(response + resp_len, &results[i].guild_id, 4); resp_len += 4;
                memcpy(response + resp_len, results[i].guild_name, 32); resp_len += 32;
                memcpy(response + resp_len, &results[i].level, 4); resp_len += 4;
                memcpy(response + resp_len, &results[i].member_count, 4); resp_len += 4;
            }

            send_response(fd, msg_type, 0, response, resp_len);
            break;
        }

        case GMSG_BROADCAST: {
            if (payload_len < 8) break;
            uint32_t guild_id;
            uint16_t msg_len;
            memcpy(&guild_id, payload, 4);
            memcpy(&msg_len, payload + 4, 2);

            /* Broadcast to all connected game servers */
            for (int i = 0; i < MAX_GAME_SERVERS; i++) {
                if (g_game_servers[i].active && g_game_servers[i].fd != fd) {
                    send(g_game_servers[i].fd, payload - 1, payload_len + 1, 0);
                }
            }
            printf("[GUILD] Broadcast to guild %u, msg_len=%d\n", guild_id, msg_len);
            break;
        }

        default:
            printf("[GUILD] Unknown message type: %d\n", msg_type);
            send_response(fd, msg_type, -1, NULL, 0);
            break;
    }
}

static void handle_data(int fd) {
    uint8_t buffer[BUFFER_SIZE];
    ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

    if (n <= 0) {
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return;
        }
        close_game_server(fd);
        return;
    }

    process_message(fd, buffer, (int)n);
}

/*==============================================================================
 * Guild Service Implementation Stubs
 * (Will be fully implemented in guild.c)
 *============================================================================*/

int guild_service_init(const char* config_file) {
    (void)config_file;
    memset(g_game_servers, 0, sizeof(g_game_servers));
    return db_init();
}

void guild_service_cleanup(void) {
    db_cleanup();
    cleanup_server();
}

/* Basic guild operations using database */
int guild_get_info(uint32_t guild_id, GuildInfo* info) {
    if (!g_db || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, server_id, guild_name, master_id, master_no, master_name, "
             "lev, guild_exp, guild_point, member_count "
             "FROM guild_info WHERE guild_id=%u AND expire_flag=0 LIMIT 1", guild_id);

    if (mysql_query(g_db, query)) {
        printf("[GUILD] Query failed: %s\n", mysql_error(g_db));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    memset(info, 0, sizeof(GuildInfo));
    info->guild_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    info->server_id = row[1] ? (int8_t)atoi(row[1]) : 0;
    if (row[2]) strncpy(info->guild_name, row[2], GUILD_MAX_NAME_LEN);
    info->master_id = row[3] ? (uint32_t)atoi(row[3]) : 0;
    info->master_charac_no = row[4] ? (uint32_t)atoi(row[4]) : 0;
    if (row[5]) strncpy(info->master_name, row[5], 20);
    info->level = row[6] ? (int32_t)atoi(row[6]) : 1;
    info->exp = row[7] ? (uint32_t)atoi(row[7]) : 0;
    info->guild_point = row[8] ? (uint32_t)atoi(row[8]) : 0;
    info->member_count = row[9] ? (int32_t)atoi(row[9]) : 0;
    info->status = GUILD_STATUS_ACTIVE;

    mysql_free_result(res);
    printf("[GUILD] Got info for guild %u: %s (lv%d, %d members)\n",
           info->guild_id, info->guild_name, info->level, info->member_count);
    return 0;
}

int guild_create(uint32_t master_id, uint32_t master_charac_no,
                 const char* master_name, const char* guild_name,
                 uint32_t* out_guild_id) {
    if (!g_db || !guild_name || !master_name || !out_guild_id) return -1;

    /* Check if guild name exists */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id FROM guild_info WHERE guild_name='%s' AND expire_flag=0 LIMIT 1",
             guild_name);

    if (mysql_query(g_db, query) == 0) {
        MYSQL_RES* res = mysql_store_result(g_db);
        if (res) {
            if (mysql_fetch_row(res)) {
                mysql_free_result(res);
                printf("[GUILD] Guild name '%s' already exists\n", guild_name);
                return -2;  /* Name exists */
            }
            mysql_free_result(res);
        }
    }

    /* Create guild */
    snprintf(query, sizeof(query),
             "INSERT INTO guild_info (server_id, guild_name, master_id, master_no, master_name, "
             "lev, guild_exp, guild_point, member_count, create_time) "
             "VALUES (1, '%s', %u, %u, '%s', 1, 0, 0, 1, NOW())",
             guild_name, master_id, master_charac_no, master_name);

    if (mysql_query(g_db, query)) {
        printf("[GUILD] Create guild failed: %s\n", mysql_error(g_db));
        return -1;
    }

    *out_guild_id = (uint32_t)mysql_insert_id(g_db);

    /* Add master as member */
    snprintf(query, sizeof(query),
             "INSERT INTO guild_member (guild_id, charac_no, charac_name, guild_right, reg_date) "
             "VALUES (%u, %u, '%s', %d, NOW())",
             *out_guild_id, master_charac_no, master_name, GUILD_RANK_MASTER);

    mysql_query(g_db, query);

    printf("[GUILD] Created guild '%s' (id=%u) by %s\n", guild_name, *out_guild_id, master_name);
    return 0;
}

int guild_disband(uint32_t guild_id, uint32_t requester_id) {
    if (!g_db) return -1;
    (void)requester_id;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET expire_flag=1 WHERE guild_id=%u", guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    printf("[GUILD] Disbanded guild %u\n", guild_id);
    return 0;
}

int guild_add_member(uint32_t guild_id, uint32_t charac_no,
                     const char* charac_name, int8_t rank) {
    if (!g_db || !charac_name) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO guild_member (guild_id, charac_no, charac_name, guild_right, reg_date) "
             "VALUES (%u, %u, '%s', %d, NOW())",
             guild_id, charac_no, charac_name, rank);

    if (mysql_query(g_db, query)) {
        printf("[GUILD] Add member failed: %s\n", mysql_error(g_db));
        return -1;
    }

    /* Update member count */
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET member_count=member_count+1 WHERE guild_id=%u", guild_id);
    mysql_query(g_db, query);

    printf("[GUILD] Added member %s to guild %u\n", charac_name, guild_id);
    return 0;
}

int guild_remove_member(uint32_t guild_id, uint32_t charac_no) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM guild_member WHERE guild_id=%u AND charac_no=%u",
             guild_id, charac_no);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    if (mysql_affected_rows(g_db) > 0) {
        snprintf(query, sizeof(query),
                 "UPDATE guild_info SET member_count=member_count-1 WHERE guild_id=%u", guild_id);
        mysql_query(g_db, query);
    }

    printf("[GUILD] Removed member %u from guild %u\n", charac_no, guild_id);
    return 0;
}

int guild_change_member_rank(uint32_t guild_id, uint32_t charac_no, int8_t new_rank) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET guild_right=%d WHERE guild_id=%u AND charac_no=%u",
             new_rank, guild_id, charac_no);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    return 0;
}

int guild_get_members(uint32_t guild_id, GuildMember* members, int max_count) {
    if (!g_db || !members) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, guild_right FROM guild_member "
             "WHERE guild_id=%u LIMIT %d", guild_id, max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        memset(&members[count], 0, sizeof(GuildMember));
        members[count].charac_no = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (row[1]) strncpy(members[count].charac_name, row[1], 20);
        members[count].rank = row[2] ? (int8_t)atoi(row[2]) : GUILD_RANK_MEMBER;
        members[count].guild_id = guild_id;
        count++;
    }

    mysql_free_result(res);
    return count;
}

int guild_set_member_online(uint32_t guild_id, uint32_t charac_no) {
    /* In-memory tracking - would update a cache structure */
    printf("[GUILD] Member %u online in guild %u\n", charac_no, guild_id);
    return 0;
}

int guild_set_member_offline(uint32_t guild_id, uint32_t charac_no) {
    printf("[GUILD] Member %u offline in guild %u\n", charac_no, guild_id);
    return 0;
}

/*==============================================================================
 * Guild Level System
 *============================================================================*/

/* Guild exp required for each level (1-20) */
static const uint32_t GUILD_EXP_TABLE[21] = {
    0,        /* Level 0 (unused) */
    0,        /* Level 1 */
    1000,     /* Level 2 */
    3000,     /* Level 3 */
    6000,     /* Level 4 */
    10000,    /* Level 5 */
    15000,    /* Level 6 */
    21000,    /* Level 7 */
    28000,    /* Level 8 */
    36000,    /* Level 9 */
    45000,    /* Level 10 */
    60000,    /* Level 11 */
    80000,    /* Level 12 */
    105000,   /* Level 13 */
    135000,   /* Level 14 */
    170000,   /* Level 15 */
    210000,   /* Level 16 */
    255000,   /* Level 17 */
    305000,   /* Level 18 */
    360000,   /* Level 19 */
    420000    /* Level 20 (max) */
};

int guild_get_exp_for_level(int level) {
    if (level < 1) return 0;
    if (level > 20) return GUILD_EXP_TABLE[20];
    return (int)GUILD_EXP_TABLE[level];
}

int guild_get_max_members_for_level(int level) {
    /* Base 20 members, +2 per level up to 60 max */
    int max = 20 + (level - 1) * 2;
    if (max > 60) max = 60;
    return max;
}

int guild_add_exp(uint32_t guild_id, uint32_t exp) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET guild_exp=guild_exp+%u WHERE guild_id=%u",
             exp, guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    printf("[GUILD] Added %u exp to guild %u\n", exp, guild_id);
    return 0;
}

/* Full version with level-up check */
int guild_add_exp_full(uint32_t guild_id, uint32_t exp, uint8_t* out_level, uint8_t* out_leveled_up) {
    if (!g_db) return -1;

    *out_level = 1;
    *out_leveled_up = 0;

    /* Get current exp and level */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT lev, guild_exp FROM guild_info WHERE guild_id=%u LIMIT 1", guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    int current_level = row[0] ? atoi(row[0]) : 1;
    uint32_t current_exp = row[1] ? (uint32_t)atoi(row[1]) : 0;
    mysql_free_result(res);

    /* Add exp */
    uint32_t new_exp = current_exp + exp;
    int new_level = current_level;

    /* Check for level up */
    while (new_level < 20 && new_exp >= GUILD_EXP_TABLE[new_level + 1]) {
        new_level++;
        *out_leveled_up = 1;
    }

    *out_level = (uint8_t)new_level;

    /* Update database */
    if (*out_leveled_up) {
        int new_max_members = guild_get_max_members_for_level(new_level);
        snprintf(query, sizeof(query),
                 "UPDATE guild_info SET guild_exp=%u, lev=%d, max_member=%d WHERE guild_id=%u",
                 new_exp, new_level, new_max_members, guild_id);
        printf("[GUILD] Guild %u leveled up to %d!\n", guild_id, new_level);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE guild_info SET guild_exp=%u WHERE guild_id=%u", new_exp, guild_id);
    }

    mysql_query(g_db, query);
    return 0;
}

int guild_add_point(uint32_t guild_id, uint32_t point) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET guild_point=guild_point+%u WHERE guild_id=%u",
             point, guild_id);

    mysql_query(g_db, query);
    return 0;
}

int guild_check_level_up(uint32_t guild_id) {
    uint8_t level, leveled_up;
    return guild_add_exp_full(guild_id, 0, &level, &leveled_up);
}

/*==============================================================================
 * Guild Membership Extended
 *============================================================================*/

int8_t guild_get_member_rank(uint32_t guild_id, uint32_t charac_no) {
    if (!g_db) return GUILD_RANK_NONE;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT guild_right FROM guild_member WHERE guild_id=%u AND charac_no=%u LIMIT 1",
             guild_id, charac_no);

    if (mysql_query(g_db, query)) {
        return GUILD_RANK_NONE;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return GUILD_RANK_NONE;

    MYSQL_ROW row = mysql_fetch_row(res);
    int8_t rank = row ? (int8_t)atoi(row[0]) : GUILD_RANK_NONE;

    mysql_free_result(res);
    return rank;
}

int guild_is_member(uint32_t guild_id, uint32_t charac_no) {
    return guild_get_member_rank(guild_id, charac_no) != GUILD_RANK_NONE;
}

int guild_get_member_count(uint32_t guild_id) {
    if (!g_db) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_member WHERE guild_id=%u", guild_id);

    if (mysql_query(g_db, query)) {
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? atoi(row[0]) : 0;

    mysql_free_result(res);
    return count;
}

int guild_add_contribution(uint32_t guild_id, uint32_t charac_no, uint32_t amount) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET contribution=contribution+%u "
             "WHERE guild_id=%u AND charac_no=%u", amount, guild_id, charac_no);

    mysql_query(g_db, query);
    printf("[GUILD] Added %u contribution to member %u in guild %u\n",
           amount, charac_no, guild_id);
    return 0;
}

/*==============================================================================
 * Guild Skills
 *============================================================================*/

int guild_learn_skill(uint32_t guild_id, uint16_t skill_id) {
    if (!g_db) return -1;

    /* Check if skill exists */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT skill_lev FROM guild_skill WHERE guild_id=%u AND skill_no=%u LIMIT 1",
             guild_id, skill_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        /* Upgrade existing skill */
        int current_level = atoi(row[0]);
        mysql_free_result(res);

        if (current_level >= 10) {
            printf("[GUILD] Skill %u already max level\n", skill_id);
            return -2;
        }

        snprintf(query, sizeof(query),
                 "UPDATE guild_skill SET skill_lev=%d WHERE guild_id=%u AND skill_no=%u",
                 current_level + 1, guild_id, skill_id);
    } else {
        mysql_free_result(res);
        /* Learn new skill */
        snprintf(query, sizeof(query),
                 "INSERT INTO guild_skill (guild_id, skill_no, skill_lev) VALUES (%u, %u, 1)",
                 guild_id, skill_id);
    }

    mysql_query(g_db, query);
    printf("[GUILD] Guild %u learned/upgraded skill %u\n", guild_id, skill_id);
    return 0;
}

int guild_get_skills(uint32_t guild_id, GuildSkill* skills, int max_count) {
    if (!g_db || !skills) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT skill_no, skill_lev FROM guild_skill WHERE guild_id=%u LIMIT %d",
             guild_id, max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        skills[count].guild_id = guild_id;
        skills[count].skill_id = row[0] ? (uint16_t)atoi(row[0]) : 0;
        skills[count].skill_level = row[1] ? (uint8_t)atoi(row[1]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

int guild_get_skill_level(uint32_t guild_id, uint16_t skill_id) {
    if (!g_db) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT skill_lev FROM guild_skill WHERE guild_id=%u AND skill_no=%u LIMIT 1",
             guild_id, skill_id);

    if (mysql_query(g_db, query)) {
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    int level = row ? atoi(row[0]) : 0;

    mysql_free_result(res);
    return level;
}

/*==============================================================================
 * Guild Warehouse
 *============================================================================*/

int guild_deposit_item(uint32_t guild_id, uint32_t charac_no,
                       uint32_t item_id, int count, int32_t* out_slot) {
    if (!g_db || !out_slot) return -1;

    /* Find empty slot */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COALESCE(MAX(slot), -1) + 1 FROM guild_warehouse WHERE guild_id=%u", guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    int32_t slot = row ? atoi(row[0]) : 0;
    mysql_free_result(res);

    if (slot >= GUILD_MAX_WAREHOUSE) {
        printf("[GUILD] Warehouse full\n");
        return -2;
    }

    snprintf(query, sizeof(query),
             "INSERT INTO guild_warehouse (guild_id, slot, item_id, count, depositor_id, deposit_time) "
             "VALUES (%u, %d, %u, %d, %u, NOW())",
             guild_id, slot, item_id, count, charac_no);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    *out_slot = slot;
    printf("[GUILD] Deposited item %u x%d to guild %u slot %d\n", item_id, count, guild_id, slot);
    return 0;
}

int guild_withdraw_item(uint32_t guild_id, uint32_t charac_no, int32_t slot, int count) {
    if (!g_db) return -1;
    (void)charac_no;
    (void)count;

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM guild_warehouse WHERE guild_id=%u AND slot=%d", guild_id, slot);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    printf("[GUILD] Withdrew from guild %u slot %d\n", guild_id, slot);
    return 0;
}

int guild_get_warehouse(uint32_t guild_id, GuildWarehouseItem* items, int max_count) {
    if (!g_db || !items) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT slot, item_id, count, depositor_id FROM guild_warehouse "
             "WHERE guild_id=%u LIMIT %d", guild_id, max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        items[count].guild_id = guild_id;
        items[count].slot = row[0] ? atoi(row[0]) : 0;
        items[count].item_id = row[1] ? (uint32_t)atoi(row[1]) : 0;
        items[count].count = row[2] ? atoi(row[2]) : 0;
        items[count].depositor_id = row[3] ? (uint32_t)atoi(row[3]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

int guild_deposit_fund(uint32_t guild_id, uint32_t charac_no, uint32_t amount) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET fund=fund+%u WHERE guild_id=%u", amount, guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    /* Add contribution for donation */
    guild_add_contribution(guild_id, charac_no, amount / 100);

    printf("[GUILD] Deposited %u gold to guild %u fund\n", amount, guild_id);
    return 0;
}

int guild_withdraw_fund(uint32_t guild_id, uint32_t charac_no, uint32_t amount) {
    if (!g_db) return -1;
    (void)charac_no;

    /* Check available funds */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT fund FROM guild_info WHERE guild_id=%u LIMIT 1", guild_id);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    uint32_t current_fund = row ? (uint32_t)atoi(row[0]) : 0;
    mysql_free_result(res);

    if (current_fund < amount) {
        printf("[GUILD] Insufficient funds\n");
        return -2;
    }

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET fund=fund-%u WHERE guild_id=%u", amount, guild_id);

    mysql_query(g_db, query);
    printf("[GUILD] Withdrew %u gold from guild %u fund\n", amount, guild_id);
    return 0;
}

/*==============================================================================
 * Guild Notice & Introduction
 *============================================================================*/

int guild_set_notice(uint32_t guild_id, const char* notice) {
    if (!g_db || !notice) return -1;

    char escaped[GUILD_MAX_NOTICE_LEN * 2 + 1];
    mysql_real_escape_string(g_db, escaped, notice, strlen(notice) > GUILD_MAX_NOTICE_LEN ?
                             GUILD_MAX_NOTICE_LEN : strlen(notice));

    char query[1024];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET notice='%s' WHERE guild_id=%u", escaped, guild_id);

    mysql_query(g_db, query);
    printf("[GUILD] Updated notice for guild %u\n", guild_id);
    return 0;
}

int guild_set_introduction(uint32_t guild_id, const char* intro) {
    if (!g_db || !intro) return -1;

    char escaped[GUILD_MAX_INTRO_LEN * 2 + 1];
    mysql_real_escape_string(g_db, escaped, intro, strlen(intro) > GUILD_MAX_INTRO_LEN ?
                             GUILD_MAX_INTRO_LEN : strlen(intro));

    char query[768];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET intro='%s' WHERE guild_id=%u", escaped, guild_id);

    mysql_query(g_db, query);
    printf("[GUILD] Updated introduction for guild %u\n", guild_id);
    return 0;
}

/*==============================================================================
 * Guild Join Requests
 *============================================================================*/

int guild_request_join(uint32_t guild_id, uint32_t charac_no,
                       const char* charac_name, uint8_t level, uint8_t job) {
    if (!g_db || !charac_name) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO guild_join_request (guild_id, charac_no, charac_name, level, job, status, request_time) "
             "VALUES (%u, %u, '%s', %d, %d, 0, NOW())",
             guild_id, charac_no, charac_name, level, job);

    mysql_query(g_db, query);
    printf("[GUILD] Join request from %s to guild %u\n", charac_name, guild_id);
    return 0;
}

int guild_accept_join(uint32_t guild_id, uint32_t charac_no) {
    if (!g_db) return -1;

    /* Get request info */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_name FROM guild_join_request WHERE guild_id=%u AND charac_no=%u AND status=0 LIMIT 1",
             guild_id, charac_no);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -2;
    }

    char charac_name[22];
    strncpy(charac_name, row[0] ? row[0] : "", 21);
    charac_name[21] = '\0';
    mysql_free_result(res);

    /* Add member */
    int result = guild_add_member(guild_id, charac_no, charac_name, GUILD_RANK_MEMBER);
    if (result != 0) return result;

    /* Update request status */
    snprintf(query, sizeof(query),
             "UPDATE guild_join_request SET status=1 WHERE guild_id=%u AND charac_no=%u",
             guild_id, charac_no);
    mysql_query(g_db, query);

    printf("[GUILD] Accepted %s to guild %u\n", charac_name, guild_id);
    return 0;
}

int guild_reject_join(uint32_t guild_id, uint32_t charac_no) {
    if (!g_db) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_join_request SET status=2 WHERE guild_id=%u AND charac_no=%u",
             guild_id, charac_no);

    mysql_query(g_db, query);
    printf("[GUILD] Rejected join request from %u to guild %u\n", charac_no, guild_id);
    return 0;
}

int guild_get_join_requests(uint32_t guild_id, GuildJoinRequest* requests, int max_count) {
    if (!g_db || !requests) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, level, job FROM guild_join_request "
             "WHERE guild_id=%u AND status=0 LIMIT %d", guild_id, max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        memset(&requests[count], 0, sizeof(GuildJoinRequest));
        requests[count].guild_id = guild_id;
        requests[count].charac_no = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (row[1]) strncpy(requests[count].charac_name, row[1], 20);
        requests[count].level = row[2] ? (uint8_t)atoi(row[2]) : 1;
        requests[count].job = row[3] ? (uint8_t)atoi(row[3]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

/*==============================================================================
 * Guild Search
 *============================================================================*/

int guild_search(const char* keyword, GuildInfo* results, int max_count) {
    if (!g_db || !results) return -1;

    char escaped[128];
    mysql_real_escape_string(g_db, escaped, keyword, strlen(keyword) > 64 ? 64 : strlen(keyword));

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, master_name, lev, member_count "
             "FROM guild_info WHERE expire_flag=0 AND guild_name LIKE '%%%s%%' LIMIT %d",
             escaped, max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        memset(&results[count], 0, sizeof(GuildInfo));
        results[count].guild_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (row[1]) strncpy(results[count].guild_name, row[1], GUILD_MAX_NAME_LEN);
        if (row[2]) strncpy(results[count].master_name, row[2], 20);
        results[count].level = row[3] ? atoi(row[3]) : 1;
        results[count].member_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

int guild_get_recommend_list(GuildInfo* results, int max_count) {
    if (!g_db || !results) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, master_name, lev, member_count "
             "FROM guild_info WHERE expire_flag=0 ORDER BY member_count DESC LIMIT %d",
             max_count);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        memset(&results[count], 0, sizeof(GuildInfo));
        results[count].guild_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
        if (row[1]) strncpy(results[count].guild_name, row[1], GUILD_MAX_NAME_LEN);
        if (row[2]) strncpy(results[count].master_name, row[2], 20);
        results[count].level = row[3] ? atoi(row[3]) : 1;
        results[count].member_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

int guild_get_by_name(const char* guild_name, GuildInfo* info) {
    if (!g_db || !guild_name || !info) return -1;

    char escaped[GUILD_MAX_NAME_LEN * 2 + 1];
    mysql_real_escape_string(g_db, escaped, guild_name, strlen(guild_name));

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id FROM guild_info WHERE guild_name='%s' AND expire_flag=0 LIMIT 1",
             escaped);

    if (mysql_query(g_db, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    uint32_t guild_id = (uint32_t)atoi(row[0]);
    mysql_free_result(res);

    return guild_get_info(guild_id, info);
}

int guild_update_info(const GuildInfo* info) {
    if (!g_db || !info) return -1;

    char escaped_notice[GUILD_MAX_NOTICE_LEN * 2 + 1];
    char escaped_intro[GUILD_MAX_INTRO_LEN * 2 + 1];
    mysql_real_escape_string(g_db, escaped_notice, info->notice, strlen(info->notice));
    mysql_real_escape_string(g_db, escaped_intro, info->introduction, strlen(info->introduction));

    char query[2048];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET lev=%d, guild_exp=%u, guild_point=%u, "
             "notice='%s', intro='%s' WHERE guild_id=%u",
             info->level, info->exp, info->guild_point,
             escaped_notice, escaped_intro, info->guild_id);

    mysql_query(g_db, query);
    return 0;
}

/*==============================================================================
 * Main Event Loop
 *============================================================================*/

int guild_service_run(void) {
    struct epoll_event events[MAX_EVENTS];

    printf("[GUILD] Guild service running...\n");

    while (g_running) {
        int nfds = epoll_wait(g_epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            printf("[GUILD] epoll_wait error: %s\n", strerror(errno));
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == g_listen_fd) {
                accept_connection();
            } else {
                handle_data(events[i].data.fd);
            }
        }
    }

    return 0;
}

/*==============================================================================
 * Main Entry Point
 *============================================================================*/

static void print_usage(const char* prog) {
    printf("Usage: %s [port]\n", prog);
    printf("  port: Server port (default: %d)\n", DEFAULT_PORT);
}

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("     DNF Guild Service v1.0\n");
    printf("========================================\n");

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        g_port = (uint16_t)atoi(argv[1]);
    }

    setup_signals();

    if (guild_service_init(NULL) != 0) {
        printf("[GUILD] Failed to initialize service\n");
        return 1;
    }

    if (init_server() != 0) {
        printf("[GUILD] Failed to initialize server\n");
        guild_service_cleanup();
        return 1;
    }

    int result = guild_service_run();

    guild_service_cleanup();
    printf("[GUILD] Service stopped\n");

    return result;
}
