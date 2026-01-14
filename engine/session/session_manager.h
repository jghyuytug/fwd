/**
 * @file session_manager.h
 * @brief Session manager for client connections
 *
 * Manages all client sessions with O(1) lookup.
 */

#ifndef ENGINE_SESSION_SESSION_MANAGER_H
#define ENGINE_SESSION_SESSION_MANAGER_H

#include "../core/types.h"
#include "session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SESSION_MGR_DEFAULT_CAPACITY 1024
#define SESSION_TIMEOUT_SECONDS      300

/* Session iterator callback */
typedef void (*SessionIterator)(Session* session, void* user_data);

/* Session filter callback */
typedef int (*SessionFilter)(const Session* session, void* user_data);

/* Session manager */
typedef struct {
    Session*    sessions;
    u32         capacity;
    u32         count;
    u32         next_id;
    int*        fd_map;         /* fd -> session index */
    int         fd_map_size;
    /* Statistics */
    u64         total_created;
    u64         total_destroyed;
    u64         total_timeouts;
} SessionManager;

/**
 * Initialize session manager
 * @param mgr      Session manager instance
 * @param capacity Maximum sessions
 * @return ENGINE_OK on success
 */
int session_mgr_init(SessionManager* mgr, u32 capacity);

/**
 * Cleanup session manager
 * @param mgr Session manager instance
 */
void session_mgr_cleanup(SessionManager* mgr);

/**
 * Create a new session
 * @param mgr   Session manager instance
 * @param fd    Socket file descriptor
 * @param ip    Remote IP address
 * @param port  Remote port
 * @return Session pointer or NULL on failure
 */
Session* session_mgr_create(SessionManager* mgr, int fd, u32 ip, u16 port);

/**
 * Destroy a session
 * @param mgr     Session manager instance
 * @param session Session to destroy
 */
void session_mgr_destroy(SessionManager* mgr, Session* session);

/**
 * Find session by file descriptor
 * @param mgr Session manager instance
 * @param fd  File descriptor
 * @return Session pointer or NULL
 */
Session* session_mgr_find_by_fd(SessionManager* mgr, int fd);

/**
 * Find session by session ID
 * @param mgr Session manager instance
 * @param id  Session ID
 * @return Session pointer or NULL
 */
Session* session_mgr_find_by_id(SessionManager* mgr, u32 id);

/**
 * Find session by account ID
 * @param mgr        Session manager instance
 * @param account_id Account ID
 * @return Session pointer or NULL
 */
Session* session_mgr_find_by_account(SessionManager* mgr, u32 account_id);

/**
 * Find session by character ID
 * @param mgr          Session manager instance
 * @param character_id Character ID
 * @return Session pointer or NULL
 */
Session* session_mgr_find_by_character(SessionManager* mgr, u32 character_id);

/**
 * Find session by character name
 * @param mgr  Session manager instance
 * @param name Character name
 * @return Session pointer or NULL
 */
Session* session_mgr_find_by_name(SessionManager* mgr, const char* name);

/**
 * Iterate all active sessions
 * @param mgr      Session manager instance
 * @param iterator Callback function
 * @param user_data User context
 */
void session_mgr_foreach(SessionManager* mgr, SessionIterator iterator,
                         void* user_data);

/**
 * Count sessions matching filter
 * @param mgr       Session manager instance
 * @param filter    Filter function (NULL = all)
 * @param user_data User context
 * @return Number of matching sessions
 */
u32 session_mgr_count(SessionManager* mgr, SessionFilter filter, void* user_data);

/**
 * Cleanup timed out sessions
 * @param mgr          Session manager instance
 * @param current_time Current time in seconds
 * @param timeout      Timeout in seconds
 * @return Number of sessions cleaned up
 */
int session_mgr_cleanup_timeout(SessionManager* mgr, u32 current_time, u32 timeout);

/**
 * Broadcast data to all in-game sessions
 * @param mgr  Session manager instance
 * @param data Data to send
 * @param len  Data length
 * @return Number of sessions sent to
 */
int session_mgr_broadcast(SessionManager* mgr, const u8* data, int len);

/**
 * Broadcast to sessions in same guild
 * @param mgr      Session manager instance
 * @param guild_id Guild ID
 * @param data     Data to send
 * @param len      Data length
 * @param exclude  Session to exclude (NULL = none)
 * @return Number of sessions sent to
 */
int session_mgr_broadcast_guild(SessionManager* mgr, u32 guild_id,
                                const u8* data, int len, const Session* exclude);

/**
 * Broadcast to sessions in same party
 * @param mgr      Session manager instance
 * @param party_id Party ID
 * @param data     Data to send
 * @param len      Data length
 * @param exclude  Session to exclude (NULL = none)
 * @return Number of sessions sent to
 */
int session_mgr_broadcast_party(SessionManager* mgr, u32 party_id,
                                const u8* data, int len, const Session* exclude);

/**
 * Broadcast to sessions in same map
 * @param mgr     Session manager instance
 * @param map_id  Map ID
 * @param data    Data to send
 * @param len     Data length
 * @param exclude Session to exclude (NULL = none)
 * @return Number of sessions sent to
 */
int session_mgr_broadcast_map(SessionManager* mgr, u32 map_id,
                              const u8* data, int len, const Session* exclude);

/**
 * Get active session count
 * @param mgr Session manager instance
 * @return Number of active sessions
 */
u32 session_mgr_active_count(const SessionManager* mgr);

/**
 * Get statistics
 * @param mgr            Session manager instance
 * @param total_created  Output: total created
 * @param total_destroyed Output: total destroyed
 * @param total_timeouts Output: total timeouts
 */
void session_mgr_get_stats(const SessionManager* mgr,
                           u64* total_created, u64* total_destroyed,
                           u64* total_timeouts);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_SESSION_SESSION_MANAGER_H */
