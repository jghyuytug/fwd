#ifndef MEMBER_MOUSE_SMS_ORM_H
#define MEMBER_MOUSE_SMS_ORM_H

#include "db_connection_manager.h"

/**
 * member_mouse_sms - 会员短信验证记录表
 *
 * 表结构:
 *   m_id      INT(11)              主键 - 会员ID
 *   occ_time  DATETIME             发生时间
 *   cnt       TINYINT(3) UNSIGNED  验证次数
 *
 * 用途: 记录会员短信验证的发生时间和次数（用于防刷验证、限流等）
 */

/* 结构体定义 */
typedef struct {
    int m_id;                       /* 会员ID (主键) */
    char occ_time[20];             /* 发生时间 (YYYY-MM-DD HH:MM:SS) */
    unsigned char cnt;             /* 验证次数 */
} MemberMouseSms;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加短信验证记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 短信验证记录
 * @return 0=成功, -1=失败
 */
int MemberMouseSms_Add(DBConnectionManager* manager, const MemberMouseSms* record);

/**
 * 获取短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberMouseSms_Get(DBConnectionManager* manager, int m_id, MemberMouseSms* record);

/**
 * 更新短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param record 短信验证记录
 * @return 0=成功, -1=失败
 */
int MemberMouseSms_Update(DBConnectionManager* manager, const MemberMouseSms* record);

/**
 * 删除短信验证记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberMouseSms_Delete(DBConnectionManager* manager, int m_id);

/**
 * 检查短信验证记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberMouseSms_Exists(DBConnectionManager* manager, int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按时间范围查询短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间 (格式: "YYYY-MM-DD HH:MM:SS")
 * @param end_time 结束时间 (格式: "YYYY-MM-DD HH:MM:SS")
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMouseSms_GetByOccTimeRange(DBConnectionManager* manager, const char* start_time,
                                      const char* end_time, MemberMouseSms* records, int max_records);

/**
 * 按验证次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_cnt 最小次数
 * @param max_cnt 最大次数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMouseSms_GetByCntRange(DBConnectionManager* manager, int min_cnt,
                                  int max_cnt, MemberMouseSms* records, int max_records);

/**
 * 获取最近的短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMouseSms_GetRecent(DBConnectionManager* manager, MemberMouseSms* records, int max_records);

/**
 * 获取旧的短信验证记录（N天前）
 *
 * @param manager 数据库连接管理器
 * @param days_ago 多少天前
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMouseSms_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                  MemberMouseSms* records, int max_records);

/**
 * 获取所有短信验证记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberMouseSms_GetAll(DBConnectionManager* manager, MemberMouseSms* records,
                           int offset, int limit);

/**
 * 获取短信验证记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberMouseSms_GetCount(DBConnectionManager* manager);

/**
 * 原子递增验证次数
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberMouseSms_IncrementCnt(DBConnectionManager* manager, int m_id);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberMouseSms_BulkAdd(DBConnectionManager* manager, const MemberMouseSms* records, int count);

/**
 * 批量删除短信验证记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberMouseSms_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印短信验证记录信息（用于调试）
 *
 * @param record 短信验证记录
 */
void MemberMouseSms_PrintInfo(const MemberMouseSms* record);

#endif /* MEMBER_MOUSE_SMS_ORM_H */
