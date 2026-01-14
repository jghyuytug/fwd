#ifndef CHARAC_ACTION_POINT_ORM_H
#define CHARAC_ACTION_POINT_ORM_H

#include "db_connection_manager.h"

/**
 * 角色行动点数 ORM
 *
 * 表: charac_action_point
 * 主键: (charac_no, occ_date)
 * 功能: 记录角色每日AP累计和奖励领取情况
 */

/* 数据结构 */
typedef struct {
    char occ_date[11];                  /* 发生日期 (主键, date, YYYY-MM-DD) */
    unsigned int charac_no;             /* 角色编号 (主键) */
    unsigned int ap_sum;                /* AP总和 */
    unsigned char is_reward_medal;      /* 奖励勋章标识 */
    unsigned char is_reward_item_1;     /* 奖励物品1标识 */
    unsigned char is_reward_item_2;     /* 奖励物品2标识 */
    unsigned char is_reward_item_3;     /* 奖励物品3标识 */
    unsigned char is_reward_item_4;     /* 奖励物品4标识 */
    unsigned char* ap_clear_state;      /* AP清除状态(二进制) */
    int ap_clear_state_len;             /* ap_clear_state数据长度 */
} CharacActionPoint;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色行动点数记录
 */
int CharacActionPoint_Add(DBConnectionManager* manager, const CharacActionPoint* record);

/**
 * 获取角色行动点数记录
 */
int CharacActionPoint_Get(DBConnectionManager* manager, unsigned int charac_no,
                          const char* occ_date, CharacActionPoint* record);

/**
 * 更新角色行动点数记录
 */
int CharacActionPoint_Update(DBConnectionManager* manager, const CharacActionPoint* record);

/**
 * 删除角色行动点数记录
 */
int CharacActionPoint_Delete(DBConnectionManager* manager, unsigned int charac_no,
                             const char* occ_date);

/**
 * 检查角色行动点数记录是否存在
 */
int CharacActionPoint_Exists(DBConnectionManager* manager, unsigned int charac_no,
                             const char* occ_date);

/* ==================== 专项操作 ==================== */

/**
 * 增加AP值
 */
int CharacActionPoint_IncrementAPSum(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* occ_date, unsigned int ap_value);

/**
 * 领取勋章奖励
 */
int CharacActionPoint_ClaimRewardMedal(DBConnectionManager* manager, unsigned int charac_no,
                                       const char* occ_date);

/**
 * 领取物品奖励
 * @param item_slot 物品槽位 (1-4)
 */
int CharacActionPoint_ClaimRewardItem(DBConnectionManager* manager, unsigned int charac_no,
                                      const char* occ_date, int item_slot);

/**
 * 更新AP清除状态
 */
int CharacActionPoint_UpdateAPClearState(DBConnectionManager* manager, unsigned int charac_no,
                                         const char* occ_date,
                                         const unsigned char* clear_state, int state_len);

/* ==================== 查询操作 ==================== */

/**
 * 按日期查询所有角色的AP记录
 */
int CharacActionPoint_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                CharacActionPoint* records, int max_result);

/**
 * 按角色查询所有日期的AP记录
 */
int CharacActionPoint_GetByCharacNo(DBConnectionManager* manager, unsigned int charac_no,
                                    CharacActionPoint* records, int max_result);

/**
 * 检查奖励领取状态
 */
int CharacActionPoint_CheckRewardStatus(DBConnectionManager* manager, unsigned int charac_no,
                                        const char* occ_date,
                                        int* medal_claimed, int* items_claimed);

/**
 * 获取指定日期AP总和大于指定值的角色
 */
int CharacActionPoint_GetByMinAPSum(DBConnectionManager* manager, const char* occ_date,
                                    unsigned int min_ap,
                                    CharacActionPoint* records, int max_result);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色行动点数记录信息
 */
void CharacActionPoint_PrintInfo(const CharacActionPoint* record);

/**
 * 释放CharacActionPoint结构中的动态内存
 */
void CharacActionPoint_Free(CharacActionPoint* record);

#endif /* CHARAC_ACTION_POINT_ORM_H */
