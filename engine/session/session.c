/**
 * @file session.c
 * @brief Client session implementation
 */

#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char* state_names[] = {
    "NONE",
    "CONNECTED",
    "AUTHENTICATING",
    "AUTHENTICATED",
    "CHAR_SELECT",
    "LOADING",
    "IN_GAME",
    "IN_DUNGEON",
    "IN_PVP",
    "DISCONNECTING"
};

static u32 get_time(void)
{
    return (u32)time(NULL);
}

int session_init(Session* session)
{
    if (!session) {
        return ENGINE_ERROR_PARAM;
    }

    memset(session, 0, sizeof(Session));

    session->fd = -1;
    session->state = SESSION_STATE_NONE;

    if (ring_buffer_init(&session->recv_buffer, SESSION_BUFFER_SIZE) != ENGINE_OK) {
        return ENGINE_ERROR_MEMORY;
    }

    if (ring_buffer_init(&session->send_buffer, SESSION_BUFFER_SIZE) != ENGINE_OK) {
        ring_buffer_cleanup(&session->recv_buffer);
        return ENGINE_ERROR_MEMORY;
    }

    return ENGINE_OK;
}

void session_cleanup(Session* session)
{
    if (!session) {
        return;
    }

    ring_buffer_cleanup(&session->recv_buffer);
    ring_buffer_cleanup(&session->send_buffer);

    if (session->user_data) {
        /* Note: caller is responsible for freeing user_data */
        session->user_data = NULL;
    }

    memset(session, 0, sizeof(Session));
    session->fd = -1;
}

void session_reset(Session* session)
{
    if (!session) {
        return;
    }

    /* Keep buffers, reset everything else */
    ring_buffer_reset(&session->recv_buffer);
    ring_buffer_reset(&session->send_buffer);

    session->id = 0;
    session->fd = -1;
    session->remote_ip = 0;
    session->remote_port = 0;
    session->state = SESSION_STATE_NONE;
    session->connect_time = 0;
    session->last_activity = 0;
    session->last_heartbeat = 0;

    session->account_id = 0;
    session->account_name[0] = '\0';
    session->vip_level = 0;
    session->gm_level = 0;

    session->character_id = 0;
    session->character_name[0] = '\0';
    session->job = 0;
    session->level = 0;
    session->guild_id = 0;
    session->party_id = 0;

    session->character_count = 0;
    session->selected_slot = 0;

    session->current_map = 0;
    session->current_channel = 0;
    session->pos_x = 0;
    session->pos_y = 0;

    session->dungeon_id = 0;
    session->dungeon_difficulty = 0;
    session->dungeon_room = 0;

    session->bytes_recv = 0;
    session->bytes_sent = 0;
    session->packets_recv = 0;
    session->packets_sent = 0;

    session->user_data = NULL;
    session->active = 0;
    session->marked_for_close = 0;
    session->authenticated = 0;
}

void session_set_state(Session* session, SessionState state)
{
    if (session) {
        session->state = state;
        session->last_activity = get_time();
    }
}

SessionState session_get_state(const Session* session)
{
    return session ? session->state : SESSION_STATE_NONE;
}

int session_is_active(const Session* session)
{
    return session && session->active && session->fd >= 0;
}

int session_is_authenticated(const Session* session)
{
    if (!session) return 0;

    return session->authenticated ||
           session->state >= SESSION_STATE_AUTHENTICATED;
}

int session_is_in_game(const Session* session)
{
    if (!session) return 0;

    return session->state >= SESSION_STATE_IN_GAME &&
           session->state <= SESSION_STATE_IN_PVP;
}

void session_touch(Session* session)
{
    if (session) {
        session->last_activity = get_time();
    }
}

const char* session_state_name(SessionState state)
{
    if (state >= 0 && state < (int)(sizeof(state_names) / sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN";
}
