#ifndef CHARAC_TOWER_SCREAM_ORM_H
#define CHARAC_TOWER_SCREAM_ORM_H

#include "db_connection_manager.h"

/**
 * 角色惊叫之塔记录 ORM
 *
 * 表: charac_tower_scream
 * 主键: charac_no
 * 功能: 惊叫之塔进度、进入次数统计
 */

/* 数据结构 */
typedef struct {
    int charac_no;                      /* 角色编号 (主键) */
    char first_layer_start_date[20];    /* 首层开始日期 */
    unsigned char today_enter_count;    /* 今日进入次数 */
    unsigned char last_clear_layer;     /* 最后通关层数 */
    int enter_count_by_week;            /* 本周进入次数 */
    char m_date[20];                    /* 修改日期 */
    char last_clear_date[20];           /* 最后通关日期 */
} CharacTowerScream;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新惊叫之塔记录
 */
int CharacTowerScream_Add(DBConnectionManager* manager, const CharacTowerScream* record);

/**
 * 获取惊叫之塔记录
 */
int CharacTowerScream_Get(DBConnectionManager* manager, int charac_no, CharacTowerScream* record);

/**
 * 更新惊叫之塔记录
 */
int CharacTowerScream_Update(DBConnectionManager* manager, const CharacTowerScream* record);

/**
 * 删除惊叫之塔记录
 */
int CharacTowerScream_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查惊叫之塔记录是否存在
 */
int CharacTowerScream_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新今日进入次数
 */
int CharacTowerScream_UpdateTodayEnterCount(DBConnectionManager* manager, int charac_no, unsigned char count);

/**
 * 增加今日进入次数
 */
int CharacTowerScream_IncrementTodayEnterCount(DBConnectionManager* manager, int charac_no);

/**
 * 重置今日进入次数（新的一天开始）
 */
int CharacTowerScream_ResetTodayEnterCount(DBConnectionManager* manager, int charac_no);

/**
 * 更新最后通关层数
 */
int CharacTowerScream_UpdateLastClearLayer(DBConnectionManager* manager, int charac_no, unsigned char layer);

/**
 * 更新本周进入次数
 */
int CharacTowerScream_UpdateEnterCountByWeek(DBConnectionManager* manager, int charac_no, int count);

/**
 * 增加本周进入次数
 */
int CharacTowerScream_IncrementEnterCountByWeek(DBConnectionManager* manager, int charac_no);

/**
 * 重置本周进入次数（新的一周开始）
 */
int CharacTowerScream_ResetEnterCountByWeek(DBConnectionManager* manager, int charac_no);

/**
 * 更新最后通关日期
 */
int CharacTowerScream_UpdateLastClearDate(DBConnectionManager* manager, int charac_no, const char* clear_date);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定层数以上的角色列表
 */
int CharacTowerScream_GetByMinLayer(DBConnectionManager* manager, unsigned char min_layer,
                                     CharacTowerScream* records, int max_result);

/**
 * 获取今日进入次数排行
 */
int CharacTowerScream_GetTopByTodayEnterCount(DBConnectionManager* manager, int limit,
                                                CharacTowerScream* records);

/**
 * 获取本周进入次数排行
 */
int CharacTowerScream_GetTopByWeekEnterCount(DBConnectionManager* manager, int limit,
                                               CharacTowerScream* records);

/**
 * 获取在指定日期之后修改的记录
 */
int CharacTowerScream_GetByModifiedAfter(DBConnectionManager* manager, const char* date,
                                           CharacTowerScream* records, int max_result);

/**
 * 批量重置今日进入次数（每日定时任务）
 */
int CharacTowerScream_BatchResetTodayEnterCount(DBConnectionManager* manager);

/**
 * 批量重置本周进入次数（每周定时任务）
 */
int CharacTowerScream_BatchResetWeekEnterCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印惊叫之塔记录信息
 */
void CharacTowerScream_PrintInfo(const CharacTowerScream* record);

#endif /* CHARAC_TOWER_SCREAM_ORM_H */
