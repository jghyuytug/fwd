#ifndef GAME_SERVER_CONTEXT_H
#define GAME_SERVER_CONTEXT_H

#include <stdint.h>
#include <mysql/mysql.h>
#include "database/headers/db_connection_manager.h"
#include "game_server_types.h"

/* Share the maximum session count with all modules */
#ifndef MAX_SESSIONS
#define MAX_SESSIONS 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Runtime server status shared across modules */
typedef struct GameServerState {
    int running;          /* 1 while the server loop is active */
    int listen_fd;        /* Listening socket descriptor */
    int epoll_fd;         /* epoll instance descriptor */
    uint16_t sequence;    /* Global packet sequence */
    uint32_t start_time;  /* Server start timestamp */
} GameServerState;

extern DBConnectionManager g_db_manager;
extern GameServerState g_server_state;
extern ClientSession g_sessions[MAX_SESSIONS];
extern int g_session_count;
extern MYSQL* g_db_account;
extern MYSQL* g_db_charac;
extern MYSQL* g_db_charac2;
int is_character_online(uint32_t charac_no);
void guild_ipc_notify_online(uint32_t guild_id, uint32_t charac_no, int online);

#ifdef __cplusplus
}
#endif

#endif /* GAME_SERVER_CONTEXT_H */
