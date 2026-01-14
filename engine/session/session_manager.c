/**
 * @file session_manager.c
 * @brief Session manager implementation
 */

#include "session_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FD_MAP_SIZE 65536

static u32 get_time(void)
{
    return (u32)time(NULL);
}

int session_mgr_init(SessionManager* mgr, u32 capacity)
{
    u32 i;
    int result;

    if (!mgr || capacity == 0) {
        return ENGINE_ERROR_PARAM;
    }

    memset(mgr, 0, sizeof(SessionManager));

    mgr->capacity = capacity;
    mgr->next_id = 1;
    mgr->fd_map_size = FD_MAP_SIZE;

    /* Allocate sessions */
    mgr->sessions = (Session*)calloc(capacity, sizeof(Session));
    if (!mgr->sessions) {
        return ENGINE_ERROR_MEMORY;
    }

    /* Initialize each session */
    for (i = 0; i < capacity; i++) {
        result = session_init(&mgr->sessions[i]);
        if (result != ENGINE_OK) {
            /* Cleanup already initialized */
            while (i > 0) {
                i--;
                session_cleanup(&mgr->sessions[i]);
            }
            free(mgr->sessions);
            return result;
        }
    }

    /* Allocate fd map */
    mgr->fd_map = (int*)malloc(FD_MAP_SIZE * sizeof(int));
    if (!mgr->fd_map) {
        for (i = 0; i < capacity; i++) {
            session_cleanup(&mgr->sessions[i]);
        }
        free(mgr->sessions);
        return ENGINE_ERROR_MEMORY;
    }

    /* Initialize fd map to -1 (no session) */
    for (i = 0; i < FD_MAP_SIZE; i++) {
        mgr->fd_map[i] = -1;
    }

    return ENGINE_OK;
}

void session_mgr_cleanup(SessionManager* mgr)
{
    u32 i;

    if (!mgr) {
        return;
    }

    if (mgr->sessions) {
        for (i = 0; i < mgr->capacity; i++) {
            session_cleanup(&mgr->sessions[i]);
        }
        free(mgr->sessions);
    }

    if (mgr->fd_map) {
        free(mgr->fd_map);
    }

    memset(mgr, 0, sizeof(SessionManager));
}

Session* session_mgr_create(SessionManager* mgr, int fd, u32 ip, u16 port)
{
    u32 i;
    Session* session;

    if (!mgr || fd < 0) {
        return NULL;
    }

    /* Find free slot */
    for (i = 0; i < mgr->capacity; i++) {
        if (!mgr->sessions[i].active) {
            session = &mgr->sessions[i];
            break;
        }
    }

    if (i >= mgr->capacity) {
        fprintf(stderr, "[SESSION_MGR] No free session slots\n");
        return NULL;
    }

    /* Reset session */
    session_reset(session);

    /* Setup session */
    session->id = mgr->next_id++;
    session->fd = fd;
    session->remote_ip = ip;
    session->remote_port = port;
    session->state = SESSION_STATE_CONNECTED;
    session->connect_time = get_time();
    session->last_activity = session->connect_time;
    session->active = 1;

    /* Update fd map */
    if (fd < mgr->fd_map_size) {
        mgr->fd_map[fd] = (int)i;
    }

    mgr->count++;
    mgr->total_created++;

    return session;
}

void session_mgr_destroy(SessionManager* mgr, Session* session)
{
    int fd;

    if (!mgr || !session) {
        return;
    }

    fd = session->fd;

    /* Clear fd map */
    if (fd >= 0 && fd < mgr->fd_map_size) {
        mgr->fd_map[fd] = -1;
    }

    /* Reset session */
    session_reset(session);

    if (mgr->count > 0) {
        mgr->count--;
    }

    mgr->total_destroyed++;
}

Session* session_mgr_find_by_fd(SessionManager* mgr, int fd)
{
    int idx;

    if (!mgr || fd < 0 || fd >= mgr->fd_map_size) {
        return NULL;
    }

    idx = mgr->fd_map[fd];

    if (idx < 0 || idx >= (int)mgr->capacity) {
        return NULL;
    }

    if (!mgr->sessions[idx].active) {
        return NULL;
    }

    return &mgr->sessions[idx];
}

Session* session_mgr_find_by_id(SessionManager* mgr, u32 id)
{
    u32 i;

    if (!mgr || id == 0) {
        return NULL;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active && mgr->sessions[i].id == id) {
            return &mgr->sessions[i];
        }
    }

    return NULL;
}

Session* session_mgr_find_by_account(SessionManager* mgr, u32 account_id)
{
    u32 i;

    if (!mgr || account_id == 0) {
        return NULL;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active &&
            mgr->sessions[i].account_id == account_id) {
            return &mgr->sessions[i];
        }
    }

    return NULL;
}

Session* session_mgr_find_by_character(SessionManager* mgr, u32 character_id)
{
    u32 i;

    if (!mgr || character_id == 0) {
        return NULL;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active &&
            mgr->sessions[i].character_id == character_id) {
            return &mgr->sessions[i];
        }
    }

    return NULL;
}

Session* session_mgr_find_by_name(SessionManager* mgr, const char* name)
{
    u32 i;

    if (!mgr || !name || name[0] == '\0') {
        return NULL;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active &&
            strcmp(mgr->sessions[i].character_name, name) == 0) {
            return &mgr->sessions[i];
        }
    }

    return NULL;
}

void session_mgr_foreach(SessionManager* mgr, SessionIterator iterator,
                         void* user_data)
{
    u32 i;

    if (!mgr || !iterator) {
        return;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active) {
            iterator(&mgr->sessions[i], user_data);
        }
    }
}

u32 session_mgr_count(SessionManager* mgr, SessionFilter filter, void* user_data)
{
    u32 i;
    u32 count = 0;

    if (!mgr) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        if (mgr->sessions[i].active) {
            if (!filter || filter(&mgr->sessions[i], user_data)) {
                count++;
            }
        }
    }

    return count;
}

int session_mgr_cleanup_timeout(SessionManager* mgr, u32 current_time, u32 timeout)
{
    u32 i;
    int cleaned = 0;
    Session* session;

    if (!mgr) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        session = &mgr->sessions[i];

        if (!session->active) {
            continue;
        }

        if (current_time - session->last_activity > timeout) {
            printf("[SESSION_MGR] Timeout: session=%u account=%u\n",
                   session->id, session->account_id);

            session_mgr_destroy(mgr, session);
            mgr->total_timeouts++;
            cleaned++;
        }
    }

    return cleaned;
}

/* Helper to send data to session */
static int send_to_session(Session* session, const u8* data, int len)
{
    u32 written;

    if (!session || !data || len <= 0) {
        return 0;
    }

    written = ring_buffer_write(&session->send_buffer, data, len);

    return (int)written == len ? 1 : 0;
}

int session_mgr_broadcast(SessionManager* mgr, const u8* data, int len)
{
    u32 i;
    int count = 0;
    Session* session;

    if (!mgr || !data || len <= 0) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        session = &mgr->sessions[i];

        if (session->active && session_is_in_game(session)) {
            if (send_to_session(session, data, len)) {
                count++;
            }
        }
    }

    return count;
}

int session_mgr_broadcast_guild(SessionManager* mgr, u32 guild_id,
                                const u8* data, int len, const Session* exclude)
{
    u32 i;
    int count = 0;
    Session* session;

    if (!mgr || guild_id == 0 || !data || len <= 0) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        session = &mgr->sessions[i];

        if (session->active &&
            session_is_in_game(session) &&
            session->guild_id == guild_id &&
            session != exclude) {
            if (send_to_session(session, data, len)) {
                count++;
            }
        }
    }

    return count;
}

int session_mgr_broadcast_party(SessionManager* mgr, u32 party_id,
                                const u8* data, int len, const Session* exclude)
{
    u32 i;
    int count = 0;
    Session* session;

    if (!mgr || party_id == 0 || !data || len <= 0) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        session = &mgr->sessions[i];

        if (session->active &&
            session_is_in_game(session) &&
            session->party_id == party_id &&
            session != exclude) {
            if (send_to_session(session, data, len)) {
                count++;
            }
        }
    }

    return count;
}

int session_mgr_broadcast_map(SessionManager* mgr, u32 map_id,
                              const u8* data, int len, const Session* exclude)
{
    u32 i;
    int count = 0;
    Session* session;

    if (!mgr || map_id == 0 || !data || len <= 0) {
        return 0;
    }

    for (i = 0; i < mgr->capacity; i++) {
        session = &mgr->sessions[i];

        if (session->active &&
            session_is_in_game(session) &&
            session->current_map == map_id &&
            session != exclude) {
            if (send_to_session(session, data, len)) {
                count++;
            }
        }
    }

    return count;
}

u32 session_mgr_active_count(const SessionManager* mgr)
{
    return mgr ? mgr->count : 0;
}

void session_mgr_get_stats(const SessionManager* mgr,
                           u64* total_created, u64* total_destroyed,
                           u64* total_timeouts)
{
    if (!mgr) {
        if (total_created) *total_created = 0;
        if (total_destroyed) *total_destroyed = 0;
        if (total_timeouts) *total_timeouts = 0;
        return;
    }

    if (total_created) *total_created = mgr->total_created;
    if (total_destroyed) *total_destroyed = mgr->total_destroyed;
    if (total_timeouts) *total_timeouts = mgr->total_timeouts;
}
