#ifndef AURA_AVATAR_OPTION_ORM_H
#define AURA_AVATAR_OPTION_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Aura Avatar Option ORM - 光环/化身选项系统
 *
 * 表: aura_avatar_option (taiwan_cain 数据库)
 * 描述: 角色光环和化身选项配置表
 * 主键: (charac_no, option_type) (复合主键)
 *
 * 字段说明:
 * - charac_no: 角色编号 (INT, PRIMARY KEY 1)
 * - option_type: 选项类型 (TINYINT, PRIMARY KEY 2)
 * - value_1: 选项数值 (INT)
 *
 * 业务用途:
 * - 存储角色的光环/化身配置选项
 * - 管理不同类型的选项数值
 * - 支持按角色查询所有选项
 *
 * API设计: 10个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #62/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 光环/化身选项记录结构 */
typedef struct {
    int charac_no;           /* 角色编号 (复合主键1) */
    char option_type;        /* 选项类型 (复合主键2) */
    int value_1;             /* 选项数值 */
} AuraAvatarOption;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新光环/化身选项（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int AuraAvatarOption_Add(DBConnectionManager* manager, const AuraAvatarOption* record);

/**
 * 获取指定角色的特定选项
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param option_type 选项类型
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int AuraAvatarOption_Get(DBConnectionManager* manager, int charac_no, char option_type, AuraAvatarOption* record);

/**
 * 更新光环/化身选项
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int AuraAvatarOption_Update(DBConnectionManager* manager, const AuraAvatarOption* record);

/**
 * 删除光环/化身选项
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param option_type 选项类型
 * @return 成功返回0，失败返回-1
 */
int AuraAvatarOption_Delete(DBConnectionManager* manager, int charac_no, char option_type);

/**
 * 检查光环/化身选项是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param option_type 选项类型
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int AuraAvatarOption_Exists(DBConnectionManager* manager, int charac_no, char option_type);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取选项记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int AuraAvatarOption_Count(DBConnectionManager* manager);

/**
 * 获取所有选项记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuraAvatarOption_GetAll(DBConnectionManager* manager, AuraAvatarOption* records, int max_records);

/**
 * 获取指定角色的所有选项
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuraAvatarOption_GetByCharacter(DBConnectionManager* manager, int charac_no, AuraAvatarOption* records, int max_records);

/**
 * 删除指定角色的所有选项
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回删除的记录数，失败返回-1
 */
int AuraAvatarOption_DeleteByCharacter(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印光环/化身选项信息（用于调试）
 * @param record 光环/化身选项记录指针
 */
void AuraAvatarOption_PrintInfo(const AuraAvatarOption* record);

#endif /* AURA_AVATAR_OPTION_ORM_H */
