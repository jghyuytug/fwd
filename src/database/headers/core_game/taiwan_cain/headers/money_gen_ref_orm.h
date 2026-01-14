#ifndef MONEY_GEN_REF_ORM_H
#define MONEY_GEN_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Money Gen Ref ORM - 金钱生成参考系统
 *
 * 表: money_gen_ref (taiwan_cain 数据库)
 * 描述: 游戏金钱掉落生成配置表
 * 唯一标识: (grade, bottom_grade) 复合键
 *
 * 字段说明:
 * - grade: 等级/品质 (INT)
 * - bottom_grade: 底层等级/最低品质 (INT)
 * - money: 金钱数量 (INT)
 * - random_value: 随机值 (INT, 用于掉落概率)
 *
 * 业务用途:
 * - 配置不同等级/品质的金钱掉落数量
 * - 支持随机掉落概率计算
 * - 用于怪物击杀、副本奖励等场景的金钱生成
 *
 * 注意事项:
 * - 表无主键，使用(grade, bottom_grade)作为复合唯一标识
 * - 配置表，主要用于查询操作
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个 (含按grade查询)
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #55/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 金钱生成参考记录结构 */
typedef struct {
    int grade;          /* 等级/品质 (复合主键1) */
    int bottom_grade;   /* 底层等级 (复合主键2) */
    int money;          /* 金钱数量 */
    int random_value;   /* 随机值 */
} MoneyGenRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新金钱生成配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int MoneyGenRef_Add(DBConnectionManager* manager, const MoneyGenRef* record);

/**
 * 获取指定等级的金钱生成配置
 * @param manager 数据库连接管理器
 * @param grade 等级/品质
 * @param bottom_grade 底层等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int MoneyGenRef_Get(DBConnectionManager* manager, int grade, int bottom_grade, MoneyGenRef* record);

/**
 * 更新金钱生成配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int MoneyGenRef_Update(DBConnectionManager* manager, const MoneyGenRef* record);

/**
 * 删除金钱生成配置
 * @param manager 数据库连接管理器
 * @param grade 等级/品质
 * @param bottom_grade 底层等级
 * @return 成功返回0，失败返回-1
 */
int MoneyGenRef_Delete(DBConnectionManager* manager, int grade, int bottom_grade);

/**
 * 检查金钱生成配置是否存在
 * @param manager 数据库连接管理器
 * @param grade 等级/品质
 * @param bottom_grade 底层等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int MoneyGenRef_Exists(DBConnectionManager* manager, int grade, int bottom_grade);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取金钱生成配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int MoneyGenRef_Count(DBConnectionManager* manager);

/**
 * 获取所有金钱生成配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MoneyGenRef_GetAll(DBConnectionManager* manager, MoneyGenRef* records, int max_records);

/**
 * 按等级/品质获取所有配置
 * @param manager 数据库连接管理器
 * @param grade 等级/品质
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MoneyGenRef_GetByGrade(DBConnectionManager* manager, int grade, MoneyGenRef* records, int max_records);

/**
 * 统计指定等级的配置数量
 * @param manager 数据库连接管理器
 * @param grade 等级/品质
 * @return >=0记录数，<0失败
 */
int MoneyGenRef_CountByGrade(DBConnectionManager* manager, int grade);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除金钱生成配置
 * @param manager 数据库连接管理器
 * @param records 要删除的记录数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int MoneyGenRef_BatchDelete(DBConnectionManager* manager, const MoneyGenRef* records, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印金钱生成配置信息（用于调试）
 * @param record 金钱生成配置记录指针
 */
void MoneyGenRef_PrintInfo(const MoneyGenRef* record);

#endif /* MONEY_GEN_REF_ORM_H */
