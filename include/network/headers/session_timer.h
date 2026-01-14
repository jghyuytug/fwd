#ifndef __SESSION_TIMER_H__
#define __SESSION_TIMER_H__

#include <time.h>

/*
 * Session Timer - DNF Game Server
 *
 * 定时器系统，用于周期性会话维护：
 * - 超时检测（每10秒）
 * - 会话清理
 * - 统计报告
 */

/* 定时器配置 */
#define TIMER_CHECK_INTERVAL_SECONDS    10   /* 每10秒检查一次 */

/* 定时器任务类型 */
typedef enum TimerTaskType {
    TIMER_TASK_TIMEOUT_CHECK = 0,  /* 超时检测任务 */
    TIMER_TASK_STATISTICS    = 1,  /* 统计报告任务 */
    TIMER_TASK_CLEANUP       = 2   /* 清理任务 */
} TimerTaskType;

/* 定时器信息 */
typedef struct SessionTimerInfo {
    time_t last_check_time;         /* 上次检查时间 */
    int check_interval;             /* 检查间隔（秒） */

    /* 统计信息 */
    unsigned long total_checks;     /* 总检查次数 */
    unsigned long total_timeouts;   /* 总超时会话数 */
    unsigned long total_cleanups;   /* 总清理会话数 */
} SessionTimerInfo;

/*
 * SessionTimer_Init - 初始化定时器系统
 * Returns: 0=success, -1=error
 */
int SessionTimer_Init(void);

/*
 * SessionTimer_Cleanup - 清理定时器系统
 */
void SessionTimer_Cleanup(void);

/*
 * SessionTimer_ShouldCheck - 检查是否应该执行定时任务
 * Returns: 1=should check, 0=not yet
 */
int SessionTimer_ShouldCheck(void);

/*
 * SessionTimer_ProcessTimeouts - 处理所有超时会话
 * Returns: 断开的会话数量
 */
int SessionTimer_ProcessTimeouts(void);

/*
 * SessionTimer_GetInfo - 获取定时器统计信息
 * Returns: Pointer to SessionTimerInfo
 */
const SessionTimerInfo* SessionTimer_GetInfo(void);

/*
 * SessionTimer_PrintStatistics - 打印定时器统计信息
 */
void SessionTimer_PrintStatistics(void);

/*
 * Week 6: 会话监控统计接口
 */

/*
 * Session_PrintAllStatistics - 打印所有会话的详细统计信息
 */
void Session_PrintAllStatistics(void);

/*
 * Session_GetActiveCount - 获取当前活跃会话数量
 * Returns: 活跃会话数
 */
int Session_GetActiveCount(void);

#endif /* __SESSION_TIMER_H__ */
