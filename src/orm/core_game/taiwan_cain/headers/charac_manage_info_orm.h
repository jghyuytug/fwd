#ifndef CHARAC_MANAGE_INFO_ORM_H
#define CHARAC_MANAGE_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * 角色管理信息表 ORM
 *
 * 表: charac_manage_info
 * 主键: charac_no
 * 功能: 记录角色管理相关信息（标记角色、技能索引、装备等级）
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;          /* 角色编号 (主键) */
    unsigned int tag_charac_no;      /* 标记角色编号 */
    unsigned char striker_skill_index; /* 打击技能索引 */
    unsigned short max_equip_level;  /* 最大装备等级 */
} CharacManageInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色管理信息
 */
int CharacManageInfo_Add(DBConnectionManager* manager, const CharacManageInfo* record);

/**
 * 获取角色管理信息
 */
int CharacManageInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacManageInfo* record);

/**
 * 更新角色管理信息
 */
int CharacManageInfo_Update(DBConnectionManager* manager, const CharacManageInfo* record);

/**
 * 删除角色管理信息
 */
int CharacManageInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色管理信息是否存在
 */
int CharacManageInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新标记角色编号
 */
int CharacManageInfo_UpdateTagCharacNo(DBConnectionManager* manager, unsigned int charac_no, unsigned int tag_charac_no);

/**
 * 更新打击技能索引
 */
int CharacManageInfo_UpdateStrikerSkillIndex(DBConnectionManager* manager, unsigned int charac_no, unsigned char skill_index);

/**
 * 更新最大装备等级
 */
int CharacManageInfo_UpdateMaxEquipLevel(DBConnectionManager* manager, unsigned int charac_no, unsigned short max_level);

/**
 * 获取指定标记角色的所有角色
 */
int CharacManageInfo_GetByTagCharacNo(DBConnectionManager* manager, unsigned int tag_charac_no,
                                       unsigned int* charac_nos, int max_count);

/**
 * 获取指定装备等级范围的角色数量
 */
int CharacManageInfo_GetCountByEquipLevelRange(DBConnectionManager* manager,
                                                 unsigned short min_level, unsigned short max_level);

/**
 * 获取最高装备等级
 */
unsigned short CharacManageInfo_GetMaxEquipLevel(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色管理信息
 */
void CharacManageInfo_PrintInfo(const CharacManageInfo* record);

#endif /* CHARAC_MANAGE_INFO_ORM_H */
