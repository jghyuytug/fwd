#ifndef EVENT_CONDITION_INFO_ORM_H
#define EVENT_CONDITION_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * 活动条件信息 ORM
 *
 * 表: event_condition_info
 * 主键: charac_no
 * 功能: 记录角色在活动中的步骤进度和奖励领取信息
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;             /* 角色编号 (主键) */
    unsigned char current_step;         /* 当前步骤 (tinyint unsigned) */
    unsigned char reward_step;          /* 奖励步骤 (tinyint unsigned) */
    char update_time[20];               /* 更新时间 (datetime, YYYY-MM-DD HH:MM:SS) */
} EventConditionInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新活动条件信息
 */
int EventConditionInfo_Add(DBConnectionManager* manager, const EventConditionInfo* record);

/**
 * 获取活动条件信息
 */
int EventConditionInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                           EventConditionInfo* record);

/**
 * 更新活动条件信息
 */
int EventConditionInfo_Update(DBConnectionManager* manager, const EventConditionInfo* record);

/**
 * 删除活动条件信息
 */
int EventConditionInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查活动条件信息是否存在
 */
int EventConditionInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加当前步骤
 * @param increment 增量值（默认为1）
 */
int EventConditionInfo_IncrementCurrentStep(DBConnectionManager* manager, unsigned int charac_no,
                                            int increment);

/**
 * 更新当前步骤
 */
int EventConditionInfo_UpdateCurrentStep(DBConnectionManager* manager, unsigned int charac_no,
                                         unsigned char current_step);

/**
 * 更新奖励步骤
 */
int EventConditionInfo_UpdateRewardStep(DBConnectionManager* manager, unsigned int charac_no,
                                        unsigned char reward_step);

/**
 * 重置进度（将current_step和reward_step都设为0）
 */
int EventConditionInfo_ResetProgress(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 批量重置所有角色的进度
 */
int EventConditionInfo_BatchResetProgress(DBConnectionManager* manager);

/**
 * 领取奖励（将reward_step设置为current_step）
 */
int EventConditionInfo_ClaimReward(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 按当前步骤精确查询
 */
int EventConditionInfo_GetByCurrentStep(DBConnectionManager* manager, unsigned char current_step,
                                        EventConditionInfo* records, int max_result);

/**
 * 按最小当前步骤查询（用于排行榜）
 */
int EventConditionInfo_GetByMinCurrentStep(DBConnectionManager* manager, unsigned char min_current_step,
                                           EventConditionInfo* records, int max_result);

/**
 * 获取当前步骤TOP N
 */
int EventConditionInfo_GetTopByCurrentStep(DBConnectionManager* manager, int top_n,
                                           EventConditionInfo* records, int max_result);

/**
 * 查询未领取奖励的角色（current_step > reward_step）
 */
int EventConditionInfo_GetUnclaimedRewards(DBConnectionManager* manager,
                                           EventConditionInfo* records, int max_result);

/**
 * 按更新时间查询
 * @param after_time 查询此时间之后更新的记录 (YYYY-MM-DD HH:MM:SS)
 */
int EventConditionInfo_GetByUpdatedAfter(DBConnectionManager* manager, const char* after_time,
                                         EventConditionInfo* records, int max_result);

/**
 * 统计指定步骤的角色数量
 */
int EventConditionInfo_CountByCurrentStep(DBConnectionManager* manager, unsigned char current_step);

/**
 * 统计未领取奖励的角色数量
 */
int EventConditionInfo_CountUnclaimedRewards(DBConnectionManager* manager);

/**
 * 获取所有角色的平均步骤进度
 */
double EventConditionInfo_GetAverageProgress(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印活动条件信息
 */
void EventConditionInfo_PrintInfo(const EventConditionInfo* record);

#endif /* EVENT_CONDITION_INFO_ORM_H */
