#ifndef CHARAC_LINK_BONUS_ORM_H
#define CHARAC_LINK_BONUS_ORM_H

#include "db_connection_manager.h"

/**
 * 角色连接奖励表 ORM
 *
 * 表: charac_link_bonus
 * 主键: charac_no
 * 功能: 记录角色连接奖励（经验、金币）和雇佣兵信息
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;          /* 角色编号 (主键) */
    unsigned int exp;                /* 经验奖励 */
    unsigned int gold;               /* 金币奖励 */
    int mercenary_start_time;        /* 雇佣兵开始时间 (Unix时间戳) */
    int mercenary_finish_time;       /* 雇佣兵结束时间 (Unix时间戳) */
    char mercenary_area;             /* 雇佣兵区域 */
    char mercenary_period;           /* 雇佣兵周期 */
} CharacLinkBonus;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色连接奖励
 */
int CharacLinkBonus_Add(DBConnectionManager* manager, const CharacLinkBonus* record);

/**
 * 获取角色连接奖励
 */
int CharacLinkBonus_Get(DBConnectionManager* manager, unsigned int charac_no, CharacLinkBonus* record);

/**
 * 更新角色连接奖励
 */
int CharacLinkBonus_Update(DBConnectionManager* manager, const CharacLinkBonus* record);

/**
 * 删除角色连接奖励
 */
int CharacLinkBonus_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色连接奖励是否存在
 */
int CharacLinkBonus_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新经验奖励
 */
int CharacLinkBonus_UpdateExp(DBConnectionManager* manager, unsigned int charac_no, unsigned int exp);

/**
 * 增加经验奖励
 */
int CharacLinkBonus_AddExp(DBConnectionManager* manager, unsigned int charac_no, unsigned int exp);

/**
 * 更新金币奖励
 */
int CharacLinkBonus_UpdateGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold);

/**
 * 增加金币奖励
 */
int CharacLinkBonus_AddGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold);

/**
 * 更新雇佣兵信息
 */
int CharacLinkBonus_UpdateMercenaryInfo(DBConnectionManager* manager, unsigned int charac_no,
                                         int start_time, int finish_time, char area, char period);

/**
 * 检查雇佣兵是否有效（当前时间在雇佣期内）
 */
int CharacLinkBonus_IsMercenaryActive(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 清除雇佣兵信息
 */
int CharacLinkBonus_ClearMercenaryInfo(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 获取总经验奖励（所有角色）
 */
unsigned long long CharacLinkBonus_GetTotalExp(DBConnectionManager* manager);

/**
 * 获取总金币奖励（所有角色）
 */
unsigned long long CharacLinkBonus_GetTotalGold(DBConnectionManager* manager);

/**
 * 获取当前有效雇佣兵数量
 */
int CharacLinkBonus_GetActiveMercenaryCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色连接奖励信息
 */
void CharacLinkBonus_PrintInfo(const CharacLinkBonus* record);

#endif /* CHARAC_LINK_BONUS_ORM_H */
