#ifndef EVENT_DUNGEON_CLEAR_ORM_H
#define EVENT_DUNGEON_CLEAR_ORM_H

#include "db_connection_manager.h"

/**
 * 活动副本通关记录 ORM
 *
 * 表: event_dungeon_clear
 * 主键: charac_no
 * 功能: 记录角色在活动副本的通关次数
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;             /* 角色编号 (主键) */
    unsigned int clear_cnt;             /* 通关次数 */
    char update_time[20];               /* 更新时间 (datetime, YYYY-MM-DD HH:MM:SS) */
} EventDungeonClear;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新活动副本通关记录
 */
int EventDungeonClear_Add(DBConnectionManager* manager, const EventDungeonClear* record);

/**
 * 获取活动副本通关记录
 */
int EventDungeonClear_Get(DBConnectionManager* manager, unsigned int charac_no,
                          EventDungeonClear* record);

/**
 * 更新活动副本通关记录
 */
int EventDungeonClear_Update(DBConnectionManager* manager, const EventDungeonClear* record);

/**
 * 删除活动副本通关记录
 */
int EventDungeonClear_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查活动副本通关记录是否存在
 */
int EventDungeonClear_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加通关次数
 * @param increment 增量值（默认为1）
 */
int EventDungeonClear_IncrementClearCount(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned int increment);

/**
 * 更新通关次数
 */
int EventDungeonClear_UpdateClearCount(DBConnectionManager* manager, unsigned int charac_no,
                                       unsigned int clear_cnt);

/**
 * 重置通关次数为0
 */
int EventDungeonClear_ResetClearCount(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 批量重置所有角色的通关次数
 */
int EventDungeonClear_BatchResetClearCounts(DBConnectionManager* manager);

/* ==================== 查询操作 ==================== */

/**
 * 按通关次数精确查询
 */
int EventDungeonClear_GetByClearCount(DBConnectionManager* manager, unsigned int clear_cnt,
                                      EventDungeonClear* records, int max_result);

/**
 * 按最小通关次数查询（用于排行榜）
 */
int EventDungeonClear_GetByMinClearCount(DBConnectionManager* manager, unsigned int min_clear_cnt,
                                         EventDungeonClear* records, int max_result);

/**
 * 获取通关次数TOP N
 */
int EventDungeonClear_GetTopByClearCount(DBConnectionManager* manager, int top_n,
                                         EventDungeonClear* records, int max_result);

/**
 * 按更新时间查询
 * @param after_time 查询此时间之后更新的记录 (YYYY-MM-DD HH:MM:SS)
 */
int EventDungeonClear_GetByUpdatedAfter(DBConnectionManager* manager, const char* after_time,
                                        EventDungeonClear* records, int max_result);

/**
 * 统计指定通关次数的角色数量
 */
int EventDungeonClear_CountByClearCount(DBConnectionManager* manager, unsigned int clear_cnt);

/**
 * 获取所有角色总通关次数
 */
unsigned long long EventDungeonClear_GetTotalClearCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印活动副本通关记录信息
 */
void EventDungeonClear_PrintInfo(const EventDungeonClear* record);

#endif /* EVENT_DUNGEON_CLEAR_ORM_H */
