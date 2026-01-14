#ifndef MEMBER_MILES_ORM_H
#define MEMBER_MILES_ORM_H

#include "db_connection_manager.h"

/**
 * member_miles - 会员里程积分表
 *
 * 表结构:
 *   m_id         INT(11)      主键 - 会员ID
 *   miles        INT(11)      总里程数
 *   daily_miles  SMALLINT(6)  每日里程数
 */

/* 结构体定义 */
typedef struct {
    int m_id;                   /* 会员ID (主键) */
    int miles;                  /* 总里程数 */
    short daily_miles;          /* 每日里程数 */
} MemberMiles;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员里程记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员里程记录
 * @return 0=成功, -1=失败
 */
int MemberMiles_Add(DBConnectionManager* manager, const MemberMiles* record);

/**
 * 根据会员ID获取里程记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberMiles_Get(DBConnectionManager* manager, int m_id, MemberMiles* record);

/**
 * 更新会员里程记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的里程记录
 * @return 0=成功, -1=失败
 */
int MemberMiles_Update(DBConnectionManager* manager, const MemberMiles* record);

/**
 * 删除会员里程记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberMiles_Delete(DBConnectionManager* manager, int m_id);

/**
 * 检查会员里程记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberMiles_Exists(DBConnectionManager* manager, int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据总里程数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_miles 最小里程数
 * @param max_miles 最大里程数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMiles_GetByMilesRange(DBConnectionManager* manager, int min_miles,
                                  int max_miles, MemberMiles* records, int max_records);

/**
 * 根据每日里程数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_daily 最小每日里程数
 * @param max_daily 最大每日里程数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMiles_GetByDailyMilesRange(DBConnectionManager* manager, short min_daily,
                                       short max_daily, MemberMiles* records, int max_records);

/**
 * 获取总里程数排行榜（按miles降序）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 排行榜数量限制（例如前100名）
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMiles_GetTopMiles(DBConnectionManager* manager, MemberMiles* records, int max_records);

/**
 * 获取所有会员里程记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMiles_GetAll(DBConnectionManager* manager, MemberMiles* records,
                        int offset, int limit);

/**
 * 获取会员里程记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberMiles_GetCount(DBConnectionManager* manager);

/**
 * 增加会员总里程数（原子操作）
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param miles_to_add 要增加的里程数
 * @return 0=成功, -1=失败
 */
int MemberMiles_IncrementMiles(DBConnectionManager* manager, int m_id, int miles_to_add);

/**
 * 重置会员每日里程数（用于每日重置）
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberMiles_ResetDailyMiles(DBConnectionManager* manager, int m_id);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加会员里程记录
 *
 * @param manager 数据库连接管理器
 * @param records 里程记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberMiles_BulkAdd(DBConnectionManager* manager, const MemberMiles* records, int count);

/**
 * 批量删除会员里程记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberMiles_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印会员里程记录（用于调试）
 *
 * @param record 里程记录
 */
void MemberMiles_PrintInfo(const MemberMiles* record);

#endif /* MEMBER_MILES_ORM_H */
