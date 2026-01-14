#ifndef BLOOD_DUNGEON_RANK_SELECT_ORM_H
#define BLOOD_DUNGEON_RANK_SELECT_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Blood Dungeon Rank Select ORM - 血色副本排名奖励配置系统
 *
 * 表: blood_dungeon_rank_select (taiwan_cain 数据库)
 * 描述: 血色副本积分段排名奖励配置表
 * 主键: (min_amount, max_amount, rank) 复合主键
 *
 * 字段说明:
 * - min_amount: 积分区间最小值 (BIGINT, PRIMARY KEY)
 * - max_amount: 积分区间最大值 (BIGINT, PRIMARY KEY)
 * - rank: 排名 (TINYINT UNSIGNED, PRIMARY KEY)
 * - reward_item_id: 奖励物品ID (INT UNSIGNED)
 * - reward_gold: 奖励金币数量 (INT UNSIGNED)
 * - winner_count: 获奖人数 (INT UNSIGNED)
 *
 * 业务用途:
 * - 奖励配置：根据积分区间和排名配置奖励
 * - 排行榜：支持不同积分段的排名奖励发放
 * - 活动管理：配置血色副本周赛奖励规则
 *
 * API设计: 12个函数
 * - CRUD操作: 5个
 * - 查询操作: 5个（按积分段/按排名/所有配置/积分段内所有排名）
 * - 统计操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #68/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 血色副本排名奖励配置 */
typedef struct {
    long long min_amount;               /* 积分区间最小值 (复合主键1) */
    long long max_amount;               /* 积分区间最大值 (复合主键2) */
    unsigned char rank;                 /* 排名 (复合主键3) */
    unsigned int reward_item_id;        /* 奖励物品ID */
    unsigned int reward_gold;           /* 奖励金币数量 */
    unsigned int winner_count;          /* 获奖人数 */
} BloodDungeonRankSelect;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

int BloodDungeonRankSelect_Add(DBConnectionManager* manager, const BloodDungeonRankSelect* record);
int BloodDungeonRankSelect_Get(DBConnectionManager* manager, long long min_amount, long long max_amount,
                                unsigned char rank, BloodDungeonRankSelect* record);
int BloodDungeonRankSelect_Update(DBConnectionManager* manager, const BloodDungeonRankSelect* record);
int BloodDungeonRankSelect_Delete(DBConnectionManager* manager, long long min_amount, long long max_amount, unsigned char rank);
int BloodDungeonRankSelect_Exists(DBConnectionManager* manager, long long min_amount, long long max_amount, unsigned char rank);

/*******************************************************************************
 * 查询操作 (5个函数)
 ******************************************************************************/

int BloodDungeonRankSelect_Count(DBConnectionManager* manager);
int BloodDungeonRankSelect_GetAll(DBConnectionManager* manager, BloodDungeonRankSelect* records, int max_records);
int BloodDungeonRankSelect_GetByPointRange(DBConnectionManager* manager, long long min_amount, long long max_amount,
                                            BloodDungeonRankSelect* records, int max_records);
int BloodDungeonRankSelect_GetByRank(DBConnectionManager* manager, unsigned char rank,
                                      BloodDungeonRankSelect* records, int max_records);
int BloodDungeonRankSelect_FindRewardByPoint(DBConnectionManager* manager, long long point, unsigned char rank,
                                              BloodDungeonRankSelect* record);

/*******************************************************************************
 * 统计操作 (1个函数)
 ******************************************************************************/

int BloodDungeonRankSelect_CountPointRanges(DBConnectionManager* manager);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

void BloodDungeonRankSelect_PrintInfo(const BloodDungeonRankSelect* record);

#endif /* BLOOD_DUNGEON_RANK_SELECT_ORM_H */
