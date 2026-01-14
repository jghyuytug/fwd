#ifndef ITEM_GEN_REF_ORM_H
#define ITEM_GEN_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Item Gen Ref ORM - 物品生成参考系统
 *
 * 表: item_gen_ref (taiwan_cain 数据库)
 * 描述: 物品掉落生成概率配置表
 * 唯一标识: (item_grade, rate_type) 复合键
 *
 * 字段说明:
 * - item_grade: 物品等级/品质 (TINYINT)
 * - rate_type: 掉落类型 (TINYINT)
 * - money_rate: 金钱掉落概率 (SMALLINT)
 * - item_rate: 物品掉落概率 (SMALLINT)
 * - free_rate: 免费掉落概率 (SMALLINT)
 *
 * 业务用途:
 * - 配置不同等级物品的掉落概率
 * - 支持多种掉落类型（金钱、物品、免费）
 * - 用于怪物击杀、副本奖励等场景的物品生成
 *
 * 注意事项:
 * - 表无主键，使用(item_grade, rate_type)作为复合唯一标识
 * - 配置表，主要用于查询操作
 * - 概率值使用千分比或万分比存储
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个 (含按item_grade查询)
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #56/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 物品生成参考记录结构 */
typedef struct {
    signed char item_grade;    /* 物品等级/品质 (复合主键1) */
    signed char rate_type;     /* 掉落类型 (复合主键2) */
    short money_rate;          /* 金钱掉落概率 */
    short item_rate;           /* 物品掉落概率 */
    short free_rate;           /* 免费掉落概率 */
} ItemGenRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新物品生成配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int ItemGenRef_Add(DBConnectionManager* manager, const ItemGenRef* record);

/**
 * 获取指定物品等级和掉落类型的配置
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @param rate_type 掉落类型
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int ItemGenRef_Get(DBConnectionManager* manager, signed char item_grade, signed char rate_type, ItemGenRef* record);

/**
 * 更新物品生成配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int ItemGenRef_Update(DBConnectionManager* manager, const ItemGenRef* record);

/**
 * 删除物品生成配置
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @param rate_type 掉落类型
 * @return 成功返回0，失败返回-1
 */
int ItemGenRef_Delete(DBConnectionManager* manager, signed char item_grade, signed char rate_type);

/**
 * 检查物品生成配置是否存在
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @param rate_type 掉落类型
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int ItemGenRef_Exists(DBConnectionManager* manager, signed char item_grade, signed char rate_type);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取物品生成配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int ItemGenRef_Count(DBConnectionManager* manager);

/**
 * 获取所有物品生成配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ItemGenRef_GetAll(DBConnectionManager* manager, ItemGenRef* records, int max_records);

/**
 * 按物品等级获取所有配置
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ItemGenRef_GetByGrade(DBConnectionManager* manager, signed char item_grade, ItemGenRef* records, int max_records);

/**
 * 统计指定物品等级的配置数量
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @return >=0记录数，<0失败
 */
int ItemGenRef_CountByGrade(DBConnectionManager* manager, signed char item_grade);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除物品生成配置
 * @param manager 数据库连接管理器
 * @param records 要删除的记录数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int ItemGenRef_BatchDelete(DBConnectionManager* manager, const ItemGenRef* records, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印物品生成配置信息（用于调试）
 * @param record 物品生成配置记录指针
 */
void ItemGenRef_PrintInfo(const ItemGenRef* record);

#endif /* ITEM_GEN_REF_ORM_H */
