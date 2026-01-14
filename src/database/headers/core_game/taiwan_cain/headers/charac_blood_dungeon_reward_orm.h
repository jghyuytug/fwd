#ifndef CHARAC_BLOOD_DUNGEON_REWARD_ORM_H
#define CHARAC_BLOOD_DUNGEON_REWARD_ORM_H

#include "db_connection_manager.h"

/**
 * 角色血色副本奖励记录表 ORM
 *
 * 表: charac_blood_dungeon_reward
 * 主键: (charac_no, week_occ_date) 复合主键
 * 功能: 记录角色每周血色副本数据（积分、进入次数、排名、奖励领取状态）
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;              /* 角色编号 (复合主键1) */
    char week_occ_date[12];              /* 周统计日期 YYYY-MM-DD (复合主键2) */
    unsigned int week_point;             /* 周积分 */
    unsigned int week_enter_count;       /* 周进入次数 */
    unsigned int week_use_gold;          /* 周使用金币 */
    char last_play_date[12];             /* 最后游玩日期 YYYY-MM-DD */
    unsigned int enter_count;            /* 总进入次数 */
    unsigned char rank;                  /* 排名 */
    unsigned char reward;                /* 奖励状态 (0=未领取, 1=已领取) */
    unsigned int reward_item_id;         /* 奖励物品ID */
    unsigned int reward_gold;            /* 奖励金币数量 */
} CharacBloodDungeonReward;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色血色副本奖励记录
 */
int CharacBloodDungeonReward_Add(DBConnectionManager* manager, const CharacBloodDungeonReward* record);

/**
 * 获取角色指定周的血色副本奖励记录
 */
int CharacBloodDungeonReward_Get(DBConnectionManager* manager, unsigned int charac_no,
                                  const char* week_occ_date, CharacBloodDungeonReward* record);

/**
 * 更新角色血色副本奖励记录
 */
int CharacBloodDungeonReward_Update(DBConnectionManager* manager, const CharacBloodDungeonReward* record);

/**
 * 删除角色血色副本奖励记录
 */
int CharacBloodDungeonReward_Delete(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* week_occ_date);

/**
 * 检查角色指定周的血色副本奖励记录是否存在
 */
int CharacBloodDungeonReward_Exists(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* week_occ_date);

/* ==================== 查询操作 ==================== */

/**
 * 获取角色的所有周奖励记录（按周倒序）
 */
int CharacBloodDungeonReward_GetByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                          CharacBloodDungeonReward* records, int max_records);

/**
 * 获取指定周的所有角色奖励记录（按周积分倒序）
 */
int CharacBloodDungeonReward_GetByWeekOccDate(DBConnectionManager* manager, const char* week_occ_date,
                                               CharacBloodDungeonReward* records, int max_records);

/**
 * 获取指定日期最后游玩的所有记录
 */
int CharacBloodDungeonReward_GetByLastPlayDate(DBConnectionManager* manager, const char* last_play_date,
                                                 CharacBloodDungeonReward* records, int max_records);

/**
 * 获取角色未领取奖励的记录（reward = 0）
 */
int CharacBloodDungeonReward_GetUnrewardedByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                                     CharacBloodDungeonReward* records, int max_records);

/**
 * 获取指定周周积分排行榜（前N名）
 */
int CharacBloodDungeonReward_GetTopByWeekPoint(DBConnectionManager* manager, const char* week_occ_date,
                                                 int limit, CharacBloodDungeonReward* records, int max_records);

/**
 * 获取角色总周积分（所有周的周积分之和）
 */
long long CharacBloodDungeonReward_GetTotalWeekPoint(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 获取已领取奖励的记录数（reward = 1）
 */
int CharacBloodDungeonReward_GetRewardedCount(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 统计操作 ==================== */

/**
 * 统计角色的周记录数量
 */
int CharacBloodDungeonReward_CountByCharac(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 统计指定周的记录数量
 */
int CharacBloodDungeonReward_CountByWeekOccDate(DBConnectionManager* manager, const char* week_occ_date);

/**
 * 统计指定奖励状态的记录数量
 */
int CharacBloodDungeonReward_CountByRewardStatus(DBConnectionManager* manager, unsigned char reward_status);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色血色副本奖励记录信息
 */
void CharacBloodDungeonReward_PrintInfo(const CharacBloodDungeonReward* record);

#endif /* CHARAC_BLOOD_DUNGEON_REWARD_ORM_H */
