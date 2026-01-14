/*
 * Session Timer - Implementation
 *
 * Functions:
 * - 定时器初始化和清理
 * - 周期性超时检测
 * - 会话清理和断开
 * - 统计信息收集
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "network/headers/session_timer.h"
#include "network/headers/session_state.h"

/* External declarations (defined in network.c) */
typedef struct SessionData SessionData;
extern void Session_GetAllSessions(SessionData **sessions, int *count);
extern SessionStateInfo* Session_GetStateInfo(int fd);
extern pthread_mutex_t* Session_GetPoolLock(void);
extern int Session_Close(int fd);  /* 关闭会话的函数 */
extern SessionData* Session_GetByIndex(int index);  /* 获取数组中的session */
extern int Session_GetSocketFd(SessionData *session);  /* 获取socket fd */
extern int Session_IsConnected(SessionData *session);   /* 检查连接状态 */

/* Global timer info */
static SessionTimerInfo g_timer_info = {0};
static pthread_mutex_t g_timer_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * SessionTimer_Init - 初始化定时器系统
 */
int SessionTimer_Init(void)
{
    pthread_mutex_lock(&g_timer_lock);

    memset(&g_timer_info, 0, sizeof(SessionTimerInfo));
    g_timer_info.last_check_time = time(NULL);
    g_timer_info.check_interval = TIMER_CHECK_INTERVAL_SECONDS;

    pthread_mutex_unlock(&g_timer_lock);

    printf("[SessionTimer] Initialized: check_interval=%d seconds\n",
           g_timer_info.check_interval);

    return 0;
}

/*
 * SessionTimer_Cleanup - 清理定时器系统
 */
void SessionTimer_Cleanup(void)
{
    pthread_mutex_lock(&g_timer_lock);

    printf("[SessionTimer] Cleanup: total_checks=%lu, total_timeouts=%lu, total_cleanups=%lu\n",
           g_timer_info.total_checks,
           g_timer_info.total_timeouts,
           g_timer_info.total_cleanups);

    pthread_mutex_unlock(&g_timer_lock);
}

/*
 * SessionTimer_ShouldCheck - 检查是否应该执行定时任务
 */
int SessionTimer_ShouldCheck(void)
{
    time_t current_time = time(NULL);
    time_t elapsed;

    pthread_mutex_lock(&g_timer_lock);
    elapsed = current_time - g_timer_info.last_check_time;
    pthread_mutex_unlock(&g_timer_lock);

    if (elapsed >= g_timer_info.check_interval) {
        return 1;
    }

    return 0;
}

/*
 * SessionTimer_ProcessTimeouts - 处理所有超时会话
 */
int SessionTimer_ProcessTimeouts(void)
{
    SessionData *sessions = NULL;
    int session_count = 0;
    int timeout_count = 0;
    int closed_count = 0;

    /* 更新检查时间 */
    pthread_mutex_lock(&g_timer_lock);
    g_timer_info.last_check_time = time(NULL);
    g_timer_info.total_checks++;
    pthread_mutex_unlock(&g_timer_lock);

    /* 获取所有会话 */
    Session_GetAllSessions(&sessions, &session_count);

    if (session_count == 0) {
        return 0;
    }

    printf("[SessionTimer] Checking %d sessions for timeouts...\n", session_count);

    /* 遍历所有会话，检查超时 */
    pthread_mutex_t *pool_lock = Session_GetPoolLock();
    pthread_mutex_lock(pool_lock);

    for (int i = 0; i < session_count; i++) {
        SessionData *session = Session_GetByIndex(i);
        if (!session) continue;

        /* 跳过未使用的会话槽位 */
        int fd = Session_GetSocketFd(session);
        if (fd <= 0 || !Session_IsConnected(session)) {
            continue;
        }

        SessionStateInfo *info = Session_GetStateInfo(fd);

        if (!info) {
            continue;
        }

        /* 检查是否超时 */
        if (Session_IsTimedOut(fd)) {
            timeout_count++;

            printf("[SessionTimer] Session fd=%d timed out in state %s, disconnecting...\n",
                   fd, Session_GetStateName(info->state));

            /* 标记需要关闭（在锁外执行实际关闭） */
            /* 这里我们只是记录，实际关闭在循环外进行 */
        }
    }

    pthread_mutex_unlock(pool_lock);

    /* 如果有超时的会话，执行关闭操作 */
    if (timeout_count > 0) {
        /* 重新遍历并关闭超时会话 */
        Session_GetAllSessions(&sessions, &session_count);

        for (int i = 0; i < session_count; i++) {
            SessionData *session = Session_GetByIndex(i);
            if (!session) continue;

            int fd = Session_GetSocketFd(session);
            if (fd <= 0 || !Session_IsConnected(session)) {
                continue;
            }

            if (Session_IsTimedOut(fd)) {
                /* 关闭会话 */
                if (Session_Close(fd) == 0) {
                    closed_count++;
                }
            }
        }

        /* 更新统计 */
        pthread_mutex_lock(&g_timer_lock);
        g_timer_info.total_timeouts += timeout_count;
        g_timer_info.total_cleanups += closed_count;
        pthread_mutex_unlock(&g_timer_lock);

        printf("[SessionTimer] Processed timeouts: detected=%d, closed=%d\n",
               timeout_count, closed_count);
    }

    return closed_count;
}

/*
 * SessionTimer_GetInfo - 获取定时器统计信息
 */
const SessionTimerInfo* SessionTimer_GetInfo(void)
{
    return &g_timer_info;
}

/*
 * SessionTimer_PrintStatistics - 打印定时器统计信息
 */
void SessionTimer_PrintStatistics(void)
{
    pthread_mutex_lock(&g_timer_lock);

    printf("========================================\n");
    printf("Session Timer Statistics\n");
    printf("========================================\n");
    printf("Check Interval:    %d seconds\n", g_timer_info.check_interval);
    printf("Total Checks:      %lu\n", g_timer_info.total_checks);
    printf("Total Timeouts:    %lu\n", g_timer_info.total_timeouts);
    printf("Total Cleanups:    %lu\n", g_timer_info.total_cleanups);
    printf("Last Check:        %s", ctime(&g_timer_info.last_check_time));
    printf("========================================\n");

    pthread_mutex_unlock(&g_timer_lock);
}

/*
 * Session_PrintAllStatistics - 打印所有会话统计信息（Week 6监控接口）
 */
void Session_PrintAllStatistics(void)
{
    SessionData *sessions = NULL;
    int session_count = 0;

    printf("\n");
    printf("========================================\n");
    printf("All Sessions Statistics\n");
    printf("========================================\n");

    Session_GetAllSessions(&sessions, &session_count);

    if (session_count == 0) {
        printf("No active sessions\n");
        printf("========================================\n");
        return;
    }

    printf("Total Active Sessions: %d\n\n", session_count);

    pthread_mutex_t *pool_lock = Session_GetPoolLock();
    pthread_mutex_lock(pool_lock);

    int active_count = 0;
    for (int i = 0; i < session_count; i++) {
        SessionData *session = Session_GetByIndex(i);
        if (!session) continue;

        int fd = Session_GetSocketFd(session);
        if (fd <= 0 || !Session_IsConnected(session)) {
            continue;
        }

        active_count++;
        SessionStateInfo *info = Session_GetStateInfo(fd);

        if (info) {
            time_t current_time = time(NULL);
            time_t elapsed = current_time - info->state_enter_time;
            int is_timeout = Session_IsTimedOut(fd);

            printf("Session #%d (fd=%d):\n", active_count, fd);
            printf("  State:         %s\n", Session_GetStateName(info->state));
            printf("  Elapsed:       %ld seconds\n", (long)elapsed);
            printf("  Timeout:       %d seconds\n", info->timeout_seconds);
            printf("  Status:        %s\n", is_timeout ? "TIMED OUT" : "OK");
            printf("  Authenticated: %s\n", info->auth.authenticated ? "YES" : "NO");
            if (info->auth.authenticated) {
                printf("  Username:      %s\n", info->auth.username);
                printf("  Account ID:    %d\n", info->auth.account_id);
            }
            printf("  Packets RX:    %lu\n", info->stats.packets_received);
            printf("  Packets TX:    %lu\n", info->stats.packets_sent);
            printf("  Bytes RX:      %lu\n", info->stats.bytes_received);
            printf("  Bytes TX:      %lu\n", info->stats.bytes_sent);
            printf("\n");
        }
    }

    pthread_mutex_unlock(pool_lock);

    printf("Active Sessions: %d / %d\n", active_count, session_count);
    printf("========================================\n");
}

/*
 * Session_GetActiveCount - 获取活跃会话数量（Week 6监控接口）
 */
int Session_GetActiveCount(void)
{
    SessionData *sessions = NULL;
    int session_count = 0;
    int active_count = 0;

    Session_GetAllSessions(&sessions, &session_count);

    pthread_mutex_t *pool_lock = Session_GetPoolLock();
    pthread_mutex_lock(pool_lock);

    for (int i = 0; i < session_count; i++) {
        SessionData *session = Session_GetByIndex(i);
        if (!session) continue;

        int fd = Session_GetSocketFd(session);
        if (fd > 0 && Session_IsConnected(session)) {
            active_count++;
        }
    }

    pthread_mutex_unlock(pool_lock);

    return active_count;
}
