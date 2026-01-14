#ifndef AUTO_MARKET_CONDITION_CTRL_ORM_H
#define AUTO_MARKET_CONDITION_CTRL_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Auto Market Condition Control ORM - 自动市场条件控制系统
 *
 * 表: auto_market_condition_ctrl (taiwan_cain 数据库)
 * 描述: 游戏经济系统金币供应全局配置表（单例模式）
 * 主键: 无（单行表，只存储1条记录）
 *
 * 字段说明:
 * - optimum_gold_supply: 最优金币供应量 (BIGINT UNSIGNED, 目标金币总量)
 * - over_gold: 超额金币量 (BIGINT UNSIGNED, 超出目标的金币量)
 *
 * 业务用途:
 * - 控制游戏内金币总量平衡
 * - 防止通货膨胀/紧缩
 * - 自动市场调节参考指标
 * - 全服唯一配置，不分区不分服
 *
 * 特殊设计:
 * - 单例模式（Singleton）：表中只能有1条记录
 * - 无主键设计：通过应用层保证单条记录
 * - 全局配置：影响整个游戏经济系统
 *
 * API设计: 7个函数
 * - 配置操作: 3个（获取/更新/初始化）
 * - 金币操作: 2个（增加供应/减少供应）
 * - 工具函数: 2个（检查状态/打印信息）
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #65/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 自动市场条件控制配置 */
typedef struct {
    unsigned long long optimum_gold_supply;  /* 最优金币供应量 */
    unsigned long long over_gold;            /* 超额金币量 */
} AutoMarketConditionCtrl;

/*******************************************************************************
 * 配置操作 (3个函数)
 ******************************************************************************/

/**
 * 获取当前市场控制配置（读取唯一记录）
 * @param manager 数据库连接管理器
 * @param config 输出参数，存储配置信息
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrl_Get(DBConnectionManager* manager, AutoMarketConditionCtrl* config);

/**
 * 更新市场控制配置（更新唯一记录）
 * @param manager 数据库连接管理器
 * @param config 新配置信息
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrl_Update(DBConnectionManager* manager, const AutoMarketConditionCtrl* config);

/**
 * 初始化市场控制配置（如果表为空则插入默认记录）
 * @param manager 数据库连接管理器
 * @param optimum_gold_supply 初始最优金币供应量
 * @param over_gold 初始超额金币量
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrl_Initialize(DBConnectionManager* manager,
                                       unsigned long long optimum_gold_supply,
                                       unsigned long long over_gold);

/*******************************************************************************
 * 金币操作 (2个函数)
 ******************************************************************************/

/**
 * 增加金币供应量（原子操作）
 * @param manager 数据库连接管理器
 * @param amount 增加的金币量
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrl_IncreaseSupply(DBConnectionManager* manager, unsigned long long amount);

/**
 * 减少金币供应量（原子操作）
 * @param manager 数据库连接管理器
 * @param amount 减少的金币量
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrl_DecreaseSupply(DBConnectionManager* manager, unsigned long long amount);

/*******************************************************************************
 * 工具函数 (2个函数)
 ******************************************************************************/

/**
 * 检查配置是否存在
 * @param manager 数据库连接管理器
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int AutoMarketConditionCtrl_Exists(DBConnectionManager* manager);

/**
 * 打印市场控制配置信息（用于调试）
 * @param config 配置指针
 */
void AutoMarketConditionCtrl_PrintInfo(const AutoMarketConditionCtrl* config);

#endif /* AUTO_MARKET_CONDITION_CTRL_ORM_H */
