#ifndef ACCOUNTS_ORM_H
#define ACCOUNTS_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// Account 结构体定义
// ========================================================================

typedef struct {
    int UID;                         // 用户ID（主键，自增）
    char accountname[256];           // 账号名称（唯一）
    char password[256];              // 密码
    char qq[256];                    // QQ号（可选）
    int dzuid;                       // DZ用户ID（可选）
    int billing;                     // 计费标识（可选）
    char VIP[256];                   // VIP标识
} Account;

// ========================================================================
// CRUD 操作函数
// ========================================================================

/**
 * 创建新的账号
 * @param manager 数据库连接管理器
 * @param account 账号信息
 * @return 成功返回0，失败返回-1
 */
int Account_Create(DBConnectionManager* manager, Account* account);

/**
 * 根据UID查询账号
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @param account 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int Account_GetByUID(DBConnectionManager* manager, int UID, Account* account);

/**
 * 根据账号名查询账号
 * @param manager 数据库连接管理器
 * @param accountname 账号名称
 * @param account 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int Account_GetByName(DBConnectionManager* manager, const char* accountname, Account* account);

/**
 * 更新账号信息
 * @param manager 数据库连接管理器
 * @param account 账号信息
 * @return 成功返回0，失败返回-1
 */
int Account_Update(DBConnectionManager* manager, const Account* account);

/**
 * 删除账号
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @return 成功返回0，失败返回-1
 */
int Account_Delete(DBConnectionManager* manager, int UID);

// ========================================================================
// 账号专用操作函数
// ========================================================================

/**
 * 验证账号密码
 * @param manager 数据库连接管理器
 * @param accountname 账号名称
 * @param password 密码
 * @return 成功返回账号UID，失败返回-1
 */
int Account_Verify(DBConnectionManager* manager, const char* accountname, const char* password);

/**
 * 更新密码
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @param new_password 新密码
 * @return 成功返回0，失败返回-1
 */
int Account_UpdatePassword(DBConnectionManager* manager, int UID, const char* new_password);

/**
 * 更新VIP状态
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @param vip_status VIP状态
 * @return 成功返回0，失败返回-1
 */
int Account_UpdateVIP(DBConnectionManager* manager, int UID, const char* vip_status);

/**
 * 更新QQ号
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @param qq QQ号
 * @return 成功返回0，失败返回-1
 */
int Account_UpdateQQ(DBConnectionManager* manager, int UID, const char* qq);

/**
 * 根据QQ号查询账号
 * @param manager 数据库连接管理器
 * @param qq QQ号
 * @param accounts 输出参数，存储账号列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int Account_GetByQQ(DBConnectionManager* manager, const char* qq,
                    Account* accounts, int max_count);

/**
 * 检查账号名是否已存在
 * @param manager 数据库连接管理器
 * @param accountname 账号名称
 * @return 存在返回true，否则返回false
 */
bool Account_NameExists(DBConnectionManager* manager, const char* accountname);

/**
 * 根据VIP状态查询账号
 * @param manager 数据库连接管理器
 * @param vip_status VIP状态
 * @param accounts 输出参数，存储账号列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int Account_GetByVIP(DBConnectionManager* manager, const char* vip_status,
                     Account* accounts, int max_count);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查账号是否存在
 * @param manager 数据库连接管理器
 * @param UID 用户ID
 * @return 存在返回true，否则返回false
 */
bool Account_Exists(DBConnectionManager* manager, int UID);

/**
 * 获取账号总数
 * @param manager 数据库连接管理器
 * @return 成功返回账号总数，失败返回-1
 */
int Account_GetCount(DBConnectionManager* manager);

/**
 * 获取所有账号
 * @param manager 数据库连接管理器
 * @param accounts 输出参数，存储账号列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int Account_GetAll(DBConnectionManager* manager, Account* accounts, int max_count);

/**
 * 搜索账号（模糊匹配）
 * @param manager 数据库连接管理器
 * @param keyword 搜索关键词
 * @param accounts 输出参数，存储账号列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int Account_Search(DBConnectionManager* manager, const char* keyword,
                   Account* accounts, int max_count);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化Account结构体
 * @param account 账号信息
 */
void Account_Init(Account* account);

/**
 * 打印Account信息（用于调试，密码将被隐藏）
 * @param account 账号信息
 */
void Account_Print(const Account* account);

/**
 * 复制Account结构体
 * @param dest 目标
 * @param src 源
 */
void Account_Copy(Account* dest, const Account* src);

#endif // ACCOUNTS_ORM_H
