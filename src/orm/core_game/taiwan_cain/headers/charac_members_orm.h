#ifndef CHARAC_MEMBERS_ORM_H
#define CHARAC_MEMBERS_ORM_H

#include "db_connection_manager.h"

/**
 * 角色成员表 ORM
 *
 * 表: charac_members
 * 主键: charac_no
 * 功能: 记录角色成员信息（主仆关系、经验、时间管理）
 */

/* 数据结构 */
typedef struct {
    int charac_no;          /* 角色编号 (主键) */
    int master_no;          /* 主人编号 */
    int exp;                /* 经验值 */
    char create_time[20];   /* 创建时间 (YYYY-MM-DD HH:MM:SS) */
    char delete_time[20];   /* 删除时间 (YYYY-MM-DD HH:MM:SS) */
} CharacMembers;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色成员
 */
int CharacMembers_Add(DBConnectionManager* manager, const CharacMembers* record);

/**
 * 获取角色成员信息
 */
int CharacMembers_Get(DBConnectionManager* manager, int charac_no, CharacMembers* record);

/**
 * 更新角色成员信息
 */
int CharacMembers_Update(DBConnectionManager* manager, const CharacMembers* record);

/**
 * 删除角色成员
 */
int CharacMembers_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色成员是否存在
 */
int CharacMembers_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新主人编号
 */
int CharacMembers_UpdateMasterNo(DBConnectionManager* manager, int charac_no, int master_no);

/**
 * 增加经验值
 */
int CharacMembers_AddExp(DBConnectionManager* manager, int charac_no, int exp);

/**
 * 更新经验值
 */
int CharacMembers_UpdateExp(DBConnectionManager* manager, int charac_no, int exp);

/**
 * 更新删除时间
 */
int CharacMembers_UpdateDeleteTime(DBConnectionManager* manager, int charac_no, const char* delete_time);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定主人的所有成员
 */
int CharacMembers_GetByMasterNo(DBConnectionManager* manager, int master_no,
                                 int* charac_nos, int max_count);

/**
 * 获取主人的成员数量
 */
int CharacMembers_GetCountByMasterNo(DBConnectionManager* manager, int master_no);

/**
 * 获取总经验值
 */
long long CharacMembers_GetTotalExp(DBConnectionManager* manager);

/**
 * 获取活跃成员数量（未删除）
 */
int CharacMembers_GetActiveMembersCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色成员信息
 */
void CharacMembers_PrintInfo(const CharacMembers* record);

#endif /* CHARAC_MEMBERS_ORM_H */
