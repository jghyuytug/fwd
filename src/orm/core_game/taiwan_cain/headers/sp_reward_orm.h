#ifndef SP_REWARD_ORM_H
#define SP_REWARD_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * SP Reward ORM - SP奖励系统
 *
 * 表: sp_reward (taiwan_cain 数据库)
 * 描述: SP技能点奖励配置表
 * 唯一标识: grade (等级)
 *
 * 字段说明:
 * - grade: 等级 (INT, 唯一标识)
 * - sp: SP技能点数量 (INT)
 *
 * 业务用途:
 * - 存储不同等级的SP奖励配置
 * - 只读配置表，用于角色升级时发放SP
 * - 支持等级-SP映射关系
 *
 * 注意事项:
 * - 表无主键，使用grade作为唯一标识
 * - 配置表，主要用于查询操作
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #49/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* SP奖励记录结构 */
typedef struct {
    int grade;    /* 等级 (唯一标识) */
    int sp;       /* SP技能点数量 */
} SpReward;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新SP奖励配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int SpReward_Add(DBConnectionManager* manager, const SpReward* record);

/**
 * 获取指定等级的SP奖励
 * @param manager 数据库连接管理器
 * @param grade 等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int SpReward_Get(DBConnectionManager* manager, int grade, SpReward* record);

/**
 * 更新SP奖励配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int SpReward_Update(DBConnectionManager* manager, const SpReward* record);

/**
 * 删除SP奖励配置
 * @param manager 数据库连接管理器
 * @param grade 等级
 * @return 成功返回0，失败返回-1
 */
int SpReward_Delete(DBConnectionManager* manager, int grade);

/**
 * 检查SP奖励配置是否存在
 * @param manager 数据库连接管理器
 * @param grade 等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int SpReward_Exists(DBConnectionManager* manager, int grade);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取SP奖励配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int SpReward_Count(DBConnectionManager* manager);

/**
 * 获取所有SP奖励配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int SpReward_GetAll(DBConnectionManager* manager, SpReward* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除SP奖励配置
 * @param manager 数据库连接管理器
 * @param grades 等级数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int SpReward_BatchDelete(DBConnectionManager* manager, const int* grades, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印SP奖励信息（用于调试）
 * @param record SP奖励记录指针
 */
void SpReward_PrintInfo(const SpReward* record);

#endif /* SP_REWARD_ORM_H */
