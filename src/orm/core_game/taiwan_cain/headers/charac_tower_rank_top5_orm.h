#ifndef CHARAC_TOWER_RANK_TOP5_ORM_H
#define CHARAC_TOWER_RANK_TOP5_ORM_H

#include "db_connection_manager.h"

/**
 * 角色塔排名TOP5 ORM
 *
 * 表: charac_tower_rank_top5
 * 主键: (tower_index, part_type, charac_no) 复合主键
 * 功能: 塔排名TOP5榜单（无尽之塔/绝望之塔等前5名排行榜）
 */

/* 数据结构 */
typedef struct {
    unsigned char tower_index;          /* 塔索引 (主键) */
    char part_type;                     /* 类型 (主键) */
    int charac_no;                      /* 角色编号 (主键) */
    char member_info[129];              /* 成员信息 */
    unsigned short rank;                /* 排名 */
} CharacTowerRankTop5;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新塔排名TOP5记录
 */
int CharacTowerRankTop5_Add(DBConnectionManager* manager, const CharacTowerRankTop5* record);

/**
 * 获取塔排名TOP5记录（通过三元复合主键）
 */
int CharacTowerRankTop5_Get(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                         int charac_no, CharacTowerRankTop5* record);

/**
 * 更新塔排名TOP5记录
 */
int CharacTowerRankTop5_Update(DBConnectionManager* manager, const CharacTowerRankTop5* record);

/**
 * 删除塔排名TOP5记录（通过三元复合主键）
 */
int CharacTowerRankTop5_Delete(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/**
 * 检查塔排名TOP5记录是否存在（通过三元复合主键）
 */
int CharacTowerRankTop5_Exists(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定塔和类型的所有TOP5排名记录（按排名升序）
 */
int CharacTowerRankTop5_GetByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                       CharacTowerRankTop5* records, int max_result);

/**
 * 获取指定塔和类型的前N名记录
 */
int CharacTowerRankTop5_GetTopN(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                             int limit, CharacTowerRankTop5* records);

/**
 * 获取指定角色的所有塔排名TOP5记录
 */
int CharacTowerRankTop5_GetByCharacNo(DBConnectionManager* manager, int charac_no,
                                   CharacTowerRankTop5* records, int max_result);

/**
 * 获取指定塔的TOP5记录总数
 */
int CharacTowerRankTop5_GetCountByTower(DBConnectionManager* manager, unsigned char tower_index, char part_type);

/**
 * 获取指定角色的排名（在指定塔和类型中）
 */
int CharacTowerRankTop5_GetRank(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/**
 * 更新成员信息
 */
int CharacTowerRankTop5_UpdateMemberInfo(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                      int charac_no, const char* member_info);

/**
 * 删除指定塔和类型的所有TOP5记录
 */
int CharacTowerRankTop5_DeleteByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type);

/**
 * 删除指定角色的所有TOP5记录
 */
int CharacTowerRankTop5_DeleteByCharacNo(DBConnectionManager* manager, int charac_no);

/* ==================== 工具函数 ==================== */

/**
 * 打印塔排名TOP5信息
 */
void CharacTowerRankTop5_PrintInfo(const CharacTowerRankTop5* record);

#endif /* CHARAC_TOWER_RANK_TOP5_ORM_H */
