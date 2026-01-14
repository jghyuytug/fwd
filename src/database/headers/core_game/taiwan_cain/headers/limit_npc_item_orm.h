#ifndef LIMIT_NPC_ITEM_ORM_H
#define LIMIT_NPC_ITEM_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Limit NPC Item ORM - NPC限购物品系统
 *
 * 表: limit_npc_item (taiwan_cain 数据库)
 * 描述: NPC商店限购物品管理表
 * 主键: item_index (单一主键)
 *
 * 字段说明:
 * - item_index: 物品索引 (INT UNSIGNED, PRIMARY KEY)
 * - max_count: 最大可售数量 (INT UNSIGNED)
 * - sell_count: 当前已售数量 (INT UNSIGNED)
 *
 * 业务用途:
 * - 管理NPC商店限购物品的库存
 * - 跟踪特殊物品的销售数量
 * - 控制稀有物品的供应量
 *
 * API设计: 10个函数
 * - CRUD操作: 5个
 * - 查询操作: 3个
 * - 业务操作: 1个 (增加销售数)
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #60/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* NPC限购物品记录结构 */
typedef struct {
    unsigned int item_index;      /* 物品索引 (主键) */
    unsigned int max_count;       /* 最大可售数量 */
    unsigned int sell_count;      /* 当前已售数量 */
} LimitNpcItem;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新NPC限购物品（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int LimitNpcItem_Add(DBConnectionManager* manager, const LimitNpcItem* record);

/**
 * 获取指定物品的限购信息
 * @param manager 数据库连接管理器
 * @param item_index 物品索引
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int LimitNpcItem_Get(DBConnectionManager* manager, unsigned int item_index, LimitNpcItem* record);

/**
 * 更新NPC限购物品信息
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int LimitNpcItem_Update(DBConnectionManager* manager, const LimitNpcItem* record);

/**
 * 删除NPC限购物品
 * @param manager 数据库连接管理器
 * @param item_index 物品索引
 * @return 成功返回0，失败返回-1
 */
int LimitNpcItem_Delete(DBConnectionManager* manager, unsigned int item_index);

/**
 * 检查NPC限购物品是否存在
 * @param manager 数据库连接管理器
 * @param item_index 物品索引
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int LimitNpcItem_Exists(DBConnectionManager* manager, unsigned int item_index);

/*******************************************************************************
 * 查询操作 (3个函数)
 ******************************************************************************/

/**
 * 获取限购物品记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int LimitNpcItem_Count(DBConnectionManager* manager);

/**
 * 获取所有限购物品记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int LimitNpcItem_GetAll(DBConnectionManager* manager, LimitNpcItem* records, int max_records);

/**
 * 获取所有已售罄的物品
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int LimitNpcItem_GetSoldOut(DBConnectionManager* manager, LimitNpcItem* records, int max_records);

/*******************************************************************************
 * 业务操作 (1个函数)
 ******************************************************************************/

/**
 * 增加物品销售数量（原子操作）
 * @param manager 数据库连接管理器
 * @param item_index 物品索引
 * @param count 增加的数量
 * @return 成功返回0，失败返回-1
 */
int LimitNpcItem_IncreaseSellCount(DBConnectionManager* manager, unsigned int item_index, unsigned int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印NPC限购物品信息（用于调试）
 * @param record NPC限购物品记录指针
 */
void LimitNpcItem_PrintInfo(const LimitNpcItem* record);

#endif /* LIMIT_NPC_ITEM_ORM_H */
