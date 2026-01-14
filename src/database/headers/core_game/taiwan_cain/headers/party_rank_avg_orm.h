#ifndef PARTY_RANK_AVG_ORM_H
#define PARTY_RANK_AVG_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Party Rank Avg ORM - 队伍副本平均统计系统
 *
 * 表: party_rank_avg (taiwan_cain 数据库)
 * 描述: 队伍副本通关平均统计表
 * 主键: (dungeon_index, party_level) 复合主键
 *
 * 字段说明:
 * - dungeon_index: 副本索引 (SMALLINT, PRIMARY KEY 1)
 * - party_level: 队伍等级 (SMALLINT, PRIMARY KEY 2)
 * - clear_count: 通关次数 (BIGINT)
 * - average: 平均值（通关时间/分数等） (INT)
 *
 * 业务用途:
 * - 统计不同等级队伍的副本通关数据
 * - 计算平均通关时间或分数
 * - 用于副本难度平衡和排名系统
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个 (含按副本查询)
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #58/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 队伍副本平均统计记录结构 */
typedef struct {
    short dungeon_index;      /* 副本索引 (复合主键1) */
    short party_level;        /* 队伍等级 (复合主键2) */
    long long clear_count;    /* 通关次数 */
    int average;              /* 平均值（时间/分数等） */
} PartyRankAvg;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新队伍副本统计（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int PartyRankAvg_Add(DBConnectionManager* manager, const PartyRankAvg* record);

/**
 * 获取指定副本和队伍等级的统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param party_level 队伍等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int PartyRankAvg_Get(DBConnectionManager* manager, short dungeon_index, short party_level, PartyRankAvg* record);

/**
 * 更新队伍副本统计
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int PartyRankAvg_Update(DBConnectionManager* manager, const PartyRankAvg* record);

/**
 * 删除队伍副本统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param party_level 队伍等级
 * @return 成功返回0，失败返回-1
 */
int PartyRankAvg_Delete(DBConnectionManager* manager, short dungeon_index, short party_level);

/**
 * 检查队伍副本统计是否存在
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param party_level 队伍等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int PartyRankAvg_Exists(DBConnectionManager* manager, short dungeon_index, short party_level);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取统计记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int PartyRankAvg_Count(DBConnectionManager* manager);

/**
 * 获取所有统计记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int PartyRankAvg_GetAll(DBConnectionManager* manager, PartyRankAvg* records, int max_records);

/**
 * 按副本索引获取所有等级的统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int PartyRankAvg_GetByDungeon(DBConnectionManager* manager, short dungeon_index, PartyRankAvg* records, int max_records);

/**
 * 统计指定副本的等级记录数
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @return >=0记录数，<0失败
 */
int PartyRankAvg_CountByDungeon(DBConnectionManager* manager, short dungeon_index);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除队伍副本统计
 * @param manager 数据库连接管理器
 * @param records 要删除的记录数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int PartyRankAvg_BatchDelete(DBConnectionManager* manager, const PartyRankAvg* records, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印队伍副本统计信息（用于调试）
 * @param record 队伍副本统计记录指针
 */
void PartyRankAvg_PrintInfo(const PartyRankAvg* record);

#endif /* PARTY_RANK_AVG_ORM_H */
