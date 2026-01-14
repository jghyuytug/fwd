#ifndef MONSTER_REWARD_REF_ORM_H
#define MONSTER_REWARD_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Monster Reward Ref ORM - 怪物奖励参考系统
 *
 * 表: monster_reward_ref (taiwan_cain 数据库)
 * 描述: 怪物等级经验奖励配置表
 * 唯一标识: level (等级)
 *
 * 字段说明:
 * - level: 怪物等级 (SMALLINT, 唯一标识)
 * - exp: 经验奖励 (INT)
 *
 * 业务用途:
 * - 存储怪物等级与经验奖励的映射关系
 * - 用于击杀怪物后的经验计算
 * - 支持等级查询和经验配置管理
 *
 * 注意事项:
 * - 表无主键，使用level作为唯一标识
 * - 配置表，主要用于查询操作
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #54/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 怪物奖励参考记录结构 */
typedef struct {
    short level;    /* 怪物等级 (唯一标识) */
    int exp;        /* 经验奖励 */
} MonsterRewardRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新怪物奖励配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int MonsterRewardRef_Add(DBConnectionManager* manager, const MonsterRewardRef* record);

/**
 * 获取指定等级的怪物奖励配置
 * @param manager 数据库连接管理器
 * @param level 怪物等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int MonsterRewardRef_Get(DBConnectionManager* manager, short level, MonsterRewardRef* record);

/**
 * 更新怪物奖励配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int MonsterRewardRef_Update(DBConnectionManager* manager, const MonsterRewardRef* record);

/**
 * 删除怪物奖励配置
 * @param manager 数据库连接管理器
 * @param level 怪物等级
 * @return 成功返回0，失败返回-1
 */
int MonsterRewardRef_Delete(DBConnectionManager* manager, short level);

/**
 * 检查怪物奖励配置是否存在
 * @param manager 数据库连接管理器
 * @param level 怪物等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int MonsterRewardRef_Exists(DBConnectionManager* manager, short level);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取怪物奖励配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int MonsterRewardRef_Count(DBConnectionManager* manager);

/**
 * 获取所有怪物奖励配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MonsterRewardRef_GetAll(DBConnectionManager* manager, MonsterRewardRef* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除怪物奖励配置
 * @param manager 数据库连接管理器
 * @param levels 等级数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int MonsterRewardRef_BatchDelete(DBConnectionManager* manager, const short* levels, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印怪物奖励信息（用于调试）
 * @param record 怪物奖励记录指针
 */
void MonsterRewardRef_PrintInfo(const MonsterRewardRef* record);

#endif /* MONSTER_REWARD_REF_ORM_H */
