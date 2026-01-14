#ifndef MEMBER_SAFE_ENSURE_HISTORY_ORM_H
#define MEMBER_SAFE_ENSURE_HISTORY_ORM_H

#include "db_connection_manager.h"

/**
 * member_safe_ensure_history - 会员安全保障历史表
 *
 * 表结构:
 *   mod_flag       TINYINT(4)          修改标志
 *   mod_time       DATETIME             主键3 - 修改时间
 *   m_id           INT(10) UNSIGNED     主键1 - 会员ID
 *   occ_time       DATETIME             主键2 - 发生时间
 *   mobile_no      VARCHAR(15)          手机号
 *   service_flag   TINYINT(4)           服务标志
 *   type1_flag     TINYINT(4)           类型1标志
 *   type2_flag     TINYINT(4)           类型2标志
 *   expire_time    DATETIME             过期时间
 *   settle_id      VARCHAR(18)          结算ID
 *
 * 用途: 记录会员安全保障服务的历史变更（修改、删除等操作的历史记录）
 * 主键: (m_id, occ_time, mod_time)
 */

/* 结构体定义 */
typedef struct {
    char mod_flag;                  /* 修改标志 (0=插入, 1=更新, 2=删除) */
    char mod_time[20];              /* 修改时间 (主键3, YYYY-MM-DD HH:MM:SS) */
    unsigned int m_id;              /* 会员ID (主键1) */
    char occ_time[20];              /* 发生时间 (主键2, YYYY-MM-DD HH:MM:SS) */
    char mobile_no[16];             /* 手机号 */
    char service_flag;              /* 服务标志 */
    char type1_flag;                /* 类型1标志 */
    char type2_flag;                /* 类型2标志 */
    char expire_time[20];           /* 过期时间 (YYYY-MM-DD HH:MM:SS) */
    char settle_id[19];             /* 结算ID */
} MemberSafeEnsureHistory;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员安全保障历史记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全保障历史记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsureHistory_Add(DBConnectionManager* manager, const MemberSafeEnsureHistory* record);

/**
 * 获取会员安全保障历史记录（通过3字段复合主键）
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @param mod_time 修改时间
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsureHistory_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                  const char* mod_time, MemberSafeEnsureHistory* record);

/**
 * 更新会员安全保障历史记录
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全保障历史记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsureHistory_Update(DBConnectionManager* manager, const MemberSafeEnsureHistory* record);

/**
 * 删除会员安全保障历史记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @param mod_time 修改时间
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberSafeEnsureHistory_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                     const char* mod_time);

/**
 * 检查会员安全保障历史记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @param mod_time 修改时间
 * @return 1=存在, 0=不存在
 */
int MemberSafeEnsureHistory_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time,
                                     const char* mod_time);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按会员ID查询安全保障历史记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                            MemberSafeEnsureHistory* records, int max_records);

/**
 * 按手机号查询安全保障历史记录
 *
 * @param manager 数据库连接管理器
 * @param mobile_no 手机号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetByMobileNo(DBConnectionManager* manager, const char* mobile_no,
                                            MemberSafeEnsureHistory* records, int max_records);

/**
 * 按结算ID查询安全保障历史记录
 *
 * @param manager 数据库连接管理器
 * @param settle_id 结算ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetBySettleId(DBConnectionManager* manager, const char* settle_id,
                                            MemberSafeEnsureHistory* records, int max_records);

/**
 * 按修改标志查询
 *
 * @param manager 数据库连接管理器
 * @param mod_flag 修改标志 (0=插入, 1=更新, 2=删除)
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetByModFlag(DBConnectionManager* manager, int mod_flag,
                                           MemberSafeEnsureHistory* records, int max_records);

/**
 * 按修改时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetByModTimeRange(DBConnectionManager* manager, const char* start_time,
                                                const char* end_time, MemberSafeEnsureHistory* records, int max_records);

/**
 * 按发生时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetByOccTimeRange(DBConnectionManager* manager, const char* start_time,
                                                const char* end_time, MemberSafeEnsureHistory* records, int max_records);

/**
 * 获取最近的修改记录
 * 按修改时间降序排序
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetRecentModifications(DBConnectionManager* manager, MemberSafeEnsureHistory* records,
                                                     int max_records);

/**
 * 获取所有安全保障历史记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetAll(DBConnectionManager* manager, MemberSafeEnsureHistory* records,
                                     int offset, int limit);

/**
 * 获取安全保障历史记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberSafeEnsureHistory_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加安全保障历史记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberSafeEnsureHistory_BulkAdd(DBConnectionManager* manager, const MemberSafeEnsureHistory* records, int count);

/**
 * 批量删除安全保障历史记录（按会员ID）
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsureHistory_BulkDeleteByMemberIds(DBConnectionManager* manager, const unsigned int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印安全保障历史记录信息（用于调试）
 *
 * @param record 安全保障历史记录
 */
void MemberSafeEnsureHistory_PrintInfo(const MemberSafeEnsureHistory* record);

#endif /* MEMBER_SAFE_ENSURE_HISTORY_ORM_H */
