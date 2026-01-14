/**
 * @file session.h
 * @brief Client session structure
 *
 * Represents a connected client with all associated state.
 */

#ifndef ENGINE_SESSION_SESSION_H
#define ENGINE_SESSION_SESSION_H

#include "../core/types.h"
#include "../core/network/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SESSION_NAME_MAX     32
#define SESSION_BUFFER_SIZE  65536
#define SESSION_MAX_CHARACTERS 16

/* Session state */
typedef enum {
    SESSION_STATE_NONE = 0,
    SESSION_STATE_CONNECTED,
    SESSION_STATE_AUTHENTICATING,
    SESSION_STATE_AUTHENTICATED,
    SESSION_STATE_CHAR_SELECT,
    SESSION_STATE_LOADING,
    SESSION_STATE_IN_GAME,
    SESSION_STATE_IN_DUNGEON,
    SESSION_STATE_IN_PVP,
    SESSION_STATE_DISCONNECTING
} SessionState;

/* Character info (for selection) */
typedef struct {
    u32     charac_no;
    char    name[21];
    u8      job;
    u8      grow_type;
    u8      level;
    u32     exp;
    u32     hp;
    u32     max_hp;
    u8      delete_flag;
} CharacterInfo;

/* Session structure */
typedef struct Session {
    /* Identity */
    u32         id;             /* Unique session ID */
    int         fd;             /* Socket file descriptor */
    u32         remote_ip;
    u16         remote_port;

    /* State */
    SessionState state;
    u32         connect_time;
    u32         last_activity;
    u32         last_heartbeat;

    /* Authentication */
    u32         account_id;
    char        account_name[SESSION_NAME_MAX];
    u8          vip_level;
    u8          gm_level;

    /* Character */
    u32         character_id;
    char        character_name[21];
    u8          job;
    u8          level;
    u32         guild_id;
    u32         party_id;

    /* Character list */
    CharacterInfo characters[SESSION_MAX_CHARACTERS];
    u8          character_count;
    u8          selected_slot;

    /* Location */
    u32         current_map;
    u32         current_channel;
    i32         pos_x;
    i32         pos_y;

    /* Dungeon */
    u32         dungeon_id;
    u8          dungeon_difficulty;
    u8          dungeon_room;

    /* Buffers */
    RingBuffer  recv_buffer;
    RingBuffer  send_buffer;

    /* Statistics */
    u64         bytes_recv;
    u64         bytes_sent;
    u64         packets_recv;
    u64         packets_sent;

    /* User data */
    void*       user_data;

    /* Flags */
    u8          active;
    u8          marked_for_close;
    u8          authenticated;
} Session;

/**
 * Initialize session
 * @param session Session instance
 * @return ENGINE_OK on success
 */
int session_init(Session* session);

/**
 * Cleanup session
 * @param session Session instance
 */
void session_cleanup(Session* session);

/**
 * Reset session to initial state
 * @param session Session instance
 */
void session_reset(Session* session);

/**
 * Set session state
 * @param session Session instance
 * @param state   New state
 */
void session_set_state(Session* session, SessionState state);

/**
 * Get session state
 * @param session Session instance
 * @return Current state
 */
SessionState session_get_state(const Session* session);

/**
 * Check if session is active
 * @param session Session instance
 * @return 1 if active, 0 otherwise
 */
int session_is_active(const Session* session);

/**
 * Check if session is authenticated
 * @param session Session instance
 * @return 1 if authenticated, 0 otherwise
 */
int session_is_authenticated(const Session* session);

/**
 * Check if session is in game
 * @param session Session instance
 * @return 1 if in game, 0 otherwise
 */
int session_is_in_game(const Session* session);

/**
 * Update last activity time
 * @param session Session instance
 */
void session_touch(Session* session);

/**
 * Get state name string
 * @param state Session state
 * @return State name
 */
const char* session_state_name(SessionState state);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_SESSION_SESSION_H */
