#ifndef CS_TABLE2_ORM_H
#define CS_TABLE2_ORM_H

#include "db_connection_manager.h"

/**
 * 客服表2 ORM
 *
 * 功能：
 * - 记录账户与角色的关联关系
 * - 支持客服系统查询账户下的所有角色
 * - 管理账户-角色映射数据
 *
 * 特点：
 * - 无主键，使用组合键(account_id, charac_id)
 * - 简洁的二元关系表
 *
 * 字段说明：
 * - account_id: 账户ID (最大30字符)
 * - charac_id: 角色ID (最大30字符)
 */

/* 客服表2记录结构 */
typedef struct {
    char account_id[31];    /* 账户ID (30 + \0) */
    char charac_id[31];     /* 角色ID (30 + \0) */
} CsTable2;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加账户-角色关联记录（使用 REPLACE）
 *
 * @param manager 数据库连接管理器
 * @param record 关联记录
 * @return 0 成功, <0 失败
 */
int CsTable2_Add(DBConnectionManager* manager, const CsTable2* record);

/**
 * 获取账户-角色关联记录
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @param charac_id 角色ID
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int CsTable2_Get(DBConnectionManager* manager, const char* account_id, const char* charac_id, CsTable2* record);

/**
 * 删除账户-角色关联记录
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @param charac_id 角色ID
 * @return 0 成功, <0 失败
 */
int CsTable2_Delete(DBConnectionManager* manager, const char* account_id, const char* charac_id);

/**
 * 检查账户-角色关联记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @param charac_id 角色ID
 * @return 1 存在, 0 不存在, <0 错误
 */
int CsTable2_Exists(DBConnectionManager* manager, const char* account_id, const char* charac_id);

/* ==================== 专项操作 ==================== */

/**
 * 查询账户下的所有角色
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @param records 输出参数，接收记录数组
 * @param max_count 最大记录数
 * @return >=0 实际返回的记录数, <0 失败
 */
int CsTable2_GetByAccountId(DBConnectionManager* manager, const char* account_id, CsTable2* records, int max_count);

/**
 * 查询角色关联的所有账户
 *
 * @param manager 数据库连接管理器
 * @param charac_id 角色ID
 * @param records 输出参数，接收记录数组
 * @param max_count 最大记录数
 * @return >=0 实际返回的记录数, <0 失败
 */
int CsTable2_GetByCharacId(DBConnectionManager* manager, const char* charac_id, CsTable2* records, int max_count);

/**
 * 删除账户的所有角色关联
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @return >=0 删除的记录数, <0 失败
 */
int CsTable2_DeleteByAccountId(DBConnectionManager* manager, const char* account_id);

/**
 * 删除角色的所有账户关联
 *
 * @param manager 数据库连接管理器
 * @param charac_id 角色ID
 * @return >=0 删除的记录数, <0 失败
 */
int CsTable2_DeleteByCharacId(DBConnectionManager* manager, const char* charac_id);

/* ==================== 查询操作 ==================== */

/**
 * 统计总记录数
 *
 * @param manager 数据库连接管理器
 * @return >=0 总记录数, <0 失败
 */
int CsTable2_CountAll(DBConnectionManager* manager);

/**
 * 统计账户关联的角色数
 *
 * @param manager 数据库连接管理器
 * @param account_id 账户ID
 * @return >=0 角色数, <0 失败
 */
int CsTable2_CountByAccountId(DBConnectionManager* manager, const char* account_id);

/**
 * 统计角色关联的账户数
 *
 * @param manager 数据库连接管理器
 * @param charac_id 角色ID
 * @return >=0 账户数, <0 失败
 */
int CsTable2_CountByCharacId(DBConnectionManager* manager, const char* charac_id);

/**
 * 获取所有记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param offset 偏移量
 * @param limit 限制数量
 * @param records 输出参数，接收记录数组
 * @return >=0 实际返回的记录数, <0 失败
 */
int CsTable2_GetAll(DBConnectionManager* manager, int offset, int limit, CsTable2* records);

/* ==================== 工具函数 ==================== */

/**
 * 打印客服表2记录（调试用）
 *
 * @param record 关联记录
 */
void CsTable2_PrintInfo(const CsTable2* record);

#endif /* CS_TABLE2_ORM_H */
