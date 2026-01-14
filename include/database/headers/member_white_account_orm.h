/**
 * DNF游戏服务器 - MemberWhiteAccount表ORM头文件
 *
 * 表名: member_white_account
 * 数据库: d_taiwan
 * 用途: 白名单账号管理，用于标记信任账号，避免反作弊系统误封
 *
 * 实施日期: 2025-11-14
 */

#ifndef MEMBER_WHITE_ACCOUNT_ORM_H
#define MEMBER_WHITE_ACCOUNT_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>
#include <time.h>

/**
 * 白名单账号结构体
 */
typedef struct {
    unsigned int m_id;          /* 会员ID（主键） */
    char reg_date[20];          /* 注册日期（YYYY-MM-DD HH:MM:SS） */
} MemberWhiteAccount;

/* ========================================================================
 * 初始化函数
 * ======================================================================== */

/**
 * 初始化白名单账号结构体
 * @param account 白名单账号结构体指针
 */
void MemberWhiteAccount_Init(MemberWhiteAccount* account);

/* ========================================================================
 * CRUD操作
 * ======================================================================== */

/**
 * 添加白名单账号
 * @param manager 数据库连接管理器
 * @param account 白名单账号结构体
 * @return 0成功，-1失败
 */
int MemberWhiteAccount_Add(DBConnectionManager* manager, MemberWhiteAccount* account);

/**
 * 获取白名单记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param account 用于存储结果的结构体指针
 * @return 0成功，-1失败
 */
int MemberWhiteAccount_Get(DBConnectionManager* manager, unsigned int m_id, MemberWhiteAccount* account);

/**
 * 删除白名单账号
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0成功，-1失败
 */
int MemberWhiteAccount_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查账号是否在白名单中
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return true存在，false不存在
 */
bool MemberWhiteAccount_Exists(DBConnectionManager* manager, unsigned int m_id);

/* ========================================================================
 * 业务查询
 * ======================================================================== */

/**
 * 获取所有白名单账号（分页）
 * @param manager 数据库连接管理器
 * @param accounts 用于存储结果的数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际返回的记录数，-1表示失败
 */
int MemberWhiteAccount_GetAll(DBConnectionManager* manager, MemberWhiteAccount* accounts, int offset, int limit);

/**
 * 获取白名单总数
 * @param manager 数据库连接管理器
 * @return 白名单总数，-1表示失败
 */
int MemberWhiteAccount_GetCount(DBConnectionManager* manager);

/**
 * 按日期范围查询白名单
 * @param manager 数据库连接管理器
 * @param start_date 开始日期（YYYY-MM-DD HH:MM:SS）
 * @param end_date 结束日期（YYYY-MM-DD HH:MM:SS）
 * @param accounts 用于存储结果的数组
 * @param max_count 数组最大容量
 * @return 实际返回的记录数，-1表示失败
 */
int MemberWhiteAccount_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                       MemberWhiteAccount* accounts, int max_count);

/**
 * 获取最近添加的白名单账号
 * @param manager 数据库连接管理器
 * @param accounts 用于存储结果的数组
 * @param limit 返回数量限制
 * @return 实际返回的记录数，-1表示失败
 */
int MemberWhiteAccount_GetRecentAdded(DBConnectionManager* manager, MemberWhiteAccount* accounts, int limit);

/**
 * 快速检查账号是否在白名单中（业务函数，用于反作弊判断）
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return true在白名单中，false不在
 */
bool MemberWhiteAccount_IsWhitelisted(DBConnectionManager* manager, unsigned int m_id);

/**
 * 获取过期/长期未活动的白名单记录（用于清理）
 * @param manager 数据库连接管理器
 * @param days_ago 多少天之前添加的记录
 * @param accounts 用于存储结果的数组
 * @param max_count 数组最大容量
 * @return 实际返回的记录数，-1表示失败
 */
int MemberWhiteAccount_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                      MemberWhiteAccount* accounts, int max_count);

/**
 * 批量删除指定日期范围的白名单
 * @param manager 数据库连接管理器
 * @param start_date 开始日期（YYYY-MM-DD HH:MM:SS）
 * @param end_date 结束日期（YYYY-MM-DD HH:MM:SS）
 * @return 删除的记录数，-1表示失败
 */
int MemberWhiteAccount_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date);

/* ========================================================================
 * 批量操作
 * ======================================================================== */

/**
 * 批量添加白名单账号
 * @param manager 数据库连接管理器
 * @param accounts 白名单账号数组
 * @param count 数组长度
 * @return 成功添加的记录数
 */
int MemberWhiteAccount_AddBatch(DBConnectionManager* manager, MemberWhiteAccount* accounts, int count);

/**
 * 批量删除白名单账号
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 成功删除的记录数
 */
int MemberWhiteAccount_DeleteBatch(DBConnectionManager* manager, unsigned int* m_ids, int count);

/* ========================================================================
 * 工具函数
 * ======================================================================== */

/**
 * 打印白名单账号信息
 * @param account 白名单账号结构体指针
 */
void MemberWhiteAccount_Print(const MemberWhiteAccount* account);

#endif /* MEMBER_WHITE_ACCOUNT_ORM_H */
