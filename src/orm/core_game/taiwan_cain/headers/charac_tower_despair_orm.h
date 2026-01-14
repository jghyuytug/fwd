#ifndef CHARAC_TOWER_DESPAIR_ORM_H
#define CHARAC_TOWER_DESPAIR_ORM_H

#include "db_connection_manager.h"

/**
 * 角色绝望之塔 ORM
 *
 * 表: charac_tower_despair
 * 主键: charac_no
 * 功能: 记录角色绝望之塔挑战数据（进入次数、通关层数、日期记录）
 */

/* 数据结构 */
typedef struct {
    int charac_no;                       /* 角色编号 (主键) */
    char first_layer_start_date[20];    /* 首层开始日期 (YYYY-MM-DD HH:MM:SS) */
    unsigned char today_enter_count;    /* 今日进入次数 */
    unsigned char last_clear_layer;     /* 最后通关层数 */
    int enter_count_by_week;            /* 每周进入次数 */
    char m_date[20];                    /* 修改日期 (YYYY-MM-DD HH:MM:SS) */
    char last_clear_date[20];           /* 最后通关日期 (YYYY-MM-DD HH:MM:SS) */
} CharacTowerDespair;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色绝望之塔数据
 */
int CharacTowerDespair_Add(DBConnectionManager* manager, const CharacTowerDespair* record);

/**
 * 获取角色绝望之塔数据
 */
int CharacTowerDespair_Get(DBConnectionManager* manager, int charac_no, CharacTowerDespair* record);

/**
 * 更新角色绝望之塔数据
 */
int CharacTowerDespair_Update(DBConnectionManager* manager, const CharacTowerDespair* record);

/**
 * 删除角色绝望之塔数据
 */
int CharacTowerDespair_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色绝望之塔数据是否存在
 */
int CharacTowerDespair_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加今日进入次数
 */
int CharacTowerDespair_IncrementTodayEnterCount(DBConnectionManager* manager, int charac_no, int increment);

/**
 * 重置今日进入次数为0
 */
int CharacTowerDespair_ResetTodayEnterCount(DBConnectionManager* manager, int charac_no);

/**
 * 更新最后通关层数
 */
int CharacTowerDespair_UpdateLastClearLayer(DBConnectionManager* manager, int charac_no, unsigned char layer);

/**
 * 增加每周进入次数
 */
int CharacTowerDespair_IncrementWeekEnterCount(DBConnectionManager* manager, int charac_no, int increment);

/**
 * 重置每周进入次数为0
 */
int CharacTowerDespair_ResetWeekEnterCount(DBConnectionManager* manager, int charac_no);

/**
 * 更新最后通关日期为当前时间
 */
int CharacTowerDespair_UpdateLastClearDate(DBConnectionManager* manager, int charac_no);

/**
 * 更新修改日期为当前时间
 */
int CharacTowerDespair_UpdateMDate(DBConnectionManager* manager, int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 获取总记录数
 */
int CharacTowerDespair_GetTotalCount(DBConnectionManager* manager);

/**
 * 获取指定层数范围的角色列表
 */
int CharacTowerDespair_GetByClearLayerRange(DBConnectionManager* manager, unsigned char min_layer, unsigned char max_layer,
                                              int* charac_nos, int max_result);

/**
 * 获取最高通关层数排行榜
 */
int CharacTowerDespair_GetTopByLayer(DBConnectionManager* manager, int limit, int* charac_nos);

/**
 * 获取今日挑战次数最多的角色
 */
int CharacTowerDespair_GetTopByTodayCount(DBConnectionManager* manager, int limit, int* charac_nos);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色绝望之塔信息
 */
void CharacTowerDespair_PrintInfo(const CharacTowerDespair* record);

#endif /* CHARAC_TOWER_DESPAIR_ORM_H */
