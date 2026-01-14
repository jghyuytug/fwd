#ifndef SINGLE_RANK_AVG_ORM_H
#define SINGLE_RANK_AVG_ORM_H

#include "db_connection_manager.h"

/* single_rank_avg 表数据结构 */
typedef struct {
    short dungeon_index;       /* 副本索引（主键1）*/
    short level;               /* 等级（主键2）*/
    short job;                 /* 职业（主键3）*/
    long long clear_count;     /* 通关次数 */
    int average;               /* 平均值 */
} SingleRankAvg;

/* ========== CRUD 操作 ========== */

/**
 * 添加或更新单人排名统计
 * @param manager 数据库连接管理器
 * @param record 单人排名统计记录
 * @return 0成功，-1失败
 */
int SingleRankAvg_Add(DBConnectionManager* manager, const SingleRankAvg* record);

/**
 * 获取单人排名统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @param job 职业
 * @param record 输出的统计记录
 * @return 0成功，-1失败
 */
int SingleRankAvg_Get(DBConnectionManager* manager, short dungeon_index, short level, 
                      short job, SingleRankAvg* record);

/**
 * 更新单人排名统计
 * @param manager 数据库连接管理器
 * @param record 单人排名统计记录
 * @return 0成功，-1失败
 */
int SingleRankAvg_Update(DBConnectionManager* manager, const SingleRankAvg* record);

/**
 * 删除单人排名统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @param job 职业
 * @return 0成功，-1失败
 */
int SingleRankAvg_Delete(DBConnectionManager* manager, short dungeon_index, short level, short job);

/**
 * 检查单人排名统计是否存在
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @param job 职业
 * @return 1存在，0不存在，-1错误
 */
int SingleRankAvg_Exists(DBConnectionManager* manager, short dungeon_index, short level, short job);

/* ========== 专项操作 ========== */

/**
 * 增加通关次数
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @param job 职业
 * @param count 增加的次数
 * @return 0成功，-1失败
 */
int SingleRankAvg_IncrementClearCount(DBConnectionManager* manager, short dungeon_index, 
                                      short level, short job, long long count);

/**
 * 更新平均值
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @param job 职业
 * @param average 新平均值
 * @return 0成功，-1失败
 */
int SingleRankAvg_UpdateAverage(DBConnectionManager* manager, short dungeon_index, 
                                short level, short job, int average);

/* ========== 查询操作 ========== */

/**
 * 获取指定副本的所有统计数量
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @return 记录数量，-1失败
 */
int SingleRankAvg_CountByDungeon(DBConnectionManager* manager, short dungeon_index);

/**
 * 获取指定副本和等级的所有职业统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @param level 等级
 * @return 记录数量，-1失败
 */
int SingleRankAvg_CountByDungeonAndLevel(DBConnectionManager* manager, short dungeon_index, short level);

/**
 * 获取指定职业的总通关次数
 * @param manager 数据库连接管理器
 * @param job 职业
 * @return 总通关次数，-1失败
 */
long long SingleRankAvg_GetTotalClearCountByJob(DBConnectionManager* manager, short job);

/**
 * 删除指定副本的所有统计
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引
 * @return 0成功，-1失败
 */
int SingleRankAvg_DeleteByDungeon(DBConnectionManager* manager, short dungeon_index);

/**
 * 获取所有统计记录总数
 * @param manager 数据库连接管理器
 * @return 记录总数，-1失败
 */
int SingleRankAvg_Count(DBConnectionManager* manager);

/* ========== 工具函数 ========== */

/**
 * 打印单人排名统计（调试用）
 * @param record 统计记录
 */
void SingleRankAvg_PrintInfo(const SingleRankAvg* record);

#endif /* SINGLE_RANK_AVG_ORM_H */
