#ifndef LOGIN_LOG_ORM_H
#define LOGIN_LOG_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// LoginLog 结构体定义
// ========================================================================

typedef struct {
    long long seq;                   // 序列号（主键，自增）
    int account_id;                  // 账号ID
    char login_time[20];             // 登录时间 (YYYY-MM-DD HH:MM:SS)
    char logout_time[20];            // 登出时间 (YYYY-MM-DD HH:MM:SS)
    char login_ip[51];               // 登录IP地址
    int server_id;                   // 服务器ID
} LoginLog;

// ========================================================================
// CRUD 操作函数
// ========================================================================

/**
 * 创建新的登录日志
 * @param manager 数据库连接管理器
 * @param log 登录日志信息
 * @return 成功返回0，失败返回-1
 */
int LoginLog_Create(DBConnectionManager* manager, LoginLog* log);

/**
 * 根据序列号查询登录日志
 * @param manager 数据库连接管理器
 * @param seq 序列号
 * @param log 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int LoginLog_GetBySeq(DBConnectionManager* manager, long long seq, LoginLog* log);

/**
 * 更新登录日志
 * @param manager 数据库连接管理器
 * @param log 登录日志信息
 * @return 成功返回0，失败返回-1
 */
int LoginLog_Update(DBConnectionManager* manager, const LoginLog* log);

/**
 * 删除登录日志
 * @param manager 数据库连接管理器
 * @param seq 序列号
 * @return 成功返回0，失败返回-1
 */
int LoginLog_Delete(DBConnectionManager* manager, long long seq);

// ========================================================================
// 登录日志专用操作函数
// ========================================================================

/**
 * 更新登出时间
 * @param manager 数据库连接管理器
 * @param seq 序列号
 * @param logout_time 登出时间
 * @return 成功返回0，失败返回-1
 */
int LoginLog_UpdateLogoutTime(DBConnectionManager* manager, long long seq, const char* logout_time);

/**
 * 根据账号ID获取登录日志
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetByAccountId(DBConnectionManager* manager, int account_id,
                            LoginLog* logs, int max_count);

/**
 * 获取某账号最近的登录记录
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param limit 返回数量限制
 * @param logs 输出参数，存储日志列表
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetRecentByAccountId(DBConnectionManager* manager, int account_id,
                                  int limit, LoginLog* logs);

/**
 * 根据IP地址查询登录日志
 * @param manager 数据库连接管理器
 * @param login_ip IP地址
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetByIP(DBConnectionManager* manager, const char* login_ip,
                     LoginLog* logs, int max_count);

/**
 * 按时间范围查询登录日志
 * @param manager 数据库连接管理器
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                            const char* end_time, LoginLog* logs, int max_count);

/**
 * 获取当前在线的登录记录（未登出的）
 * @param manager 数据库连接管理器
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetActiveLogins(DBConnectionManager* manager, LoginLog* logs, int max_count);

/**
 * 根据服务器ID查询登录日志
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetByServerId(DBConnectionManager* manager, int server_id,
                           LoginLog* logs, int max_count);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查登录日志记录是否存在
 * @param manager 数据库连接管理器
 * @param seq 序列号
 * @return 存在返回true，否则返回false
 */
bool LoginLog_Exists(DBConnectionManager* manager, long long seq);

/**
 * 获取登录日志记录总数
 * @param manager 数据库连接管理器
 * @return 成功返回记录总数，失败返回-1
 */
int LoginLog_GetCount(DBConnectionManager* manager);

/**
 * 获取某账号的登录次数
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @return 成功返回登录次数，失败返回-1
 */
int LoginLog_GetCountByAccountId(DBConnectionManager* manager, int account_id);

/**
 * 获取所有登录日志
 * @param manager 数据库连接管理器
 * @param logs 输出参数，存储日志列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int LoginLog_GetAll(DBConnectionManager* manager, LoginLog* logs, int max_count);

/**
 * 删除指定时间之前的日志记录（清理旧日志）
 * @param manager 数据库连接管理器
 * @param before_time 删除此时间之前的记录
 * @return 成功返回删除的记录数，失败返回-1
 */
int LoginLog_DeleteBefore(DBConnectionManager* manager, const char* before_time);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化LoginLog结构体
 * @param log 登录日志信息
 */
void LoginLog_Init(LoginLog* log);

/**
 * 打印LoginLog信息（用于调试）
 * @param log 登录日志信息
 */
void LoginLog_Print(const LoginLog* log);

/**
 * 复制LoginLog结构体
 * @param dest 目标
 * @param src 源
 */
void LoginLog_Copy(LoginLog* dest, const LoginLog* src);

#endif // LOGIN_LOG_ORM_H
