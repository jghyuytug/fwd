#ifndef CHARAC_HOUSING_TREE_INFO_ORM_H
#define CHARAC_HOUSING_TREE_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * 角色住房树信息表 ORM
 *
 * 表: charac_housing_tree_info
 * 主键: charac_no
 * 功能: 记录角色住房种植树的信息，包括树ID、过期时间、点数、浇水次数等
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;          /* 角色编号 (主键) */
    unsigned int tree_id;            /* 树ID */
    char expire_date[20];            /* 过期时间 YYYY-MM-DD HH:MM:SS */
    short current_point;             /* 当前点数 */
    short leaf_point;                /* 叶子点数 */
    short day_water_count;           /* 每日浇水次数 */
} CharacHousingTreeInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色住房树信息
 */
int CharacHousingTreeInfo_Add(DBConnectionManager* manager, const CharacHousingTreeInfo* record);

/**
 * 获取角色住房树信息
 */
int CharacHousingTreeInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacHousingTreeInfo* record);

/**
 * 更新角色住房树信息
 */
int CharacHousingTreeInfo_Update(DBConnectionManager* manager, const CharacHousingTreeInfo* record);

/**
 * 删除角色住房树信息
 */
int CharacHousingTreeInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色住房树信息是否存在
 */
int CharacHousingTreeInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 树信息操作 ==================== */

/**
 * 更新树的当前点数
 * @param points 点数增量（可为负数）
 */
int CharacHousingTreeInfo_UpdatePoints(DBConnectionManager* manager, unsigned int charac_no, short points);

/**
 * 更新叶子点数
 * @param points 点数增量（可为负数）
 */
int CharacHousingTreeInfo_UpdateLeafPoints(DBConnectionManager* manager, unsigned int charac_no, short points);

/**
 * 增加每日浇水次数
 */
int CharacHousingTreeInfo_IncrementWaterCount(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 重置每日浇水次数
 */
int CharacHousingTreeInfo_ResetWaterCount(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 更新树的过期时间
 */
int CharacHousingTreeInfo_UpdateExpireDate(DBConnectionManager* manager, unsigned int charac_no, const char* expire_date);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定树ID的所有记录数量
 */
int CharacHousingTreeInfo_CountByTreeId(DBConnectionManager* manager, unsigned int tree_id);

/**
 * 检查树是否已过期
 * @return 1=已过期, 0=未过期, -1=错误
 */
int CharacHousingTreeInfo_IsExpired(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 获取即将过期的树列表（未来N天内过期）
 * @param days 天数
 * @param charac_nos 输出数组，存储角色编号
 * @param max_count 数组最大容量
 * @return 找到的记录数量
 */
int CharacHousingTreeInfo_GetExpiringSoon(DBConnectionManager* manager, int days,
                                           unsigned int* charac_nos, int max_count);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色住房树信息
 */
void CharacHousingTreeInfo_PrintInfo(const CharacHousingTreeInfo* record);

#endif /* CHARAC_HOUSING_TREE_INFO_ORM_H */
