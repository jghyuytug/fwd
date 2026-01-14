#ifndef MEMBER_SAFE_ENSURE_ORM_H
#define MEMBER_SAFE_ENSURE_ORM_H

#include "db_connection_manager.h"

/**
 * member_safe_ensure - 会员安全保障表
 *
 * 表结构:
 *   m_id           INT(10) UNSIGNED     主键1 - 会员ID
 *   occ_time       DATETIME             主键2 - 发生时间
 *   mobile_no      VARCHAR(15)          手机号
 *   service_flag   TINYINT(4)           服务标志
 *   type1_flag     TINYINT(4)           类型1标志
 *   type2_flag     TINYINT(4)           类型2标志
 *   expire_time    DATETIME             过期时间
 *   settle_id      VARCHAR(18)          结算ID
 *
 * 用途: 记录会员安全保障服务信息（手机绑定、安全认证等）
 * 主键: (m_id, occ_time)
 */

/* 结构体定义 */
typedef struct {
    unsigned int m_id;              /* 会员ID (主键1) */
    char occ_time[20];              /* 发生时间 (主键2, YYYY-MM-DD HH:MM:SS) */
    char mobile_no[16];             /* 手机号 */
    char service_flag;              /* 服务标志 */
    char type1_flag;                /* 类型1标志 */
    char type2_flag;                /* 类型2标志 */
    char expire_time[20];           /* 过期时间 (YYYY-MM-DD HH:MM:SS) */
    char settle_id[19];             /* 结算ID */
} MemberSafeEnsure;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员安全保障记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全保障记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsure_Add(DBConnectionManager* manager, const MemberSafeEnsure* record);

/**
 * 获取会员安全保障记录（通过复合主键）
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsure_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_time, MemberSafeEnsure* record);

/**
 * 更新会员安全保障记录
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全保障记录
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsure_Update(DBConnectionManager* manager, const MemberSafeEnsure* record);

/**
 * 删除会员安全保障记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberSafeEnsure_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);

/**
 * 检查会员安全保障记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param occ_time 发生时间
 * @return 1=存在, 0=不存在
 */
int MemberSafeEnsure_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按会员ID查询安全保障记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                    MemberSafeEnsure* records, int max_records);

/**
 * 按手机号查询安全保障记录
 *
 * @param manager 数据库连接管理器
 * @param mobile_no 手机号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetByMobileNo(DBConnectionManager* manager, const char* mobile_no,
                                    MemberSafeEnsure* records, int max_records);

/**
 * 按结算ID查询安全保障记录
 *
 * @param manager 数据库连接管理器
 * @param settle_id 结算ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetBySettleId(DBConnectionManager* manager, const char* settle_id,
                                    MemberSafeEnsure* records, int max_records);

/**
 * 获取当前有效的安全保障服务
 * 查询条件: expire_time > NOW()
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetActiveServices(DBConnectionManager* manager, MemberSafeEnsure* records, int max_records);

/**
 * 获取已过期的安全保障服务
 * 查询条件: expire_time <= NOW()
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetExpiredServices(DBConnectionManager* manager, MemberSafeEnsure* records, int max_records);

/**
 * 按服务标志查询
 *
 * @param manager 数据库连接管理器
 * @param service_flag 服务标志
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetByServiceFlag(DBConnectionManager* manager, int service_flag,
                                       MemberSafeEnsure* records, int max_records);

/**
 * 按时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                     const char* end_time, MemberSafeEnsure* records, int max_records);

/**
 * 获取所有安全保障记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSafeEnsure_GetAll(DBConnectionManager* manager, MemberSafeEnsure* records,
                             int offset, int limit);

/**
 * 获取安全保障记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberSafeEnsure_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加安全保障记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberSafeEnsure_BulkAdd(DBConnectionManager* manager, const MemberSafeEnsure* records, int count);

/**
 * 批量删除安全保障记录（按会员ID）
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberSafeEnsure_BulkDeleteByMemberIds(DBConnectionManager* manager, const unsigned int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印安全保障记录信息（用于调试）
 *
 * @param record 安全保障记录
 */
void MemberSafeEnsure_PrintInfo(const MemberSafeEnsure* record);

#endif /* MEMBER_SAFE_ENSURE_ORM_H */
