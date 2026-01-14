#ifndef MEMBER_HANDICAP_ORM_H
#define MEMBER_HANDICAP_ORM_H

#include "db_connection_manager.h"

/**
 * member_handicap - 会员能力值调整表
 *
 * 表结构:
 *   event_id        INT(11)              主键1 - 事件ID
 *   cap_type        TINYINT(3) UNSIGNED  主键2 - 能力类型
 *   server_id       TINYINT(4)           主键3 - 服务器ID
 *   m_id            INT(11)              主键4 - 会员ID
 *   start_time      DATETIME             主键5 - 开始时间
 *   end_time        DATETIME             结束时间
 *   handicap_value  INT(11)              能力值
 *
 * 用途: 记录活动期间的会员能力值加成/限制（如经验加倍、伤害加成等）
 * 主键: (event_id, cap_type, server_id, m_id, start_time)
 */

/* 结构体定义 */
typedef struct {
    int event_id;                   /* 事件ID (主键1) */
    unsigned char cap_type;         /* 能力类型 (主键2) */
    char server_id;                 /* 服务器ID (主键3) */
    int m_id;                       /* 会员ID (主键4) */
    char start_time[20];            /* 开始时间 (主键5, YYYY-MM-DD HH:MM:SS) */
    char end_time[20];              /* 结束时间 (YYYY-MM-DD HH:MM:SS) */
    int handicap_value;             /* 能力值 */
} MemberHandicap;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加能力值调整记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 能力值调整记录
 * @return 0=成功, -1=失败
 */
int MemberHandicap_Add(DBConnectionManager* manager, const MemberHandicap* record);

/**
 * 获取能力值调整记录（通过5字段复合主键）
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param cap_type 能力类型
 * @param server_id 服务器ID
 * @param m_id 会员ID
 * @param start_time 开始时间
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberHandicap_Get(DBConnectionManager* manager, int event_id, int cap_type,
                        int server_id, int m_id, const char* start_time, MemberHandicap* record);

/**
 * 更新能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param record 能力值调整记录
 * @return 0=成功, -1=失败
 */
int MemberHandicap_Update(DBConnectionManager* manager, const MemberHandicap* record);

/**
 * 删除能力值调整记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param cap_type 能力类型
 * @param server_id 服务器ID
 * @param m_id 会员ID
 * @param start_time 开始时间
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberHandicap_Delete(DBConnectionManager* manager, int event_id, int cap_type,
                           int server_id, int m_id, const char* start_time);

/**
 * 检查能力值调整记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param cap_type 能力类型
 * @param server_id 服务器ID
 * @param m_id 会员ID
 * @param start_time 开始时间
 * @return 1=存在, 0=不存在
 */
int MemberHandicap_Exists(DBConnectionManager* manager, int event_id, int cap_type,
                           int server_id, int m_id, const char* start_time);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按事件ID查询能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetByEventId(DBConnectionManager* manager, int event_id,
                                  MemberHandicap* records, int max_records);

/**
 * 按会员ID查询能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   MemberHandicap* records, int max_records);

/**
 * 按服务器ID查询能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetByServerId(DBConnectionManager* manager, int server_id,
                                   MemberHandicap* records, int max_records);

/**
 * 按能力类型查询能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param cap_type 能力类型
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetByCapType(DBConnectionManager* manager, int cap_type,
                                  MemberHandicap* records, int max_records);

/**
 * 获取当前活动的能力值调整记录
 * 查询条件: NOW() BETWEEN start_time AND end_time
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetActiveHandicaps(DBConnectionManager* manager, MemberHandicap* records, int max_records);

/**
 * 按时间范围查询能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                    const char* end_time, MemberHandicap* records, int max_records);

/**
 * 获取所有能力值调整记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberHandicap_GetAll(DBConnectionManager* manager, MemberHandicap* records,
                           int offset, int limit);

/**
 * 获取能力值调整记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberHandicap_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加能力值调整记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberHandicap_BulkAdd(DBConnectionManager* manager, const MemberHandicap* records, int count);

/**
 * 批量删除能力值调整记录（按事件ID）
 *
 * @param manager 数据库连接管理器
 * @param event_ids 事件ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberHandicap_BulkDeleteByEventIds(DBConnectionManager* manager, const int* event_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印能力值调整记录信息（用于调试）
 *
 * @param record 能力值调整记录
 */
void MemberHandicap_PrintInfo(const MemberHandicap* record);

#endif /* MEMBER_HANDICAP_ORM_H */
