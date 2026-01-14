#ifndef CHARAC_TOWER_RANK_ORM_H
#define CHARAC_TOWER_RANK_ORM_H

#include "db_connection_manager.h"

/**
 * 角色塔排名 ORM
 *
 * 表: charac_tower_rank
 * 主键: (tower_index, part_type, charac_no) 复合主键
 * 功能: 塔排名记录（无尽之塔/绝望之塔等排行榜）
 */

/* 数据结构 */
typedef struct {
    unsigned char tower_index;          /* 塔索引 (主键) */
    char part_type;                     /* 类型 (主键) */
    int charac_no;                      /* 角色编号 (主键) */
    char member_info[129];              /* 成员信息 */
    unsigned short rank;                /* 排名 */
} CharacTowerRank;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新塔排名记录
 */
int CharacTowerRank_Add(DBConnectionManager* manager, const CharacTowerRank* record);

/**
 * 获取塔排名记录（通过三元复合主键）
 */
int CharacTowerRank_Get(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                         int charac_no, CharacTowerRank* record);

/**
 * 更新塔排名记录
 */
int CharacTowerRank_Update(DBConnectionManager* manager, const CharacTowerRank* record);

/**
 * 删除塔排名记录（通过三元复合主键）
 */
int CharacTowerRank_Delete(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/**
 * 检查塔排名记录是否存在（通过三元复合主键）
 */
int CharacTowerRank_Exists(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定塔和类型的所有排名记录（按排名升序）
 */
int CharacTowerRank_GetByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                       CharacTowerRank* records, int max_result);

/**
 * 获取指定塔和类型的前N名记录
 */
int CharacTowerRank_GetTopN(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                             int limit, CharacTowerRank* records);

/**
 * 获取指定角色的所有塔排名记录
 */
int CharacTowerRank_GetByCharacNo(DBConnectionManager* manager, int charac_no,
                                   CharacTowerRank* records, int max_result);

/**
 * 获取指定塔的记录总数
 */
int CharacTowerRank_GetCountByTower(DBConnectionManager* manager, unsigned char tower_index, char part_type);

/**
 * 获取指定角色的排名（在指定塔和类型中）
 */
int CharacTowerRank_GetRank(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no);

/**
 * 更新成员信息
 */
int CharacTowerRank_UpdateMemberInfo(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                      int charac_no, const char* member_info);

/**
 * 删除指定塔和类型的所有记录
 */
int CharacTowerRank_DeleteByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type);

/**
 * 删除指定角色的所有记录
 */
int CharacTowerRank_DeleteByCharacNo(DBConnectionManager* manager, int charac_no);

/* ==================== 工具函数 ==================== */

/**
 * 打印塔排名信息
 */
void CharacTowerRank_PrintInfo(const CharacTowerRank* record);

#endif /* CHARAC_TOWER_RANK_ORM_H */
