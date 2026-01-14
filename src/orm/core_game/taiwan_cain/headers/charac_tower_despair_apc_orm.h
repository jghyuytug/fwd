#ifndef CHARAC_TOWER_DESPAIR_APC_ORM_H
#define CHARAC_TOWER_DESPAIR_APC_ORM_H

#include "db_connection_manager.h"

/**
 * 角色绝望之塔APC ORM
 *
 * 表: charac_tower_despair_apc
 * 主键: (reg_date, seq) 复合主键
 * 功能: 绝望之塔每日APC注册记录（可能用于活动或每日榜单）
 */

/* 数据结构 */
typedef struct {
    char reg_date[11];                  /* 注册日期 (YYYY-MM-DD) (主键) */
    int charac_no;                      /* 角色编号 */
    int seq;                            /* 序号 (主键) */
} CharacTowerDespairApc;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新绝望之塔APC记录
 */
int CharacTowerDespairApc_Add(DBConnectionManager* manager, const CharacTowerDespairApc* record);

/**
 * 获取绝望之塔APC记录（通过复合主键）
 */
int CharacTowerDespairApc_Get(DBConnectionManager* manager, const char* reg_date, int seq, CharacTowerDespairApc* record);

/**
 * 更新绝望之塔APC记录
 */
int CharacTowerDespairApc_Update(DBConnectionManager* manager, const CharacTowerDespairApc* record);

/**
 * 删除绝望之塔APC记录（通过复合主键）
 */
int CharacTowerDespairApc_Delete(DBConnectionManager* manager, const char* reg_date, int seq);

/**
 * 检查绝望之塔APC记录是否存在（通过复合主键）
 */
int CharacTowerDespairApc_Exists(DBConnectionManager* manager, const char* reg_date, int seq);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定日期的所有APC记录
 */
int CharacTowerDespairApc_GetByDate(DBConnectionManager* manager, const char* reg_date,
                                     CharacTowerDespairApc* records, int max_result);

/**
 * 获取指定角色的所有APC记录
 */
int CharacTowerDespairApc_GetByCharacNo(DBConnectionManager* manager, int charac_no,
                                         CharacTowerDespairApc* records, int max_result);

/**
 * 获取指定日期的记录总数
 */
int CharacTowerDespairApc_GetCountByDate(DBConnectionManager* manager, const char* reg_date);

/**
 * 获取指定角色的记录总数
 */
int CharacTowerDespairApc_GetCountByCharacNo(DBConnectionManager* manager, int charac_no);

/**
 * 获取指定日期的下一个可用序号
 */
int CharacTowerDespairApc_GetNextSeq(DBConnectionManager* manager, const char* reg_date);

/**
 * 删除指定日期的所有记录
 */
int CharacTowerDespairApc_DeleteByDate(DBConnectionManager* manager, const char* reg_date);

/**
 * 删除指定角色的所有记录
 */
int CharacTowerDespairApc_DeleteByCharacNo(DBConnectionManager* manager, int charac_no);

/* ==================== 工具函数 ==================== */

/**
 * 打印绝望之塔APC信息
 */
void CharacTowerDespairApc_PrintInfo(const CharacTowerDespairApc* record);

#endif /* CHARAC_TOWER_DESPAIR_APC_ORM_H */
