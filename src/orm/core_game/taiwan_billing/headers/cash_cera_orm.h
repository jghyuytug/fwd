#ifndef CASH_CERA_ORM_H
#define CASH_CERA_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// CashCera 结构体定义（根据实际表结构）
// ========================================================================

typedef struct {
    int account_id;                  // 账号ID（主键）
    long long cera_point;            // 点券数量
    char update_time[20];            // 更新时间 (YYYY-MM-DD HH:MM:SS)
} CashCera;

// ========================================================================
// CRUD 操作函数
// ========================================================================

/**
 * 创建新的点券记录
 * @param manager 数据库连接管理器
 * @param cash 点券信息
 * @return 成功返回0，失败返回-1
 */
int CashCera_Create(DBConnectionManager* manager, CashCera* cash);

/**
 * 根据账号ID查询点券信息
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param cash 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int CashCera_GetByAccountId(DBConnectionManager* manager, int account_id, CashCera* cash);

/**
 * 更新点券信息
 * @param manager 数据库连接管理器
 * @param cash 点券信息
 * @return 成功返回0，失败返回-1
 */
int CashCera_Update(DBConnectionManager* manager, const CashCera* cash);

/**
 * 删除点券记录
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @return 成功返回0，失败返回-1
 */
int CashCera_Delete(DBConnectionManager* manager, int account_id);

// ========================================================================
// 点券操作函数
// ========================================================================

/**
 * 增加点券（原子操作）
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param amount 增加数量
 * @return 成功返回0，失败返回-1
 */
int CashCera_AddCera(DBConnectionManager* manager, int account_id, long long amount);

/**
 * 扣除点券（原子操作，带余额检查）
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param amount 扣除数量
 * @return 成功返回0，失败返回-1，余额不足返回-2
 */
int CashCera_SubtractCera(DBConnectionManager* manager, int account_id, long long amount);

/**
 * 更新点券数量
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @param new_cera 新的点券数量
 * @return 成功返回0，失败返回-1
 */
int CashCera_UpdateCera(DBConnectionManager* manager, int account_id, long long new_cera);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查账号点券记录是否存在
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @return 存在返回true，否则返回false
 */
bool CashCera_Exists(DBConnectionManager* manager, int account_id);

/**
 * 获取账号的点券数量
 * @param manager 数据库连接管理器
 * @param account_id 账号ID
 * @return 成功返回点券数量，失败返回-1
 */
long long CashCera_GetCeraAmount(DBConnectionManager* manager, int account_id);

/**
 * 获取点券记录总数
 * @param manager 数据库连接管理器
 * @return 成功返回记录总数，失败返回-1
 */
int CashCera_GetCount(DBConnectionManager* manager);

/**
 * 获取所有点券记录
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储记录列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int CashCera_GetAll(DBConnectionManager* manager, CashCera* records, int max_count);

/**
 * 获取点券数量在指定范围内的记录
 * @param manager 数据库连接管理器
 * @param min_cera 最小点券数量
 * @param max_cera 最大点券数量
 * @param records 输出参数，存储记录列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的记录数量，失败返回-1
 */
int CashCera_GetByRange(DBConnectionManager* manager, long long min_cera,
                        long long max_cera, CashCera* records, int max_count);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化CashCera结构体
 * @param cash 点券信息
 */
void CashCera_Init(CashCera* cash);

/**
 * 打印CashCera信息（用于调试）
 * @param cash 点券信息
 */
void CashCera_Print(const CashCera* cash);

/**
 * 复制CashCera结构体
 * @param dest 目标
 * @param src 源
 */
void CashCera_Copy(CashCera* dest, const CashCera* src);

#endif // CASH_CERA_ORM_H
