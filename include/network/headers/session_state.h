#ifndef __SESSION_STATE_H__
#define __SESSION_STATE_H__

#include <stdint.h>
#include <time.h>

/*
 * Session State Machine - DNF Game Server
 *
 * State transition flow:
 *   DISCONNECTED -> CONNECTED -> AUTHENTICATED -> IN_GAME -> DISCONNECTED
 *
 * Timeouts:
 *   CONNECTED: 30 seconds (must authenticate)
 *   AUTHENTICATED: 10 minutes (idle timeout)
 *   IN_GAME: 5 minutes (heartbeat timeout)
 */

/* Session states */
typedef enum SessionState {
    SESSION_STATE_DISCONNECTED = 0,  /* Initial state / Closed */
    SESSION_STATE_CONNECTED    = 1,  /* TCP connected, waiting for auth */
    SESSION_STATE_AUTHENTICATED= 2,  /* Authenticated, not in game yet */
    SESSION_STATE_IN_GAME      = 3,  /* Active in game */
    SESSION_STATE_DISCONNECTING= 4   /* Graceful shutdown in progress */
} SessionState;

/* Timeout values (seconds) */
#define SESSION_TIMEOUT_HANDSHAKE      30    /* Must auth within 30s */
#define SESSION_TIMEOUT_IDLE          600    /* 10 min idle in authenticated state */
#define SESSION_TIMEOUT_HEARTBEAT     300    /* 5 min heartbeat timeout in game */

/* Session statistics */
typedef struct SessionStats {
    unsigned long packets_received;    /* Total packets received */
    unsigned long packets_sent;        /* Total packets sent */
    unsigned long bytes_received;      /* Total bytes received */
    unsigned long bytes_sent;          /* Total bytes sent */

    time_t connect_time;               /* When session connected */
    time_t last_activity;              /* Last packet received time */
    time_t last_heartbeat;             /* Last heartbeat time */

    unsigned int ping_ms;              /* Round-trip time in milliseconds */
    unsigned int packet_loss;          /* Packet loss count */
} SessionStats;

/* Session authentication info */
typedef struct SessionAuth {
    int authenticated;                 /* 0=not auth, 1=authenticated */
    int account_id;                    /* Account ID (0=not set) */
    int character_id;                  /* Character ID (0=not set) */
    char username[64];                 /* Username */
    char ip_address[64];               /* Client IP address */
    int privilege_level;               /* 0=normal, 1=GM, 2=admin */
} SessionAuth;

/* Session state info (embedded in SessionData) */
typedef struct SessionStateInfo {
    SessionState state;                /* Current state */
    SessionStats stats;                /* Statistics */
    SessionAuth auth;                  /* Authentication info */

    time_t state_enter_time;           /* When entered current state */
    int timeout_seconds;               /* Timeout for current state */
} SessionStateInfo;

/* Session state transition functions */

/*
 * Session_SetState - Change session state
 * Returns: 0=success, -1=invalid transition
 */
int Session_SetState(int fd, SessionState new_state);

/*
 * Session_GetState - Get current session state
 * Returns: SessionState value, or -1 if session not found
 */
SessionState Session_GetState(int fd);

/*
 * Session_IsTimedOut - Check if session has timed out
 * Returns: 1=timed out, 0=ok
 */
int Session_IsTimedOut(int fd);

/*
 * Session_UpdateActivity - Update last activity time
 */
void Session_UpdateActivity(int fd);

/*
 * Session_UpdateHeartbeat - Update last heartbeat time
 */
void Session_UpdateHeartbeat(int fd);

/*
 * Session_GetStats - Get session statistics
 * Returns: Pointer to SessionStats, or NULL if not found
 */
const SessionStats* Session_GetStats(int fd);

/*
 * Session_GetAuth - Get session authentication info
 * Returns: Pointer to SessionAuth, or NULL if not found
 */
const SessionAuth* Session_GetAuth(int fd);

/*
 * Session_SetAuth - Set authentication info
 * Returns: 0=success, -1=error
 */
int Session_SetAuth(int fd, int account_id, const char *username);

/*
 * Session_CheckTimeouts - Check all sessions for timeouts
 * Returns: Number of sessions timed out
 *
 * This should be called periodically (e.g., every 10 seconds)
 * from a timer or event loop.
 */
int Session_CheckTimeouts(void);

/*
 * Session_PrintState - Print session state (debug)
 */
void Session_PrintState(int fd);

/*
 * Session_GetStateName - Get state name string
 */
const char* Session_GetStateName(SessionState state);

#endif /* __SESSION_STATE_H__ */
