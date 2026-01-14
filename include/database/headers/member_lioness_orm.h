#ifndef MEMBER_LIONESS_ORM_H
#define MEMBER_LIONESS_ORM_H

#include "db_connection_manager.h"

/**
 * member_lioness - 特殊会员标记表
 *
 * 表结构:
 *   user_id  VARCHAR(30)  主键 - 用户ID
 *
 * 用途: 标记特殊会员（如女性玩家、活动参与者等）
 */

/* 结构体定义 */
typedef struct {
    char user_id[31];           /* 用户ID (主键) */
} MemberLioness;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员标记（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 0=成功, -1=失败
 */
int MemberLioness_Add(DBConnectionManager* manager, const char* user_id);

/**
 * 删除会员标记（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberLioness_Delete(DBConnectionManager* manager, const char* user_id);

/**
 * 检查会员标记是否存在
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 1=存在, 0=不存在
 */
int MemberLioness_Exists(DBConnectionManager* manager, const char* user_id);

/**
 * 获取所有会员标记（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetAll(DBConnectionManager* manager, MemberLioness* records,
                          int offset, int limit);

/**
 * 获取会员标记总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberLioness_GetCount(DBConnectionManager* manager);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 按用户ID模式查询（LIKE查询）
 *
 * @param manager 数据库连接管理器
 * @param pattern 模式字符串（如 "user%", "%test%"）
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetByPattern(DBConnectionManager* manager, const char* pattern,
                                 MemberLioness* records, int max_records);

/**
 * 按前缀查询
 *
 * @param manager 数据库连接管理器
 * @param prefix 前缀字符串
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetByPrefix(DBConnectionManager* manager, const char* prefix,
                                MemberLioness* records, int max_records);

/**
 * 按后缀查询
 *
 * @param manager 数据库连接管理器
 * @param suffix 后缀字符串
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetBySuffix(DBConnectionManager* manager, const char* suffix,
                                MemberLioness* records, int max_records);

/**
 * 包含特定字符串的user_id查询
 *
 * @param manager 数据库连接管理器
 * @param substring 子字符串
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetContaining(DBConnectionManager* manager, const char* substring,
                                  MemberLioness* records, int max_records);

/**
 * 按user_id长度范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_length 最小长度
 * @param max_length 最大长度
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetByLengthRange(DBConnectionManager* manager, int min_length,
                                     int max_length, MemberLioness* records, int max_records);

/**
 * 获取随机会员标记（用于抽奖等）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 要获取的随机记录数
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetRandom(DBConnectionManager* manager, MemberLioness* records, int max_records);

/**
 * 获取按user_id排序的记录（升序）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberLioness_GetSorted(DBConnectionManager* manager, MemberLioness* records, int max_records);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加会员标记
 *
 * @param manager 数据库连接管理器
 * @param user_ids 用户ID数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberLioness_BulkAdd(DBConnectionManager* manager, const char** user_ids, int count);

/**
 * 批量删除会员标记
 *
 * @param manager 数据库连接管理器
 * @param user_ids 用户ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberLioness_BulkDelete(DBConnectionManager* manager, const char** user_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印会员标记信息（用于调试）
 *
 * @param record 会员标记记录
 */
void MemberLioness_PrintInfo(const MemberLioness* record);

#endif /* MEMBER_LIONESS_ORM_H */
