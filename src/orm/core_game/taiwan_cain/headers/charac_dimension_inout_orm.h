#ifndef CHARAC_DIMENSION_INOUT_ORM_H
#define CHARAC_DIMENSION_INOUT_ORM_H

#include "db_connection_manager.h"

/**
 * 角色维度副本进出记录表 ORM
 *
 * 表: charac_dimension_inout
 * 主键: charac_no
 * 功能: 记录角色对10个维度副本的解锁/通关状态
 */

#define DIMENSION_DUNGEON_COUNT 10

/* 数据结构 */
typedef struct {
    int charac_no;                          /* 角色编号 (主键) */
    char dungeon_status[DIMENSION_DUNGEON_COUNT]; /* 副本状态数组 (dungeon1~dungeon10) */
} CharacDimensionInout;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色维度副本进出记录
 */
int CharacDimensionInout_Add(DBConnectionManager* manager, const CharacDimensionInout* record);

/**
 * 获取角色维度副本进出记录
 */
int CharacDimensionInout_Get(DBConnectionManager* manager, int charac_no, CharacDimensionInout* record);

/**
 * 更新角色维度副本进出记录
 */
int CharacDimensionInout_Update(DBConnectionManager* manager, const CharacDimensionInout* record);

/**
 * 删除角色维度副本进出记录
 */
int CharacDimensionInout_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色维度副本进出记录是否存在
 */
int CharacDimensionInout_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 副本状态操作 ==================== */

/**
 * 获取指定副本的状态
 * @param dungeon_index 副本索引 (1-10)
 * @return 副本状态值，失败返回-1
 */
int CharacDimensionInout_GetDungeonStatus(DBConnectionManager* manager, int charac_no, int dungeon_index);

/**
 * 更新指定副本的状态
 * @param dungeon_index 副本索引 (1-10)
 * @param status 新状态值
 */
int CharacDimensionInout_UpdateDungeonStatus(DBConnectionManager* manager, int charac_no,
                                              int dungeon_index, char status);

/**
 * 获取已解锁副本列表
 * @param unlocked_dungeons 输出数组，存储已解锁的副本索引 (1-10)
 * @param max_count 数组最大容量
 * @return 已解锁副本数量
 */
int CharacDimensionInout_GetUnlockedDungeons(DBConnectionManager* manager, int charac_no,
                                              int* unlocked_dungeons, int max_count);

/**
 * 解锁指定副本（设置状态为1）
 * @param dungeon_index 副本索引 (1-10)
 */
int CharacDimensionInout_UnlockDungeon(DBConnectionManager* manager, int charac_no, int dungeon_index);

/**
 * 批量设置所有副本状态
 * @param status 要设置的状态值
 */
int CharacDimensionInout_SetAllDungeonStatus(DBConnectionManager* manager, int charac_no, char status);

/* ==================== 统计操作 ==================== */

/**
 * 统计已解锁副本数量（状态值 > 0）
 */
int CharacDimensionInout_CountUnlockedDungeons(DBConnectionManager* manager, int charac_no);

/**
 * 统计指定状态的副本数量
 */
int CharacDimensionInout_CountDungeonsByStatus(DBConnectionManager* manager, int charac_no, char status);

/**
 * 检查是否所有副本都已解锁
 * @return 1=全部解锁, 0=未全部解锁, -1=错误
 */
int CharacDimensionInout_IsAllUnlocked(DBConnectionManager* manager, int charac_no);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色维度副本进出记录信息
 */
void CharacDimensionInout_PrintInfo(const CharacDimensionInout* record);

#endif /* CHARAC_DIMENSION_INOUT_ORM_H */
