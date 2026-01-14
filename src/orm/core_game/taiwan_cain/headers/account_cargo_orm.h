/**
 * DNF游戏服务器 - AccountCargo表ORM层
 *
 * 功能: 账号仓库数据管理
 * 数据库: test_taiwan_cain
 * 表: account_cargo
 */

#ifndef ACCOUNT_CARGO_ORM_H
#define ACCOUNT_CARGO_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// 账号仓库结构定义
// ========================================================================

/**
 * 账号仓库信息结构
 *
 * account_cargo表用于存储账号共享仓库数据
 * - 一个账号只有一条记录
 * - cargo_data是BLOB类型，存储序列化的仓库物品数据
 */
typedef struct {
    int m_id;                       // 账号ID (主键, 外键关联accounts表)
    unsigned char* cargo_data;      // 仓库数据 (BLOB, 动态分配)
    int cargo_data_len;             // 仓库数据长度
    char update_time[20];           // 更新时间 (YYYY-MM-DD HH:MM:SS)
} AccountCargo;

// ========================================================================
// CRUD操作函数
// ========================================================================

/**
 * 创建账号仓库记录
 *
 * @param manager 数据库连接管理器
 * @param cargo 账号仓库信息结构
 * @return 0=成功, -1=失败
 */
int AccountCargo_Create(DBConnectionManager* manager, AccountCargo* cargo);

/**
 * 根据账号ID获取仓库数据
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @param cargo 输出参数，仓库信息
 * @return 0=成功, -1=失败（不存在或错误）
 *
 * 注意: cargo_data会被动态分配，使用后需要调用AccountCargo_Free()释放
 */
int AccountCargo_GetByID(DBConnectionManager* manager, int m_id, AccountCargo* cargo);

/**
 * 更新账号仓库数据（完整更新）
 *
 * @param manager 数据库连接管理器
 * @param cargo 账号仓库信息结构
 * @return 0=成功, -1=失败
 */
int AccountCargo_Update(DBConnectionManager* manager, AccountCargo* cargo);

/**
 * 删除账号仓库记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @return 0=成功, -1=失败
 */
int AccountCargo_Delete(DBConnectionManager* manager, int m_id);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查账号是否有仓库记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @return true=存在, false=不存在
 */
bool AccountCargo_Exists(DBConnectionManager* manager, int m_id);

/**
 * 获取仓库数据大小（不加载实际数据）
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @return 数据大小(字节), -1=不存在或失败
 */
int AccountCargo_GetDataSize(DBConnectionManager* manager, int m_id);

/**
 * 获取所有有仓库数据的账号列表
 *
 * @param manager 数据库连接管理器
 * @param m_ids 输出数组（账号ID列表）
 * @param max_count 数组最大容量
 * @return 实际账号数量, -1=失败
 */
int AccountCargo_GetAllAccountIDs(DBConnectionManager* manager, int* m_ids, int max_count);

/**
 * 获取仓库记录总数
 *
 * @param manager 数据库连接管理器
 * @return 仓库记录总数, -1=失败
 */
int AccountCargo_GetCount(DBConnectionManager* manager);

/**
 * 批量清理过期仓库数据
 * 删除超过指定天数未更新的仓库记录
 *
 * @param manager 数据库连接管理器
 * @param days 天数阈值
 * @return 删除的记录数, -1=失败
 */
int AccountCargo_CleanupOldData(DBConnectionManager* manager, int days);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化账号仓库结构（设置默认值）
 *
 * @param cargo 账号仓库结构指针
 */
void AccountCargo_Init(AccountCargo* cargo);

/**
 * 释放账号仓库结构中的动态内存
 *
 * @param cargo 账号仓库结构指针
 */
void AccountCargo_Free(AccountCargo* cargo);

/**
 * 打印账号仓库信息（用于调试，不显示完整BLOB数据）
 *
 * @param cargo 账号仓库结构指针
 */
void AccountCargo_Print(const AccountCargo* cargo);

/**
 * 创建空的仓库数据
 * 分配并初始化指定大小的仓库数据缓冲区
 *
 * @param cargo 账号仓库结构指针
 * @param size 数据大小
 * @return 0=成功, -1=失败
 */
int AccountCargo_AllocateData(AccountCargo* cargo, int size);

/**
 * 复制仓库数据
 *
 * @param dest 目标仓库结构
 * @param src 源仓库结构
 * @return 0=成功, -1=失败
 */
int AccountCargo_Copy(AccountCargo* dest, const AccountCargo* src);

#endif // ACCOUNT_CARGO_ORM_H
