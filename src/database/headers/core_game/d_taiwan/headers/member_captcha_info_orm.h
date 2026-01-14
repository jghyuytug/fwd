#ifndef MEMBER_CAPTCHA_INFO_ORM_H
#define MEMBER_CAPTCHA_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * member_captcha_info - 会员验证码信息表
 *
 * 表结构:
 *   m_id         INT(10) UNSIGNED    主键 - 会员ID
 *   cert_time    INT(10) UNSIGNED    验证时间戳
 *   fail_count   TINYINT(3) UNSIGNED 失败次数
 */

/* 结构体定义 */
typedef struct {
    unsigned int m_id;              /* 会员ID (主键) */
    unsigned int cert_time;         /* 验证时间戳 */
    unsigned char fail_count;       /* 失败次数 */
} MemberCaptchaInfo;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加验证码记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 验证码记录
 * @return 0=成功, -1=失败
 */
int MemberCaptchaInfo_Add(DBConnectionManager* manager, const MemberCaptchaInfo* record);

/**
 * 根据会员ID获取验证码记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberCaptchaInfo_Get(DBConnectionManager* manager, unsigned int m_id, MemberCaptchaInfo* record);

/**
 * 更新验证码记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的验证码记录
 * @return 0=成功, -1=失败
 */
int MemberCaptchaInfo_Update(DBConnectionManager* manager, const MemberCaptchaInfo* record);

/**
 * 删除验证码记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberCaptchaInfo_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查验证码记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberCaptchaInfo_Exists(DBConnectionManager* manager, unsigned int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 增加失败次数
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberCaptchaInfo_IncrementFailCount(DBConnectionManager* manager, unsigned int m_id);

/**
 * 重置失败次数
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberCaptchaInfo_ResetFailCount(DBConnectionManager* manager, unsigned int m_id);

/**
 * 根据失败次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_count 最小失败次数
 * @param max_count_val 最大失败次数
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberCaptchaInfo_GetByFailCountRange(DBConnectionManager* manager, int min_count,
                                           int max_count_val, MemberCaptchaInfo* records,
                                           int max_count);

/**
 * 根据时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 起始时间戳
 * @param end_time 结束时间戳
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberCaptchaInfo_GetByTimeRange(DBConnectionManager* manager, unsigned int start_time,
                                      unsigned int end_time, MemberCaptchaInfo* records,
                                      int max_count);

/**
 * 获取最近验证的记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberCaptchaInfo_GetRecentVerified(DBConnectionManager* manager, MemberCaptchaInfo* records,
                                         int limit);

/**
 * 获取所有验证码记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberCaptchaInfo_GetAll(DBConnectionManager* manager, MemberCaptchaInfo* records,
                              int offset, int limit);

/**
 * 获取验证码记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberCaptchaInfo_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加验证码记录
 *
 * @param manager 数据库连接管理器
 * @param records 验证码记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberCaptchaInfo_BulkAdd(DBConnectionManager* manager, const MemberCaptchaInfo* records,
                               int count);

/**
 * 批量删除验证码记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberCaptchaInfo_BulkDelete(DBConnectionManager* manager, const unsigned int* m_ids,
                                  int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印验证码记录信息（用于调试）
 *
 * @param record 验证码记录
 */
void MemberCaptchaInfo_PrintInfo(const MemberCaptchaInfo* record);

#endif /* MEMBER_CAPTCHA_INFO_ORM_H */
