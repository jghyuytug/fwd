/**
 * DNF游戏服务器 - LogCombat表ORM头文件
 *
 * 功能: 战斗日志记录（攻击、技能、Buff、死亡事件）
 * 数据库: taiwan_cain_log
 * 表名: log_combat
 * 创建: 2025-11-23 (Phase 6.1)
 */

#ifndef LOG_COMBAT_ORM_H
#define LOG_COMBAT_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 数据结构定义
 * ======================================================================== */

/**
 * LogCombat - 战斗日志条目
 *
 * 字段说明:
 * - log_id: 日志ID（主键，自增）
 * - event_type: 事件类型（1=攻击, 2=技能, 3=Buff应用, 4=Buff移除, 5=死亡）
 * - attacker_id: 攻击者角色ID
 * - defender_id: 防御者角色ID
 * - skill_id: 技能ID（0表示普通攻击）
 * - damage: 伤害值
 * - is_critical: 是否暴击（0=否, 1=是）
 * - buff_id: Buff ID（仅用于Buff事件）
 * - timestamp: 事件时间戳
 */
typedef struct {
    int log_id;                /* 日志ID（主键，自增） */
    int event_type;            /* 事件类型 */
    int attacker_id;           /* 攻击者角色ID */
    int defender_id;           /* 防御者角色ID */
    int skill_id;              /* 技能ID */
    int damage;                /* 伤害值 */
    signed char is_critical;   /* 是否暴击 */
    int buff_id;               /* Buff ID */
    char timestamp[32];        /* 事件时间戳 */
} LogCombat;

/* ========================================================================
 * CRUD操作函数
 * ======================================================================== */

/**
 * 插入战斗日志记录
 *
 * @param manager 数据库连接管理器
 * @param log 战斗日志数据结构指针
 * @return 成功返回0，失败返回-1
 */
int LogCombat_Insert(DBConnectionManager* manager, LogCombat* log);

/**
 * 批量插入战斗日志记录
 *
 * @param manager 数据库连接管理器
 * @param logs 战斗日志数组
 * @param count 日志数量
 * @return 成功插入的数量，失败返回-1
 */
int LogCombat_BatchInsert(DBConnectionManager* manager, LogCombat* logs, int count);

/**
 * 根据日志ID查询战斗日志
 *
 * @param manager 数据库连接管理器
 * @param log_id 日志ID
 * @param log 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int LogCombat_GetByLogId(DBConnectionManager* manager, int log_id, LogCombat* log);

/**
 * 查询指定角色的战斗日志
 *
 * @param manager 数据库连接管理器
 * @param character_id 角色ID（可以是攻击者或防御者）
 * @param logs 输出数组，存储查询结果
 * @param max_count 最大返回数量
 * @return 实际返回的日志数量
 */
int LogCombat_GetByCharacterId(DBConnectionManager* manager, int character_id, LogCombat* logs, int max_count);

/**
 * 查询指定时间范围内的战斗日志
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（YYYY-MM-DD HH:MM:SS）
 * @param logs 输出数组，存储查询结果
 * @param max_count 最大返回数量
 * @return 实际返回的日志数量
 */
int LogCombat_GetByTimeRange(DBConnectionManager* manager, const char* start_time, const char* end_time, LogCombat* logs, int max_count);

/**
 * 删除指定时间之前的战斗日志（数据清理）
 *
 * @param manager 数据库连接管理器
 * @param before_time 时间界限（YYYY-MM-DD HH:MM:SS）
 * @return 删除的记录数量
 */
int LogCombat_DeleteBefore(DBConnectionManager* manager, const char* before_time);

/**
 * 获取战斗日志总数
 *
 * @param manager 数据库连接管理器
 * @return 日志总数
 */
int LogCombat_GetCount(DBConnectionManager* manager);

#ifdef __cplusplus
}
#endif

#endif /* LOG_COMBAT_ORM_H */
