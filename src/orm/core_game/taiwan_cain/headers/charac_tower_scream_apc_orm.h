#ifndef CHARAC_TOWER_SCREAM_APC_ORM_H
#define CHARAC_TOWER_SCREAM_APC_ORM_H

#include "db_connection_manager.h"

/**
 * 角色惊叫之塔APC记录 ORM
 *
 * 表: charac_tower_scream_apc
 * 主键: (reg_date, seq) 复合主键
 * 功能: 每日惊叫之塔APC记录存储、查询、统计
 */

/* 数据结构 */
typedef struct {
    char reg_date[12];      /* 注册日期 (主键1) YYYY-MM-DD */
    int charac_no;          /* 角色编号 */
    int seq;                /* 序列号 (主键2) */
} CharacTowerScreamApc;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加惊叫之塔APC记录
 */
int CharacTowerScreamApc_Add(DBConnectionManager* manager, const CharacTowerScreamApc* record);

/**
 * 获取惊叫之塔APC记录（按reg_date和seq）
 */
int CharacTowerScreamApc_Get(DBConnectionManager* manager, const char* reg_date, int seq, CharacTowerScreamApc* record);

/**
 * 更新惊叫之塔APC记录
 */
int CharacTowerScreamApc_Update(DBConnectionManager* manager, const CharacTowerScreamApc* record);

/**
 * 删除惊叫之塔APC记录（按reg_date和seq）
 */
int CharacTowerScreamApc_Delete(DBConnectionManager* manager, const char* reg_date, int seq);

/**
 * 检查惊叫之塔APC记录是否存在
 */
int CharacTowerScreamApc_Exists(DBConnectionManager* manager, const char* reg_date, int seq);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定日期的所有惊叫之塔APC记录
 */
int CharacTowerScreamApc_GetByDate(DBConnectionManager* manager, const char* reg_date,
                                   CharacTowerScreamApc* records, int max_result);

/**
 * 获取指定角色的所有惊叫之塔APC记录
 */
int CharacTowerScreamApc_GetByCharac(DBConnectionManager* manager, int charac_no,
                                     CharacTowerScreamApc* records, int max_result);

/**
 * 获取指定日期范围的惊叫之塔APC记录
 */
int CharacTowerScreamApc_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                        CharacTowerScreamApc* records, int max_result);

/**
 * 统计指定日期的惊叫之塔APC记录数量
 */
int CharacTowerScreamApc_CountByDate(DBConnectionManager* manager, const char* reg_date);

/**
 * 统计指定角色的惊叫之塔APC记录数量
 */
int CharacTowerScreamApc_CountByCharac(DBConnectionManager* manager, int charac_no);

/**
 * 删除指定日期之前的惊叫之塔APC记录（数据清理）
 */
int CharacTowerScreamApc_DeleteBeforeDate(DBConnectionManager* manager, const char* before_date);

/**
 * 删除指定日期的所有惊叫之塔APC记录
 */
int CharacTowerScreamApc_DeleteByDate(DBConnectionManager* manager, const char* reg_date);

/* ==================== 工具函数 ==================== */

/**
 * 打印惊叫之塔APC记录信息
 */
void CharacTowerScreamApc_PrintInfo(const CharacTowerScreamApc* record);

#endif /* CHARAC_TOWER_SCREAM_APC_ORM_H */
