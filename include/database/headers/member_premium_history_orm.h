#ifndef MEMBER_PREMIUM_HISTORY_ORM_H
#define MEMBER_PREMIUM_HISTORY_ORM_H

#include "db_connection_manager.h"

/**
 * member_premium_history - 会员高级会员服务历史表
 *
 * 表结构:
 *   event_id       INT(11)              主键1 - 事件ID
 *   pre_type       TINYINT(3) UNSIGNED  主键2 - 高级会员类型
 *   m_id           INT(11)              主键3 - 会员ID
 *   service_start  DATETIME             主键4 - 服务开始时间
 *   service_end    DATETIME             服务结束时间
 *
 * 用途: 记录会员高级会员服务历史（如VIP、SVIP等不同等级的服务记录）
 * 主键: (event_id, pre_type, m_id, service_start)
 */

/* 结构体定义 */
typedef struct {
    int event_id;                   /* 事件ID (主键1) */
    unsigned char pre_type;         /* 高级会员类型 (主键2) */
    int m_id;                       /* 会员ID (主键3) */
    char service_start[20];         /* 服务开始时间 (主键4, YYYY-MM-DD HH:MM:SS) */
    char service_end[20];           /* 服务结束时间 (YYYY-MM-DD HH:MM:SS) */
} MemberPremiumHistory;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加高级会员服务历史记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 高级会员服务历史记录
 * @return 0=成功, -1=失败
 */
int MemberPremiumHistory_Add(DBConnectionManager* manager, const MemberPremiumHistory* record);

/**
 * 获取高级会员服务历史记录（通过4字段复合主键）
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param pre_type 高级会员类型
 * @param m_id 会员ID
 * @param service_start 服务开始时间
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberPremiumHistory_Get(DBConnectionManager* manager, int event_id, int pre_type,
                              int m_id, const char* service_start, MemberPremiumHistory* record);

/**
 * 更新高级会员服务历史记录
 *
 * @param manager 数据库连接管理器
 * @param record 高级会员服务历史记录
 * @return 0=成功, -1=失败
 */
int MemberPremiumHistory_Update(DBConnectionManager* manager, const MemberPremiumHistory* record);

/**
 * 删除高级会员服务历史记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param pre_type 高级会员类型
 * @param m_id 会员ID
 * @param service_start 服务开始时间
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberPremiumHistory_Delete(DBConnectionManager* manager, int event_id, int pre_type,
                                 int m_id, const char* service_start);

/**
 * 检查高级会员服务历史记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param pre_type 高级会员类型
 * @param m_id 会员ID
 * @param service_start 服务开始时间
 * @return 1=存在, 0=不存在
 */
int MemberPremiumHistory_Exists(DBConnectionManager* manager, int event_id, int pre_type,
                                 int m_id, const char* service_start);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按事件ID查询高级会员服务历史记录
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetByEventId(DBConnectionManager* manager, int event_id,
                                       MemberPremiumHistory* records, int max_records);

/**
 * 按会员ID查询高级会员服务历史记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                        MemberPremiumHistory* records, int max_records);

/**
 * 按高级会员类型查询历史记录
 *
 * @param manager 数据库连接管理器
 * @param pre_type 高级会员类型
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetByPreType(DBConnectionManager* manager, int pre_type,
                                       MemberPremiumHistory* records, int max_records);

/**
 * 获取当前活动的高级会员服务
 * 查询条件: NOW() BETWEEN service_start AND service_end
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetActiveServices(DBConnectionManager* manager, MemberPremiumHistory* records, int max_records);

/**
 * 按服务开始时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                         const char* end_time, MemberPremiumHistory* records, int max_records);

/**
 * 获取即将过期的服务（N天内过期）
 *
 * @param manager 数据库连接管理器
 * @param days 天数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetExpiringSoon(DBConnectionManager* manager, int days,
                                          MemberPremiumHistory* records, int max_records);

/**
 * 获取所有高级会员服务历史记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPremiumHistory_GetAll(DBConnectionManager* manager, MemberPremiumHistory* records,
                                 int offset, int limit);

/**
 * 获取高级会员服务历史记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberPremiumHistory_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加高级会员服务历史记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberPremiumHistory_BulkAdd(DBConnectionManager* manager, const MemberPremiumHistory* records, int count);

/**
 * 批量删除高级会员服务历史记录（按事件ID）
 *
 * @param manager 数据库连接管理器
 * @param event_ids 事件ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberPremiumHistory_BulkDeleteByEventIds(DBConnectionManager* manager, const int* event_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印高级会员服务历史记录信息（用于调试）
 *
 * @param record 高级会员服务历史记录
 */
void MemberPremiumHistory_PrintInfo(const MemberPremiumHistory* record);

#endif /* MEMBER_PREMIUM_HISTORY_ORM_H */
