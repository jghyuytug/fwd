#ifndef ITEM_SELECT_STD_ORM_H
#define ITEM_SELECT_STD_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Item Select Std ORM - 物品选择标准系统
 *
 * 表: item_select_std (taiwan_cain 数据库)
 * 描述: 物品选择标准配置表，用于物品掉落权重计算
 * 主键: item_grade (单一主键)
 *
 * 字段说明:
 * - item_grade: 物品等级/品质 (INT, PRIMARY KEY)
 * - top: 上限值 (INT)
 * - bottom: 下限值 (INT)
 * - weight: 权重值 (INT)
 *
 * 业务用途:
 * - 配置不同等级物品的掉落权重
 * - 定义物品选择的上下限范围
 * - 用于掉落系统的概率计算
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 3个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #61/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 物品选择标准记录结构 */
typedef struct {
    int item_grade;      /* 物品等级/品质 (主键) */
    int top;             /* 上限值 */
    int bottom;          /* 下限值 */
    int weight;          /* 权重值 */
} ItemSelectStd;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新物品选择标准（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int ItemSelectStd_Add(DBConnectionManager* manager, const ItemSelectStd* record);

/**
 * 获取指定等级的物品选择标准
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int ItemSelectStd_Get(DBConnectionManager* manager, int item_grade, ItemSelectStd* record);

/**
 * 更新物品选择标准
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int ItemSelectStd_Update(DBConnectionManager* manager, const ItemSelectStd* record);

/**
 * 删除物品选择标准
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @return 成功返回0，失败返回-1
 */
int ItemSelectStd_Delete(DBConnectionManager* manager, int item_grade);

/**
 * 检查物品选择标准是否存在
 * @param manager 数据库连接管理器
 * @param item_grade 物品等级/品质
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int ItemSelectStd_Exists(DBConnectionManager* manager, int item_grade);

/*******************************************************************************
 * 查询操作 (3个函数)
 ******************************************************************************/

/**
 * 获取选择标准记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int ItemSelectStd_Count(DBConnectionManager* manager);

/**
 * 获取所有选择标准记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ItemSelectStd_GetAll(DBConnectionManager* manager, ItemSelectStd* records, int max_records);

/**
 * 获取权重总和（用于概率计算）
 * @param manager 数据库连接管理器
 * @return >=0权重总和，<0失败
 */
int ItemSelectStd_GetTotalWeight(DBConnectionManager* manager);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印物品选择标准信息（用于调试）
 * @param record 物品选择标准记录指针
 */
void ItemSelectStd_PrintInfo(const ItemSelectStd* record);

#endif /* ITEM_SELECT_STD_ORM_H */
