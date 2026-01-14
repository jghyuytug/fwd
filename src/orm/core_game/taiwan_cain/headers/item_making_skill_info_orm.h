#ifndef ITEM_MAKING_SKILL_INFO_ORM_H
#define ITEM_MAKING_SKILL_INFO_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Item Making Skill Info ORM - 物品制作技能系统
 *
 * 表: item_making_skill_info (taiwan_cain 数据库)
 * 描述: 角色物品制作技能等级管理表
 * 唯一标识: charac_no (角色编号)
 *
 * 字段说明:
 * - charac_no: 角色编号 (UNSIGNED INT, 主键)
 * - weapon: 武器制作等级 (UNSIGNED SMALLINT)
 * - cloth: 布甲制作等级 (UNSIGNED SMALLINT)
 * - leather: 皮甲制作等级 (UNSIGNED SMALLINT)
 * - light_armor: 轻甲制作等级 (UNSIGNED SMALLINT)
 * - heavy_armor: 重甲制作等级 (UNSIGNED SMALLINT)
 * - plate: 板甲制作等级 (UNSIGNED SMALLINT)
 * - amulet: 项链制作等级 (UNSIGNED SMALLINT)
 * - wrist: 手镯制作等级 (UNSIGNED SMALLINT)
 * - ring: 戒指制作等级 (UNSIGNED SMALLINT)
 * - support: 辅助装备制作等级 (UNSIGNED SMALLINT)
 * - magic_stone: 魔法石制作等级 (UNSIGNED SMALLINT)
 *
 * 业务用途:
 * - 记录角色在各类装备制作上的技能等级
 * - 支持装备制作系统，判断角色是否可以制作特定物品
 * - 制作技能升级管理
 *
 * 注意事项:
 * - 每个角色有唯一记录
 * - 技能等级默认为0（未学习）
 * - 11种制作技能类型涵盖所有装备类别
 *
 * API设计: 16个函数
 * - CRUD操作: 5个
 * - 专项操作: 5个 (技能升级、重置、按类型获取/设置等)
 * - 查询操作: 4个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-16
 * ORM模块编号: #101/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 物品制作技能信息记录结构 */
typedef struct {
    unsigned int charac_no;         /* 角色编号 (主键) */
    unsigned short weapon;          /* 武器制作等级 */
    unsigned short cloth;           /* 布甲制作等级 */
    unsigned short leather;         /* 皮甲制作等级 */
    unsigned short light_armor;     /* 轻甲制作等级 */
    unsigned short heavy_armor;     /* 重甲制作等级 */
    unsigned short plate;           /* 板甲制作等级 */
    unsigned short amulet;          /* 项链制作等级 */
    unsigned short wrist;           /* 手镯制作等级 */
    unsigned short ring;            /* 戒指制作等级 */
    unsigned short support;         /* 辅助装备制作等级 */
    unsigned short magic_stone;     /* 魔法石制作等级 */
} ItemMakingSkillInfo;

/* 制作技能类型枚举 */
typedef enum {
    MAKING_SKILL_WEAPON = 0,
    MAKING_SKILL_CLOTH = 1,
    MAKING_SKILL_LEATHER = 2,
    MAKING_SKILL_LIGHT_ARMOR = 3,
    MAKING_SKILL_HEAVY_ARMOR = 4,
    MAKING_SKILL_PLATE = 5,
    MAKING_SKILL_AMULET = 6,
    MAKING_SKILL_WRIST = 7,
    MAKING_SKILL_RING = 8,
    MAKING_SKILL_SUPPORT = 9,
    MAKING_SKILL_MAGIC_STONE = 10,
    MAKING_SKILL_COUNT = 11
} MakingSkillType;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新角色制作技能信息（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_Add(DBConnectionManager* manager, const ItemMakingSkillInfo* record);

/**
 * 获取指定角色的制作技能信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int ItemMakingSkillInfo_Get(DBConnectionManager* manager, int charac_no, ItemMakingSkillInfo* record);

/**
 * 更新角色制作技能信息
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_Update(DBConnectionManager* manager, const ItemMakingSkillInfo* record);

/**
 * 删除角色制作技能信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色制作技能信息是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int ItemMakingSkillInfo_Exists(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 专项操作 (5个函数)
 ******************************************************************************/

/**
 * 升级指定类型的制作技能
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param skill_type 技能类型
 * @param levels 升级级数
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_UpgradeSkill(DBConnectionManager* manager, int charac_no,
                                     MakingSkillType skill_type, unsigned short levels);

/**
 * 设置指定类型的制作技能等级
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param skill_type 技能类型
 * @param level 新等级
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_SetSkillLevel(DBConnectionManager* manager, int charac_no,
                                       MakingSkillType skill_type, unsigned short level);

/**
 * 获取指定类型的制作技能等级
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param skill_type 技能类型
 * @return >=0技能等级，<0失败
 */
int ItemMakingSkillInfo_GetSkillLevel(DBConnectionManager* manager, int charac_no,
                                       MakingSkillType skill_type);

/**
 * 重置所有制作技能等级为0
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_ResetAllSkills(DBConnectionManager* manager, int charac_no);

/**
 * 获取角色的最高制作技能等级
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return >=0最高等级，<0失败
 */
int ItemMakingSkillInfo_GetMaxSkillLevel(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取总记录数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int ItemMakingSkillInfo_Count(DBConnectionManager* manager);

/**
 * 获取所有记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ItemMakingSkillInfo_GetAll(DBConnectionManager* manager, ItemMakingSkillInfo* records, int max_records);

/**
 * 按技能等级范围查询
 * @param manager 数据库连接管理器
 * @param skill_type 技能类型
 * @param min_level 最小等级
 * @param max_level 最大等级
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ItemMakingSkillInfo_GetBySkillLevelRange(DBConnectionManager* manager, MakingSkillType skill_type,
                                              unsigned short min_level, unsigned short max_level,
                                              ItemMakingSkillInfo* records, int max_records);

/**
 * 获取拥有至少一种制作技能的角色数量
 * @param manager 数据库连接管理器
 * @return >=0角色数，<0失败
 */
int ItemMakingSkillInfo_CountActiveUsers(DBConnectionManager* manager);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除角色制作技能信息
 * @param manager 数据库连接管理器
 * @param charac_nos 角色编号数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int ItemMakingSkillInfo_BatchDelete(DBConnectionManager* manager, const int* charac_nos, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印物品制作技能信息（用于调试）
 * @param record 记录指针
 */
void ItemMakingSkillInfo_PrintInfo(const ItemMakingSkillInfo* record);

#endif /* ITEM_MAKING_SKILL_INFO_ORM_H */
