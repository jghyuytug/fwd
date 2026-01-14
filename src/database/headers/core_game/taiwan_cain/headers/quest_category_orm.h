#ifndef QUEST_CATEGORY_ORM_H
#define QUEST_CATEGORY_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Quest Category ORM - 任务分类系统
 *
 * 表: quest_category (taiwan_cain 数据库)
 * 描述: 任务分类配置表
 * 主键: quest_idx (任务索引)
 *
 * 字段说明:
 * - quest_idx: 任务索引 (INT, 主键)
 * - quest_name: 任务分类名称 (VARCHAR(30))
 *
 * 业务用途:
 * - 存储任务分类配置数据
 * - 只读配置表，主要用于查询
 * - 支持任务系统显示分类名称
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #48/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_QUEST_NAME_LEN 30

/* 任务分类记录结构 */
typedef struct {
    int quest_idx;                    /* 任务索引 (主键) */
    char quest_name[MAX_QUEST_NAME_LEN + 1];  /* 任务分类名称 */
} QuestCategory;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新任务分类（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int QuestCategory_Add(DBConnectionManager* manager, const QuestCategory* record);

/**
 * 获取任务分类
 * @param manager 数据库连接管理器
 * @param quest_idx 任务索引
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int QuestCategory_Get(DBConnectionManager* manager, int quest_idx, QuestCategory* record);

/**
 * 更新任务分类名称
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int QuestCategory_Update(DBConnectionManager* manager, const QuestCategory* record);

/**
 * 删除任务分类
 * @param manager 数据库连接管理器
 * @param quest_idx 任务索引
 * @return 成功返回0，失败返回-1
 */
int QuestCategory_Delete(DBConnectionManager* manager, int quest_idx);

/**
 * 检查任务分类是否存在
 * @param manager 数据库连接管理器
 * @param quest_idx 任务索引
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int QuestCategory_Exists(DBConnectionManager* manager, int quest_idx);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取任务分类总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int QuestCategory_Count(DBConnectionManager* manager);

/**
 * 获取所有任务分类
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int QuestCategory_GetAll(DBConnectionManager* manager, QuestCategory* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除任务分类
 * @param manager 数据库连接管理器
 * @param quest_idxs 任务索引数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int QuestCategory_BatchDelete(DBConnectionManager* manager, const int* quest_idxs, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印任务分类信息（用于调试）
 * @param record 任务分类记录指针
 */
void QuestCategory_PrintInfo(const QuestCategory* record);

#endif /* QUEST_CATEGORY_ORM_H */
