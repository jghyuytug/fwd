/*
 * Session State Management - Implementation
 *
 * Functions:
 * - Session state transitions
 * - Timeout detection
 * - Activity tracking
 * - Statistics management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "network/headers/session_state.h"

/* External declarations (defined in network.c) */
extern SessionStateInfo* Session_GetStateInfo(int fd);
extern pthread_mutex_t* Session_GetPoolLock(void);

/* For SessionData iteration (opaque type) */
typedef struct SessionData SessionData;
extern void Session_GetAllSessions(SessionData **sessions, int *count);

/* State names for debugging */
static const char* g_state_names[] = {
    "DISCONNECTED",
    "CONNECTED",
    "AUTHENTICATED",
    "IN_GAME",
    "DISCONNECTING"
};

const char* Session_GetStateName(SessionState state)
{
    if (state < 0 || state > SESSION_STATE_DISCONNECTING) {
        return "UNKNOWN";
    }
    return g_state_names[state];
}

/*
 * Session_SetState - Change session state
 */
int Session_SetState(int fd, SessionState new_state)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        fprintf(stderr, "[SessionState] SetState: Session not found for fd=%d\n", fd);
        return -1;
    }

    SessionState old_state = info->state;

    /* Validate state transition */
    /* Simple validation: can't go backwards except to DISCONNECTED */
    if (new_state < old_state && new_state != SESSION_STATE_DISCONNECTED) {
        fprintf(stderr, "[SessionState] Invalid transition: %s -> %s\n",
                Session_GetStateName(old_state),
                Session_GetStateName(new_state));
        return -1;
    }

    pthread_mutex_t *lock = Session_GetPoolLock();
    pthread_mutex_lock(lock);

    info->state = new_state;
    info->state_enter_time = time(NULL);

    /* Set timeout based on new state */
    switch (new_state) {
        case SESSION_STATE_CONNECTED:
            info->timeout_seconds = SESSION_TIMEOUT_HANDSHAKE;
            break;
        case SESSION_STATE_AUTHENTICATED:
            info->timeout_seconds = SESSION_TIMEOUT_IDLE;
            break;
        case SESSION_STATE_IN_GAME:
            info->timeout_seconds = SESSION_TIMEOUT_HEARTBEAT;
            break;
        default:
            info->timeout_seconds = 0;
            break;
    }

    pthread_mutex_unlock(lock);

    printf("[SessionState] fd=%d: %s -> %s (timeout=%ds)\n",
           fd,
           Session_GetStateName(old_state),
           Session_GetStateName(new_state),
           info->timeout_seconds);

    return 0;
}

/*
 * Session_GetState - Get current session state
 */
SessionState Session_GetState(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return SESSION_STATE_DISCONNECTED;
    }

    return info->state;
}

/*
 * Session_IsTimedOut - Check if session has timed out
 */
int Session_IsTimedOut(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return 1;  /* Not found = timed out */
    }

    /* No timeout for disconnected/disconnecting states */
    if (info->state == SESSION_STATE_DISCONNECTED ||
        info->state == SESSION_STATE_DISCONNECTING) {
        return 0;
    }

    /* No timeout configured */
    if (info->timeout_seconds == 0) {
        return 0;
    }

    time_t current_time = time(NULL);
    time_t elapsed = current_time - info->state_enter_time;

    if (elapsed > info->timeout_seconds) {
        printf("[SessionState] fd=%d TIMEOUT: %ld seconds in state %s (limit=%d)\n",
               fd, (long)elapsed, Session_GetStateName(info->state), info->timeout_seconds);
        return 1;
    }

    return 0;
}

/*
 * Session_UpdateActivity - Update last activity time
 */
void Session_UpdateActivity(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return;
    }

    pthread_mutex_t *lock = Session_GetPoolLock();
    pthread_mutex_lock(lock);

    info->stats.last_activity = time(NULL);

    pthread_mutex_unlock(lock);
}

/*
 * Session_UpdateHeartbeat - Update last heartbeat time
 */
void Session_UpdateHeartbeat(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return;
    }

    pthread_mutex_t *lock = Session_GetPoolLock();
    pthread_mutex_lock(lock);

    info->stats.last_heartbeat = time(NULL);

    /* Reset state_enter_time to extend timeout */
    info->state_enter_time = time(NULL);

    pthread_mutex_unlock(lock);

    printf("[SessionState] Heartbeat updated for fd=%d\n", fd);
}

/*
 * Session_GetStats - Get session statistics
 */
const SessionStats* Session_GetStats(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return NULL;
    }

    return &info->stats;
}

/*
 * Session_GetAuth - Get session authentication info
 */
const SessionAuth* Session_GetAuth(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return NULL;
    }

    return &info->auth;
}

/*
 * Session_SetAuth - Set authentication info
 */
int Session_SetAuth(int fd, int account_id, const char *username)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        return -1;
    }

    pthread_mutex_t *lock = Session_GetPoolLock();
    pthread_mutex_lock(lock);

    info->auth.authenticated = 1;
    info->auth.account_id = account_id;
    if (username) {
        strncpy(info->auth.username, username, sizeof(info->auth.username) - 1);
        info->auth.username[sizeof(info->auth.username) - 1] = '\0';
    }

    pthread_mutex_unlock(lock);

    printf("[SessionState] Auth set for fd=%d: account_id=%d, username=%s\n",
           fd, account_id, username ? username : "(null)");

    /* Transition to AUTHENTICATED state */
    Session_SetState(fd, SESSION_STATE_AUTHENTICATED);

    return 0;
}

/*
 * Session_CheckTimeouts - Check all sessions for timeouts
 *
 * Week 6: 此函数现在由SessionTimer_ProcessTimeouts实现
 * 这里保留为兼容性包装器
 */
int Session_CheckTimeouts(void)
{
    /* Week 6: 由session_timer.c中的SessionTimer_ProcessTimeouts处理 */
    extern int SessionTimer_ProcessTimeouts(void);
    return SessionTimer_ProcessTimeouts();
}

/*
 * Session_PrintState - Print session state (debug)
 */
void Session_PrintState(int fd)
{
    SessionStateInfo *info = Session_GetStateInfo(fd);
    if (!info) {
        printf("[SessionState] fd=%d: NOT FOUND\n", fd);
        return;
    }

    time_t current_time = time(NULL);
    time_t elapsed = current_time - info->state_enter_time;

    printf("========================================\n");
    printf("Session State: fd=%d\n", fd);
    printf("========================================\n");
    printf("State:        %s\n", Session_GetStateName(info->state));
    printf("Elapsed:      %ld seconds\n", (long)elapsed);
    printf("Timeout:      %d seconds\n", info->timeout_seconds);
    printf("Authenticated: %s\n", info->auth.authenticated ? "YES" : "NO");
    if (info->auth.authenticated) {
        printf("Account ID:   %d\n", info->auth.account_id);
        printf("Username:     %s\n", info->auth.username);
        printf("IP Address:   %s\n", info->auth.ip_address);
    }
    printf("Packets RX:   %lu\n", info->stats.packets_received);
    printf("Packets TX:   %lu\n", info->stats.packets_sent);
    printf("Bytes RX:     %lu\n", info->stats.bytes_received);
    printf("Bytes TX:     %lu\n", info->stats.bytes_sent);
    printf("Connect Time: %s", ctime(&info->stats.connect_time));
    printf("Last Activity: %s", ctime(&info->stats.last_activity));
    printf("Last Heartbeat: %s", ctime(&info->stats.last_heartbeat));
    printf("========================================\n");
}
