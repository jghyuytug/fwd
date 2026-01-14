#ifndef MEMBER_ABNOMAL_ORM_H
#define MEMBER_ABNOMAL_ORM_H

#include "db_connection_manager.h"

/**
 * member_abnomal - 异常会员记录表
 *
 * 表结构:
 *   user_id           VARCHAR(12)         主键 - 用户ID
 *   overlab_count     SMALLINT(6)         重叠计数（异常行为次数）
 */

/* 结构体定义 */
typedef struct {
    char user_id[13];           /* 用户ID (主键) */
    short overlab_count;        /* 重叠计数 */
} MemberAbnomal;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加异常会员记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 异常会员记录
 * @return 0=成功, -1=失败
 */
int MemberAbnomal_Add(DBConnectionManager* manager, const MemberAbnomal* record);

/**
 * 根据用户ID获取异常会员记录
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberAbnomal_Get(DBConnectionManager* manager, const char* user_id, MemberAbnomal* record);

/**
 * 更新异常会员记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的异常会员记录
 * @return 0=成功, -1=失败
 */
int MemberAbnomal_Update(DBConnectionManager* manager, const MemberAbnomal* record);

/**
 * 删除异常会员记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberAbnomal_Delete(DBConnectionManager* manager, const char* user_id);

/**
 * 检查异常会员记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 1=存在, 0=不存在
 */
int MemberAbnomal_Exists(DBConnectionManager* manager, const char* user_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据重叠次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_count 最小次数
 * @param max_count 最大次数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberAbnomal_GetByCountRange(DBConnectionManager* manager, short min_count, short max_count,
                                   MemberAbnomal* records, int max_records);

/**
 * 获取高重叠次数记录（降序）
 *
 * @param manager 数据库连接管理器
 * @param min_count 最小次数阈值
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberAbnomal_GetHighCount(DBConnectionManager* manager, short min_count,
                                MemberAbnomal* records, int limit);

/**
 * 按用户ID模式查询
 *
 * @param manager 数据库连接管理器
 * @param pattern 用户ID模式（支持%通配符）
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberAbnomal_GetByUserIdPattern(DBConnectionManager* manager, const char* pattern,
                                      MemberAbnomal* records, int max_records);

/**
 * 递增重叠次数
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 0=成功, -1=失败
 */
int MemberAbnomal_IncrementCount(DBConnectionManager* manager, const char* user_id);

/**
 * 重置重叠次数为0
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return 0=成功, -1=失败
 */
int MemberAbnomal_ResetCount(DBConnectionManager* manager, const char* user_id);

/**
 * 获取所有异常会员记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberAbnomal_GetAll(DBConnectionManager* manager, MemberAbnomal* records,
                         int offset, int limit);

/**
 * 获取异常会员记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberAbnomal_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加异常会员记录
 *
 * @param manager 数据库连接管理器
 * @param records 异常会员记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberAbnomal_BulkAdd(DBConnectionManager* manager, const MemberAbnomal* records,
                          int count);

/**
 * 批量删除异常会员记录
 *
 * @param manager 数据库连接管理器
 * @param user_ids 用户ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberAbnomal_BulkDelete(DBConnectionManager* manager, const char** user_ids,
                              int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印异常会员记录（用于调试）
 *
 * @param record 异常会员记录
 */
void MemberAbnomal_PrintInfo(const MemberAbnomal* record);

#endif /* MEMBER_ABNOMAL_ORM_H */
