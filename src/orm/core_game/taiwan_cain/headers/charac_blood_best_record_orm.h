#ifndef CHARAC_BLOOD_BEST_RECORD_ORM_H
#define CHARAC_BLOOD_BEST_RECORD_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Blood Best Record ORM - 角色血色副本最佳记录系统
 *
 * 表: charac_blood_best_record (taiwan_cain 数据库)
 * 描述: 角色血色副本最佳通关记录表
 * 主键: (charac_no, dungeon_index) 复合主键
 *
 * 字段说明:
 * - charac_no: 角色编号 (INT UNSIGNED, PRIMARY KEY)
 * - dungeon_index: 副本索引 (INT UNSIGNED, PRIMARY KEY)
 * - best_round: 最佳回合数 (TINYINT UNSIGNED)
 * - best_time: 最佳通关时间(毫秒) (INT)
 *
 * 业务用途:
 * - 记录保存：存储玩家在各个血色副本的最佳成绩
 * - 成绩查询：查询玩家在特定副本的最佳记录
 * - 排行榜：支持按时间或回合数排序的排行功能
 * - 成就系统：判断玩家是否刷新个人最佳记录
 *
 * API设计: 13个函数
 * - CRUD操作: 5个
 * - 查询操作: 5个（按角色/按副本/排行榜/成绩更新）
 * - 统计操作: 2个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #69/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 角色血色副本最佳记录 */
typedef struct {
    unsigned int charac_no;         /* 角色编号 (复合主键1) */
    unsigned int dungeon_index;     /* 副本索引 (复合主键2) */
    unsigned char best_round;       /* 最佳回合数 */
    int best_time;                  /* 最佳通关时间(毫秒) */
} CharacBloodBestRecord;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

int CharacBloodBestRecord_Add(DBConnectionManager* manager, const CharacBloodBestRecord* record);
int CharacBloodBestRecord_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index,
                               CharacBloodBestRecord* record);
int CharacBloodBestRecord_Update(DBConnectionManager* manager, const CharacBloodBestRecord* record);
int CharacBloodBestRecord_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index);
int CharacBloodBestRecord_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned int dungeon_index);

/*******************************************************************************
 * 查询操作 (5个函数)
 ******************************************************************************/

int CharacBloodBestRecord_GetByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                      CharacBloodBestRecord* records, int max_records);
int CharacBloodBestRecord_GetByDungeon(DBConnectionManager* manager, unsigned int dungeon_index,
                                       CharacBloodBestRecord* records, int max_records);
int CharacBloodBestRecord_GetTopByTime(DBConnectionManager* manager, unsigned int dungeon_index, int limit,
                                       CharacBloodBestRecord* records, int max_records);
int CharacBloodBestRecord_GetTopByRound(DBConnectionManager* manager, unsigned int dungeon_index, int limit,
                                        CharacBloodBestRecord* records, int max_records);
int CharacBloodBestRecord_UpdateIfBetter(DBConnectionManager* manager, const CharacBloodBestRecord* record);

/*******************************************************************************
 * 统计操作 (2个函数)
 ******************************************************************************/

int CharacBloodBestRecord_CountByCharac(DBConnectionManager* manager, unsigned int charac_no);
int CharacBloodBestRecord_CountByDungeon(DBConnectionManager* manager, unsigned int dungeon_index);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

void CharacBloodBestRecord_PrintInfo(const CharacBloodBestRecord* record);

#endif /* CHARAC_BLOOD_BEST_RECORD_ORM_H */
