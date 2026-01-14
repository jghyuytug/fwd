#ifndef CHARAC_TOWER_RECORD_ORM_H
#define CHARAC_TOWER_RECORD_ORM_H

#include "db_connection_manager.h"

/**
 * 角色塔记录 ORM
 *
 * 表: charac_tower_record
 * 主键: charac_no
 * 功能: 塔挑战记录（1-4人不同队伍规模的最佳记录）
 */

/* 数据结构 */
typedef struct {
    int charac_no;                      /* 角色编号 (主键) */
    unsigned char tower_index;          /* 塔索引 */

    /* 1人记录 */
    char member_info_1[33];             /* 成员信息 1人 */
    unsigned char stage_1;              /* 阶段 1人 */
    unsigned int play_time_1;           /* 游戏时间 1人 (秒) */
    char occ_time_1[20];                /* 完成时间 1人 */

    /* 2人记录 */
    char member_info_2[65];             /* 成员信息 2人 */
    unsigned char stage_2;              /* 阶段 2人 */
    unsigned int play_time_2;           /* 游戏时间 2人 (秒) */
    char occ_time_2[20];                /* 完成时间 2人 */

    /* 3人记录 */
    char member_info_3[97];             /* 成员信息 3人 */
    unsigned char stage_3;              /* 阶段 3人 */
    unsigned int play_time_3;           /* 游戏时间 3人 (秒) */
    char occ_time_3[20];                /* 完成时间 3人 */

    /* 4人记录 */
    char member_info_4[129];            /* 成员信息 4人 */
    unsigned char stage_4;              /* 阶段 4人 */
    unsigned int play_time_4;           /* 游戏时间 4人 (秒) */
    char occ_time_4[20];                /* 完成时间 4人 */
} CharacTowerRecord;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新塔记录
 */
int CharacTowerRecord_Add(DBConnectionManager* manager, const CharacTowerRecord* record);

/**
 * 获取塔记录
 */
int CharacTowerRecord_Get(DBConnectionManager* manager, int charac_no, CharacTowerRecord* record);

/**
 * 更新塔记录
 */
int CharacTowerRecord_Update(DBConnectionManager* manager, const CharacTowerRecord* record);

/**
 * 删除塔记录
 */
int CharacTowerRecord_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查塔记录是否存在
 */
int CharacTowerRecord_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新指定人数规模的记录（1-4人）
 * @param member_size 队伍人数 (1-4)
 */
int CharacTowerRecord_UpdateMemberRecord(DBConnectionManager* manager, int charac_no, int member_size,
                                          const char* member_info, unsigned char stage,
                                          unsigned int play_time, const char* occ_time);

/**
 * 更新指定人数规模的成员信息
 */
int CharacTowerRecord_UpdateMemberInfo(DBConnectionManager* manager, int charac_no, int member_size,
                                        const char* member_info);

/**
 * 更新指定人数规模的阶段
 */
int CharacTowerRecord_UpdateStage(DBConnectionManager* manager, int charac_no, int member_size,
                                   unsigned char stage);

/**
 * 更新指定人数规模的游戏时间
 */
int CharacTowerRecord_UpdatePlayTime(DBConnectionManager* manager, int charac_no, int member_size,
                                      unsigned int play_time);

/**
 * 更新塔索引
 */
int CharacTowerRecord_UpdateTowerIndex(DBConnectionManager* manager, int charac_no, unsigned char tower_index);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定塔索引的所有记录
 */
int CharacTowerRecord_GetByTowerIndex(DBConnectionManager* manager, unsigned char tower_index,
                                       CharacTowerRecord* records, int max_result);

/**
 * 获取指定人数规模最快通关记录（按游戏时间排序）
 * @param member_size 队伍人数 (1-4)
 */
int CharacTowerRecord_GetFastestByMemberSize(DBConnectionManager* manager, int member_size, int limit,
                                               CharacTowerRecord* records);

/**
 * 获取指定塔的记录总数
 */
int CharacTowerRecord_GetCountByTowerIndex(DBConnectionManager* manager, unsigned char tower_index);

/**
 * 删除指定塔索引的所有记录
 */
int CharacTowerRecord_DeleteByTowerIndex(DBConnectionManager* manager, unsigned char tower_index);

/* ==================== 工具函数 ==================== */

/**
 * 打印塔记录信息
 */
void CharacTowerRecord_PrintInfo(const CharacTowerRecord* record);

#endif /* CHARAC_TOWER_RECORD_ORM_H */
