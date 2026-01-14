#ifndef DNF_EVENT_LOG_ORM_H
#define DNF_EVENT_LOG_ORM_H

#include "db_connection_manager.h"

/**
 * @file dnf_event_log_orm.h
 * @brief DNF活动日志表 ORM 接口
 *
 * 数据库: d_taiwan
 * 表名: dnf_event_log
 * 用途: 记录游戏中各类活动的详细日志
 */

/* 数据结构定义 */
typedef struct {
    unsigned int log_id;            /* 日志ID (自增主键) */
    int occ_time;                   /* 发生时间 (Unix时间戳) */
    unsigned char event_type;       /* 活动类型 (tinyint unsigned) */
    unsigned int parameter1;        /* 参数1 */
    unsigned int parameter2;        /* 参数2 */
    unsigned char server_id;        /* 服务器ID (tinyint unsigned) */
    signed char event_flag;         /* 活动标志 (tinyint) */
    int start_time;                 /* 开始时间 (Unix时间戳) */
    int end_time;                   /* 结束时间 (Unix时间戳) */
    int m_id;                       /* 账户ID */
    char expl[201];                 /* 说明 (varchar 200) */
    char etc[101];                  /* 其他信息 (varchar 100) */
} DnfEventLog;

/* CRUD 操作 */

/**
 * @brief 添加活动日志记录 (自增主键会被忽略)
 * @param manager 数据库连接管理器
 * @param record 日志记录指针
 * @param inserted_id 输出参数，返回插入的log_id (可为NULL)
 * @return 0=成功, <0=失败
 */
int DnfEventLog_Add(DBConnectionManager* manager, const DnfEventLog* record, unsigned int* inserted_id);

/**
 * @brief 获取活动日志记录
 * @param manager 数据库连接管理器
 * @param log_id 日志ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, <0=失败
 */
int DnfEventLog_Get(DBConnectionManager* manager, unsigned int log_id, DnfEventLog* record);

/**
 * @brief 更新活动日志记录
 * @param manager 数据库连接管理器
 * @param record 日志记录指针
 * @return 0=成功, <0=失败
 */
int DnfEventLog_Update(DBConnectionManager* manager, const DnfEventLog* record);

/**
 * @brief 删除活动日志记录
 * @param manager 数据库连接管理器
 * @param log_id 日志ID
 * @return 0=成功, <0=失败
 */
int DnfEventLog_Delete(DBConnectionManager* manager, unsigned int log_id);

/**
 * @brief 检查活动日志记录是否存在
 * @param manager 数据库连接管理器
 * @param log_id 日志ID
 * @return 1=存在, 0=不存在, <0=错误
 */
int DnfEventLog_Exists(DBConnectionManager* manager, unsigned int log_id);

/* 业务操作 */

/**
 * @brief 获取指定时间范围内的日志数量
 * @param manager 数据库连接管理器
 * @param start_time 开始时间 (Unix时间戳)
 * @param end_time 结束时间 (Unix时间戳)
 * @return 日志数量, <0=错误
 */
int DnfEventLog_GetCountByTimeRange(DBConnectionManager* manager, int start_time, int end_time);

/**
 * @brief 获取特定活动类型的日志数量
 * @param manager 数据库连接管理器
 * @param event_type 活动类型
 * @return 日志数量, <0=错误
 */
int DnfEventLog_GetCountByEventType(DBConnectionManager* manager, unsigned char event_type);

/**
 * @brief 获取特定服务器的日志数量
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @return 日志数量, <0=错误
 */
int DnfEventLog_GetCountByServer(DBConnectionManager* manager, unsigned char server_id);

/**
 * @brief 获取特定账户的活动日志数量
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @return 日志数量, <0=错误
 */
int DnfEventLog_GetCountByAccount(DBConnectionManager* manager, int m_id);

/**
 * @brief 删除指定时间之前的旧日志 (日志清理)
 * @param manager 数据库连接管理器
 * @param before_time 时间戳，删除此时间之前的日志
 * @return 删除的记录数, <0=错误
 */
int DnfEventLog_DeleteOldLogs(DBConnectionManager* manager, int before_time);

/**
 * @brief 按活动类型和时间范围删除日志
 * @param manager 数据库连接管理器
 * @param event_type 活动类型
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @return 删除的记录数, <0=错误
 */
int DnfEventLog_DeleteByEventTypeAndTime(DBConnectionManager* manager, unsigned char event_type,
                                         int start_time, int end_time);

/**
 * @brief 获取最新的N条日志ID
 * @param manager 数据库连接管理器
 * @param limit 返回数量限制
 * @param log_ids 输出参数，日志ID数组 (需预先分配)
 * @return 实际返回的数量, <0=错误
 */
int DnfEventLog_GetLatestLogIds(DBConnectionManager* manager, int limit, unsigned int* log_ids);

/* 工具函数 */

/**
 * @brief 打印活动日志记录信息（调试用）
 * @param record 日志记录指针
 */
void DnfEventLog_PrintInfo(const DnfEventLog* record);

#endif /* DNF_EVENT_LOG_ORM_H */
