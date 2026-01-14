#ifndef STAT_GAME_CHANNEL_ORM_H
#define STAT_GAME_CHANNEL_ORM_H

#include "db_connection_manager.h"

/**
 * 游戏频道统计数据 ORM
 *
 * 功能：
 * - 记录游戏频道历史在线人数统计
 * - 支持时序数据分析和报表生成
 * - 按频道和时间范围查询统计数据
 *
 * 注意：
 * - 无主键设计，使用复合索引 (gc_channel, gc_up_time)
 * - Get/Update/Delete/Exists 需要同时提供 gc_channel 和 gc_up_time
 */

/* 游戏频道统计记录结构 */
typedef struct {
    char gc_channel[11];      /* 频道名称（varchar(10) + \0） */
    char gc_up_time[20];      /* 更新时间（YYYY-MM-DD HH:MM:SS） */
    short gc_now;             /* 当前在线人数 */
} StatGameChannel;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加游戏频道统计记录（使用 INSERT）
 *
 * @param manager 数据库连接管理器
 * @param record 频道统计记录
 * @return 0 成功, <0 失败
 */
int StatGameChannel_Add(DBConnectionManager* manager, const StatGameChannel* record);

/**
 * 获取游戏频道统计记录（按复合键：频道+时间）
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param gc_up_time 更新时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int StatGameChannel_Get(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time, StatGameChannel* record);

/**
 * 更新游戏频道统计记录（按复合键：频道+时间）
 *
 * @param manager 数据库连接管理器
 * @param record 频道统计记录（必须包含有效的gc_channel和gc_up_time）
 * @return 0 成功, <0 失败
 */
int StatGameChannel_Update(DBConnectionManager* manager, const StatGameChannel* record);

/**
 * 删除游戏频道统计记录（按复合键：频道+时间）
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param gc_up_time 更新时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return 0 成功, <0 失败
 */
int StatGameChannel_Delete(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time);

/**
 * 检查游戏频道统计记录是否存在（按复合键：频道+时间）
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param gc_up_time 更新时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return 1 存在, 0 不存在, <0 错误
 */
int StatGameChannel_Exists(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time);

/* ==================== 专项操作 ==================== */

/**
 * 按频道查询最新统计记录
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param record 输出参数，接收最新记录
 * @return 0 成功, <0 失败或无记录
 */
int StatGameChannel_GetLatest(DBConnectionManager* manager, const char* gc_channel, StatGameChannel* record);

/**
 * 按频道删除指定时间范围内的记录
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 删除的记录数, <0 失败
 */
int StatGameChannel_DeleteByTimeRange(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time);

/**
 * 批量删除过期统计记录（保留最近N天）
 *
 * @param manager 数据库连接管理器
 * @param days 保留天数（删除早于此天数的记录）
 * @return >=0 删除的记录数, <0 失败
 */
int StatGameChannel_DeleteOlderThan(DBConnectionManager* manager, int days);

/* ==================== 查询操作 ==================== */

/**
 * 按频道统计记录数量
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @return >=0 记录数量, <0 失败
 */
int StatGameChannel_CountByChannel(DBConnectionManager* manager, const char* gc_channel);

/**
 * 统计所有频道的总记录数
 *
 * @param manager 数据库连接管理器
 * @return >=0 总记录数, <0 失败
 */
int StatGameChannel_CountAll(DBConnectionManager* manager);

/**
 * 按频道查询时间范围内的平均在线人数
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 平均在线人数, <0 失败
 */
int StatGameChannel_GetAverageOnline(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time);

/**
 * 按频道查询时间范围内的最高在线人数
 *
 * @param manager 数据库连接管理器
 * @param gc_channel 频道名称
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 最高在线人数, <0 失败
 */
int StatGameChannel_GetMaxOnline(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time);

/**
 * 统计不同频道数量
 *
 * @param manager 数据库连接管理器
 * @return >=0 频道数量, <0 失败
 */
int StatGameChannel_CountDistinctChannels(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印游戏频道统计记录（调试用）
 *
 * @param record 频道统计记录
 */
void StatGameChannel_PrintInfo(const StatGameChannel* record);

#endif /* STAT_GAME_CHANNEL_ORM_H */
