#ifndef CHARAC_BLOOD_INOUT_ORM_H
#define CHARAC_BLOOD_INOUT_ORM_H

#include "db_connection_manager.h"

/**
 * 角色血色副本进出记录表 ORM
 *
 * 表: charac_blood_inout
 * 主键: charac_no
 * 功能: 记录角色对10个血色副本的解锁/通关状态
 */

#define BLOOD_DUNGEON_COUNT 10

/* 数据结构 */
typedef struct {
    int charac_no;                          /* 角色编号 (主键) */
    char dungeon_status[BLOOD_DUNGEON_COUNT]; /* 副本状态数组 (dungeon1~dungeon10) */
} CharacBloodInout;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色血色副本进出记录
 */
int CharacBloodInout_Add(DBConnectionManager* manager, const CharacBloodInout* record);

/**
 * 获取角色血色副本进出记录
 */
int CharacBloodInout_Get(DBConnectionManager* manager, int charac_no, CharacBloodInout* record);

/**
 * 更新角色血色副本进出记录
 */
int CharacBloodInout_Update(DBConnectionManager* manager, const CharacBloodInout* record);

/**
 * 删除角色血色副本进出记录
 */
int CharacBloodInout_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色血色副本进出记录是否存在
 */
int CharacBloodInout_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 副本状态操作 ==================== */

/**
 * 获取指定副本的状态
 * @param dungeon_index 副本索引 (1-10)
 * @return 副本状态值，失败返回-1
 */
int CharacBloodInout_GetDungeonStatus(DBConnectionManager* manager, int charac_no, int dungeon_index);

/**
 * 更新指定副本的状态
 * @param dungeon_index 副本索引 (1-10)
 * @param status 新状态值
 */
int CharacBloodInout_UpdateDungeonStatus(DBConnectionManager* manager, int charac_no,
                                          int dungeon_index, char status);

/**
 * 获取已解锁副本列表
 * @param unlocked_dungeons 输出数组，存储已解锁的副本索引 (1-10)
 * @param max_count 数组最大容量
 * @return 已解锁副本数量
 */
int CharacBloodInout_GetUnlockedDungeons(DBConnectionManager* manager, int charac_no,
                                          int* unlocked_dungeons, int max_count);

/**
 * 解锁指定副本（设置状态为1）
 * @param dungeon_index 副本索引 (1-10)
 */
int CharacBloodInout_UnlockDungeon(DBConnectionManager* manager, int charac_no, int dungeon_index);

/**
 * 批量设置所有副本状态
 * @param status 要设置的状态值
 */
int CharacBloodInout_SetAllDungeonStatus(DBConnectionManager* manager, int charac_no, char status);

/* ==================== 统计操作 ==================== */

/**
 * 统计已解锁副本数量（状态值 > 0）
 */
int CharacBloodInout_CountUnlockedDungeons(DBConnectionManager* manager, int charac_no);

/**
 * 统计指定状态的副本数量
 */
int CharacBloodInout_CountDungeonsByStatus(DBConnectionManager* manager, int charac_no, char status);

/**
 * 检查是否所有副本都已解锁
 * @return 1=全部解锁, 0=未全部解锁, -1=错误
 */
int CharacBloodInout_IsAllUnlocked(DBConnectionManager* manager, int charac_no);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色血色副本进出记录信息
 */
void CharacBloodInout_PrintInfo(const CharacBloodInout* record);

#endif /* CHARAC_BLOOD_INOUT_ORM_H */
