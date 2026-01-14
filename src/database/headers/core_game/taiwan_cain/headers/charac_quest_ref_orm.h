#ifndef CHARAC_QUEST_REF_ORM_H
#define CHARAC_QUEST_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Quest Ref ORM - 角色任务映射系统
 *
 * 表: charac_quest_ref (taiwan_cain 数据库)
 * 描述: 角色任务ID映射参考表
 * 主键: origin_idx (原始任务ID)
 * 唯一索引: mapped_idx (映射任务ID)
 *
 * 字段说明:
 * - origin_idx: 原始任务ID (INT, 主键)
 * - mapped_idx: 映射任务ID (INT, 唯一索引)
 *
 * 业务用途:
 * - 存储任务ID映射关系
 * - 支持任务版本转换
 * - 用于任务系统重构时的兼容性处理
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #51/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 角色任务映射记录结构 */
typedef struct {
    int origin_idx;    /* 原始任务ID (主键) */
    int mapped_idx;    /* 映射任务ID (唯一索引) */
} CharacQuestRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新任务映射（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacQuestRef_Add(DBConnectionManager* manager, const CharacQuestRef* record);

/**
 * 根据原始任务ID获取映射
 * @param manager 数据库连接管理器
 * @param origin_idx 原始任务ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacQuestRef_Get(DBConnectionManager* manager, int origin_idx, CharacQuestRef* record);

/**
 * 更新任务映射
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int CharacQuestRef_Update(DBConnectionManager* manager, const CharacQuestRef* record);

/**
 * 删除任务映射
 * @param manager 数据库连接管理器
 * @param origin_idx 原始任务ID
 * @return 成功返回0，失败返回-1
 */
int CharacQuestRef_Delete(DBConnectionManager* manager, int origin_idx);

/**
 * 检查任务映射是否存在
 * @param manager 数据库连接管理器
 * @param origin_idx 原始任务ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacQuestRef_Exists(DBConnectionManager* manager, int origin_idx);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取任务映射总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int CharacQuestRef_Count(DBConnectionManager* manager);

/**
 * 获取所有任务映射
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacQuestRef_GetAll(DBConnectionManager* manager, CharacQuestRef* records, int max_records);

/**
 * 根据映射任务ID反向查找原始任务ID
 * @param manager 数据库连接管理器
 * @param mapped_idx 映射任务ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacQuestRef_GetByMappedIdx(DBConnectionManager* manager, int mapped_idx, CharacQuestRef* record);

/**
 * 检查映射任务ID是否存在
 * @param manager 数据库连接管理器
 * @param mapped_idx 映射任务ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacQuestRef_MappedIdxExists(DBConnectionManager* manager, int mapped_idx);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除任务映射
 * @param manager 数据库连接管理器
 * @param origin_idxs 原始任务ID数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int CharacQuestRef_BatchDelete(DBConnectionManager* manager, const int* origin_idxs, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印任务映射信息（用于调试）
 * @param record 任务映射记录指针
 */
void CharacQuestRef_PrintInfo(const CharacQuestRef* record);

#endif /* CHARAC_QUEST_REF_ORM_H */
