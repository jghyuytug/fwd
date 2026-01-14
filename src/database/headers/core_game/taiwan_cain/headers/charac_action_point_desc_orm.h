#ifndef CHARAC_ACTION_POINT_DESC_ORM_H
#define CHARAC_ACTION_POINT_DESC_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Action Point Desc ORM - 角色行动点描述系统
 *
 * 表: charac_action_point_desc (taiwan_cain 数据库)
 * 描述: 角色行动点描述配置表
 * 唯一标识: (action_group_index, action_index) 复合主键
 *
 * 字段说明:
 * - action_group_index: 行动组索引 (INT, 复合主键1)
 * - action_index: 行动索引 (INT, 复合主键2)
 * - action_group_name: 行动组名称 (VARCHAR(128))
 *
 * 业务用途:
 * - 存储行动点系统的分组和动作描述
 * - 用于行动点配置管理
 * - 支持按组查询所有动作
 *
 * 注意事项:
 * - 复合主键设计，需要两个字段唯一标识记录
 * - 配置表，主要用于查询操作
 * - 与 charac_action_point 表关联
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #53/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 行动点描述记录结构 */
typedef struct {
    int action_group_index;       /* 行动组索引 (复合主键1) */
    int action_index;             /* 行动索引 (复合主键2) */
    char action_group_name[128];  /* 行动组名称 */
} CharacActionPointDesc;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新行动点描述（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacActionPointDesc_Add(DBConnectionManager* manager, const CharacActionPointDesc* record);

/**
 * 获取指定行动点描述
 * @param manager 数据库连接管理器
 * @param action_group_index 行动组索引
 * @param action_index 行动索引
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacActionPointDesc_Get(DBConnectionManager* manager, int action_group_index, int action_index, CharacActionPointDesc* record);

/**
 * 更新行动点描述
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int CharacActionPointDesc_Update(DBConnectionManager* manager, const CharacActionPointDesc* record);

/**
 * 删除行动点描述
 * @param manager 数据库连接管理器
 * @param action_group_index 行动组索引
 * @param action_index 行动索引
 * @return 成功返回0，失败返回-1
 */
int CharacActionPointDesc_Delete(DBConnectionManager* manager, int action_group_index, int action_index);

/**
 * 检查行动点描述是否存在
 * @param manager 数据库连接管理器
 * @param action_group_index 行动组索引
 * @param action_index 行动索引
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacActionPointDesc_Exists(DBConnectionManager* manager, int action_group_index, int action_index);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取行动点描述总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int CharacActionPointDesc_Count(DBConnectionManager* manager);

/**
 * 获取所有行动点描述
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacActionPointDesc_GetAll(DBConnectionManager* manager, CharacActionPointDesc* records, int max_records);

/**
 * 按行动组索引获取所有行动描述
 * @param manager 数据库连接管理器
 * @param action_group_index 行动组索引
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacActionPointDesc_GetByGroupIndex(DBConnectionManager* manager, int action_group_index, CharacActionPointDesc* records, int max_records);

/**
 * 获取指定行动组的动作数量
 * @param manager 数据库连接管理器
 * @param action_group_index 行动组索引
 * @return >=0记录数，<0失败
 */
int CharacActionPointDesc_CountByGroup(DBConnectionManager* manager, int action_group_index);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除行动点描述（按复合主键数组）
 * @param manager 数据库连接管理器
 * @param keys 复合主键数组（每个元素包含 group_index 和 action_index）
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int CharacActionPointDesc_BatchDelete(DBConnectionManager* manager, const CharacActionPointDesc* keys, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印行动点描述信息（用于调试）
 * @param record 行动点描述记录指针
 */
void CharacActionPointDesc_PrintInfo(const CharacActionPointDesc* record);

#endif /* CHARAC_ACTION_POINT_DESC_ORM_H */
