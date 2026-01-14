#ifndef AUTO_MARKET_CONDITION_CTRL_CHANGE_ORM_H
#define AUTO_MARKET_CONDITION_CTRL_CHANGE_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Auto Market Condition Control Change ORM - 自动市场条件控制变更记录系统
 *
 * 表: auto_market_condition_ctrl_change (taiwan_cain 数据库)
 * 描述: 游戏经济系统金币供应参数变更审计日志表
 * 主键: occ_time (DATE)
 *
 * 字段说明:
 * - occ_time: 发生日期 (DATE, PRIMARY KEY, "YYYY-MM-DD")
 * - total_gold_old: 变更前金币总量 (BIGINT UNSIGNED)
 * - over_gold_old: 变更前超额金币量 (BIGINT UNSIGNED)
 * - total_gold_new: 变更后金币总量 (BIGINT UNSIGNED)
 * - over_gold_new: 变更后超额金币量 (BIGINT UNSIGNED)
 * - MNG_user_id: 管理员用户ID (VARCHAR(30))
 * - memo: 备注信息 (VARCHAR(255))
 *
 * 业务用途:
 * - 审计跟踪：记录所有市场控制参数变更历史
 * - 合规要求：满足游戏经济监管需求
 * - 问题排查：追溯金币供应变化原因
 * - 数据分析：统计管理员操作频率和影响
 *
 * 特殊设计:
 * - 主键为DATE：每天只保留一条最新记录
 * - 旧值/新值对比：完整记录变更前后状态
 * - 管理员追踪：记录操作人员
 * - 备注字段：记录操作原因
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个（按日期范围/管理员/时间排序）
 * - 统计操作: 1个（变更频率统计）
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #66/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 市场条件控制变更记录 */
typedef struct {
    char occ_time[12];                    /* 发生日期 (YYYY-MM-DD, 主键) */
    unsigned long long total_gold_old;    /* 变更前金币总量 */
    unsigned long long over_gold_old;     /* 变更前超额金币量 */
    unsigned long long total_gold_new;    /* 变更后金币总量 */
    unsigned long long over_gold_new;     /* 变更后超额金币量 */
    char MNG_user_id[31];                 /* 管理员用户ID */
    char memo[256];                       /* 备注信息 */
} AutoMarketConditionCtrlChange;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加市场控制变更记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrlChange_Add(DBConnectionManager* manager, const AutoMarketConditionCtrlChange* record);

/**
 * 获取指定日期的变更记录
 * @param manager 数据库连接管理器
 * @param occ_time 发生日期 (格式: "YYYY-MM-DD")
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int AutoMarketConditionCtrlChange_Get(DBConnectionManager* manager, const char* occ_time, AutoMarketConditionCtrlChange* record);

/**
 * 更新变更记录
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrlChange_Update(DBConnectionManager* manager, const AutoMarketConditionCtrlChange* record);

/**
 * 删除指定日期的变更记录
 * @param manager 数据库连接管理器
 * @param occ_time 发生日期 (格式: "YYYY-MM-DD")
 * @return 成功返回0，失败返回-1
 */
int AutoMarketConditionCtrlChange_Delete(DBConnectionManager* manager, const char* occ_time);

/**
 * 检查指定日期的变更记录是否存在
 * @param manager 数据库连接管理器
 * @param occ_time 发生日期 (格式: "YYYY-MM-DD")
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int AutoMarketConditionCtrlChange_Exists(DBConnectionManager* manager, const char* occ_time);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取变更记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int AutoMarketConditionCtrlChange_Count(DBConnectionManager* manager);

/**
 * 获取所有变更记录（按日期倒序）
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AutoMarketConditionCtrlChange_GetAll(DBConnectionManager* manager, AutoMarketConditionCtrlChange* records, int max_records);

/**
 * 获取指定日期范围内的变更记录
 * @param manager 数据库连接管理器
 * @param start_date 开始日期 (格式: "YYYY-MM-DD")
 * @param end_date 结束日期 (格式: "YYYY-MM-DD")
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AutoMarketConditionCtrlChange_GetByDateRange(DBConnectionManager* manager,
                                                  const char* start_date,
                                                  const char* end_date,
                                                  AutoMarketConditionCtrlChange* records,
                                                  int max_records);

/**
 * 获取指定管理员的所有变更记录
 * @param manager 数据库连接管理器
 * @param user_id 管理员用户ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AutoMarketConditionCtrlChange_GetByUser(DBConnectionManager* manager,
                                             const char* user_id,
                                             AutoMarketConditionCtrlChange* records,
                                             int max_records);

/*******************************************************************************
 * 统计操作 (1个函数)
 ******************************************************************************/

/**
 * 获取最近N天的变更次数
 * @param manager 数据库连接管理器
 * @param days 天数
 * @return >=0变更次数，<0失败
 */
int AutoMarketConditionCtrlChange_CountRecentChanges(DBConnectionManager* manager, int days);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印变更记录信息（用于调试）
 * @param record 变更记录指针
 */
void AutoMarketConditionCtrlChange_PrintInfo(const AutoMarketConditionCtrlChange* record);

#endif /* AUTO_MARKET_CONDITION_CTRL_CHANGE_ORM_H */
