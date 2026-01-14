#ifndef ACCOUNTSS_ORM_H
#define ACCOUNTSS_ORM_H

#include "db_connection_manager.h"

/**
 * accountss - 账户备用表
 *
 * 表结构：
 * - uid (int, auto_increment): 用户ID（主键）
 * - accountname (varchar(255)): 账户名
 * - password (varchar(255)): 密码
 *
 * 业务说明：
 * - 账户备份表，用于存储账户的副本数据
 * - 与 accounts 表结构相同，用作冗余或备份
 * - 主要用于数据恢复、迁移或历史记录
 */

/* 数据结构定义 */
typedef struct {
    int uid;                    /* 用户ID（主键，自增） */
    char accountname[256];      /* 账户名 */
    char password[256];         /* 密码 */
} Accountss;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加账户记录
 * @param manager 数据库连接管理器
 * @param account 账户记录
 * @return 新插入的uid值（成功）, <0（失败）
 */
int Accountss_Add(DBConnectionManager* manager, const Accountss* account);

/**
 * 根据uid查询账户记录
 * @param manager 数据库连接管理器
 * @param uid 用户ID
 * @param account 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int Accountss_Get(DBConnectionManager* manager, int uid, Accountss* account);

/**
 * 根据账户名查询账户记录
 * @param manager 数据库连接管理器
 * @param accountname 账户名
 * @param account 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int Accountss_GetByAccountname(DBConnectionManager* manager, const char* accountname, Accountss* account);

/**
 * 更新账户记录
 * @param manager 数据库连接管理器
 * @param account 账户记录
 * @return 0=成功, <0=失败
 */
int Accountss_Update(DBConnectionManager* manager, const Accountss* account);

/**
 * 删除账户记录
 * @param manager 数据库连接管理器
 * @param uid 用户ID
 * @return 0=成功, <0=失败
 */
int Accountss_Delete(DBConnectionManager* manager, int uid);

/**
 * 检查账户是否存在
 * @param manager 数据库连接管理器
 * @param uid 用户ID
 * @return 1=存在, 0=不存在, <0=错误
 */
int Accountss_Exists(DBConnectionManager* manager, int uid);

/**
 * 检查账户名是否存在
 * @param manager 数据库连接管理器
 * @param accountname 账户名
 * @return 1=存在, 0=不存在, <0=错误
 */
int Accountss_ExistsByAccountname(DBConnectionManager* manager, const char* accountname);

/* ==================== 专项操作 ==================== */

/**
 * 更新密码
 * @param manager 数据库连接管理器
 * @param uid 用户ID
 * @param new_password 新密码
 * @return 0=成功, <0=失败
 */
int Accountss_UpdatePassword(DBConnectionManager* manager, int uid, const char* new_password);

/**
 * 验证账户密码
 * @param manager 数据库连接管理器
 * @param accountname 账户名
 * @param password 密码
 * @return 1=验证成功, 0=验证失败, <0=错误
 */
int Accountss_VerifyPassword(DBConnectionManager* manager, const char* accountname, const char* password);

/* ==================== 查询操作 ==================== */

/**
 * 统计总账户数
 * @param manager 数据库连接管理器
 * @return 账户数量, <0=错误
 */
int Accountss_CountTotal(DBConnectionManager* manager);

/**
 * 按账户名模糊查询数量
 * @param manager 数据库连接管理器
 * @param pattern 账户名模式（支持%通配符）
 * @return 匹配数量, <0=错误
 */
int Accountss_CountByPattern(DBConnectionManager* manager, const char* pattern);

/* ==================== 批量操作 ==================== */

/**
 * 批量删除账户（按uid列表）
 * @param manager 数据库连接管理器
 * @param uids uid数组
 * @param count 数组长度
 * @return 0=成功, <0=失败
 */
int Accountss_BatchDelete(DBConnectionManager* manager, const int* uids, int count);

/* ==================== 工具函数 ==================== */

/**
 * 打印账户记录信息（调试用）
 * @param account 账户记录
 */
void Accountss_PrintInfo(const Accountss* account);

/**
 * 释放账户记录内存（如果有动态分配）
 * @param account 账户记录
 */
void Accountss_Free(Accountss* account);

#endif /* ACCOUNTSS_ORM_H */
