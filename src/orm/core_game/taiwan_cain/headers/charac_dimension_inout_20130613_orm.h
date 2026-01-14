#ifndef CHARAC_DIMENSION_INOUT_20130613_ORM_H
#define CHARAC_DIMENSION_INOUT_20130613_ORM_H

#include "db_connection_manager.h"

/**
 * charac_dimension_inout_20130613 - 角色维度副本进出状态记录（2013-06-13历史版本）
 *
 * 表结构：
 * - charac_no (int): 角色编号（主键）
 * - dungeon1-10 (tinyint): 10个维度副本的进入状态（0=未进入，1=已进入）
 *
 * 业务说明：
 * - 记录角色在10个维度副本的进入状态
 * - 2013年6月13日版本，可能已废弃或作为历史数据
 * - 用于副本通关统计、奖励发放
 */

/* 维度副本编号常量 */
#define DIMENSION_DUNGEON_COUNT 10
#define DIMENSION_DUNGEON_MIN 1
#define DIMENSION_DUNGEON_MAX 10

/* 副本状态值 */
#define DUNGEON_STATUS_NOT_ENTERED 0
#define DUNGEON_STATUS_ENTERED 1

/* 数据结构定义 */
typedef struct {
    unsigned int charac_no;       /* 角色编号（主键） */
    char dungeon1;                /* 维度副本1状态 */
    char dungeon2;                /* 维度副本2状态 */
    char dungeon3;                /* 维度副本3状态 */
    char dungeon4;                /* 维度副本4状态 */
    char dungeon5;                /* 维度副本5状态 */
    char dungeon6;                /* 维度副本6状态 */
    char dungeon7;                /* 维度副本7状态 */
    char dungeon8;                /* 维度副本8状态 */
    char dungeon9;                /* 维度副本9状态 */
    char dungeon10;               /* 维度副本10状态 */
} CharacDimensionInout20130613;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色维度副本进入状态记录
 * @param manager 数据库连接管理器
 * @param record 记录数据
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_Add(DBConnectionManager* manager, const CharacDimensionInout20130613* record);

/**
 * 根据角色编号查询维度副本进入状态
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_Get(DBConnectionManager* manager, unsigned int charac_no, CharacDimensionInout20130613* record);

/**
 * 更新角色维度副本进入状态记录
 * @param manager 数据库连接管理器
 * @param record 记录数据
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_Update(DBConnectionManager* manager, const CharacDimensionInout20130613* record);

/**
 * 删除角色维度副本进入状态记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色是否有维度副本进入状态记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 1=存在, 0=不存在, <0=错误
 */
int CharacDimensionInout20130613_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 设置指定维度副本的进入状态
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_index 副本索引（1-10）
 * @param status 状态（0=未进入，1=已进入）
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_SetDungeonStatus(DBConnectionManager* manager, unsigned int charac_no, int dungeon_index, char status);

/**
 * 获取指定维度副本的进入状态
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_index 副本索引（1-10）
 * @param status 输出状态（调用者提供）
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_GetDungeonStatus(DBConnectionManager* manager, unsigned int charac_no, int dungeon_index, char* status);

/**
 * 设置所有维度副本的进入状态
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param status 状态（0=全部未进入，1=全部已进入）
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_SetAllDungeons(DBConnectionManager* manager, unsigned int charac_no, char status);

/**
 * 清空角色所有维度副本状态（全部设为0）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_ClearAllDungeons(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 统计角色已完成的维度副本数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param count 输出已完成数量（调用者提供）
 * @return 0=成功, <0=失败
 */
int CharacDimensionInout20130613_GetCompletedCount(DBConnectionManager* manager, unsigned int charac_no, int* count);

/* ==================== 查询操作 ==================== */

/**
 * 按已完成副本数量统计角色数
 * @param manager 数据库连接管理器
 * @param completed_count 已完成副本数量（0-10）
 * @return 角色数量, <0=错误
 */
int CharacDimensionInout20130613_CountByCompletedDungeons(DBConnectionManager* manager, int completed_count);

/**
 * 按指定维度副本状态查询角色数量
 * @param manager 数据库连接管理器
 * @param dungeon_index 副本索引（1-10）
 * @param status 状态（0=未进入，1=已进入）
 * @return 角色数量, <0=错误
 */
int CharacDimensionInout20130613_CountByDungeonStatus(DBConnectionManager* manager, int dungeon_index, char status);

/**
 * 统计总记录数
 * @param manager 数据库连接管理器
 * @return 记录数, <0=错误
 */
int CharacDimensionInout20130613_CountTotal(DBConnectionManager* manager);

/**
 * 统计完成所有副本的角色数量
 * @param manager 数据库连接管理器
 * @return 角色数量, <0=错误
 */
int CharacDimensionInout20130613_CountFullyCompleted(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印记录信息（调试用）
 * @param record 记录数据
 */
void CharacDimensionInout20130613_PrintInfo(const CharacDimensionInout20130613* record);

#endif /* CHARAC_DIMENSION_INOUT_20130613_ORM_H */
