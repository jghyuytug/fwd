#ifndef AUTO_MARKET_CONDITION_CTRL_DAILY_ORM_H
#define AUTO_MARKET_CONDITION_CTRL_DAILY_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Auto Market Condition Control Daily ORM - 市场条件控制每日统计系统
 *
 * 表: auto_market_condition_ctrl_daily (taiwan_cain 数据库)
 * 描述: 游戏经济系统每日金币流通和经济阶段统计表
 * 主键: occ_time (DATE)
 *
 * 字段说明:
 * - occ_time: 统计日期 (DATE, PRIMARY KEY)
 * - total_gold: 金币总量 (BIGINT UNSIGNED)
 * - auction_gold: 拍卖行金币量 (BIGINT UNSIGNED)
 * - over_gold: 超额金币量 (BIGINT UNSIGNED)
 * - optimum_gold_supply: 最优金币供应量 (BIGINT UNSIGNED)
 * - gold_phase: 金币阶段 (INT, 经济周期标识)
 * - item_phase: 物品阶段 (INT, 物品流通标识)
 * - durability_phase: 耐久度阶段 (INT UNSIGNED, 装备损耗标识)
 *
 * 业务用途:
 * - 每日快照：记录每天的经济指标
 * - 趋势分析：统计金币供应变化趋势
 * - 阶段监控：跟踪经济周期和物品流通阶段
 * - 数据报表：生成经济健康度报告
 *
 * API设计: 12个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个（按日期范围/最近N天/阶段）
 * - 统计操作: 2个（平均值/趋势）
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #67/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 市场条件控制每日统计 */
typedef struct {
    char occ_time[12];                      /* 统计日期 (YYYY-MM-DD, 主键) */
    unsigned long long total_gold;          /* 金币总量 */
    unsigned long long auction_gold;        /* 拍卖行金币量 */
    unsigned long long over_gold;           /* 超额金币量 */
    unsigned long long optimum_gold_supply; /* 最优金币供应量 */
    int gold_phase;                         /* 金币阶段 */
    int item_phase;                         /* 物品阶段 */
    unsigned int durability_phase;          /* 耐久度阶段 */
} AutoMarketConditionCtrlDaily;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

int AutoMarketConditionCtrlDaily_Add(DBConnectionManager* manager, const AutoMarketConditionCtrlDaily* record);
int AutoMarketConditionCtrlDaily_Get(DBConnectionManager* manager, const char* occ_time, AutoMarketConditionCtrlDaily* record);
int AutoMarketConditionCtrlDaily_Update(DBConnectionManager* manager, const AutoMarketConditionCtrlDaily* record);
int AutoMarketConditionCtrlDaily_Delete(DBConnectionManager* manager, const char* occ_time);
int AutoMarketConditionCtrlDaily_Exists(DBConnectionManager* manager, const char* occ_time);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

int AutoMarketConditionCtrlDaily_Count(DBConnectionManager* manager);
int AutoMarketConditionCtrlDaily_GetAll(DBConnectionManager* manager, AutoMarketConditionCtrlDaily* records, int max_records);
int AutoMarketConditionCtrlDaily_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                                 AutoMarketConditionCtrlDaily* records, int max_records);
int AutoMarketConditionCtrlDaily_GetRecent(DBConnectionManager* manager, int days,
                                           AutoMarketConditionCtrlDaily* records, int max_records);

/*******************************************************************************
 * 统计操作 (2个函数)
 ******************************************************************************/

long long AutoMarketConditionCtrlDaily_GetAverageTotalGold(DBConnectionManager* manager, int days);
int AutoMarketConditionCtrlDaily_GetGoldPhaseCount(DBConnectionManager* manager, int phase, int days);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

void AutoMarketConditionCtrlDaily_PrintInfo(const AutoMarketConditionCtrlDaily* record);

#endif /* AUTO_MARKET_CONDITION_CTRL_DAILY_ORM_H */
