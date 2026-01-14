/**
 * DNF游戏服务器 - CharacItemStat表ORM头文件
 *
 * 功能: 角色装备属性数据管理（冷却时间、效果、检查标记）
 * 数据库: test_taiwan_cain
 * 表名: charac_item_stat
 * 复杂度: ⭐（4个字段，3个BLOB）
 */

#ifndef CHARAC_ITEM_STAT_ORM_H
#define CHARAC_ITEM_STAT_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 数据结构定义
 * ======================================================================== */

/**
 * CharacItemStat - 角色装备属性数据
 *
 * 字段说明:
 * - charac_no: 角色编号（主键）
 * - cooltime_item: 冷却时间道具数据（BLOB）
 * - effect_item: 效果道具数据（BLOB）
 * - check_flag: 检查标记数据（BLOB）
 */
typedef struct {
    int charac_no;                  /* 角色编号（主键） */

    /* BLOB数据字段 */
    unsigned char* cooltime_item;   /* 冷却时间道具数据 */
    int cooltime_item_len;          /* 冷却时间数据长度 */

    unsigned char* effect_item;     /* 效果道具数据 */
    int effect_item_len;            /* 效果数据长度 */

    unsigned char* check_flag;      /* 检查标记数据 */
    int check_flag_len;             /* 标记数据长度 */
} CharacItemStat;

/* ========================================================================
 * CRUD操作函数
 * ======================================================================== */

/**
 * 创建角色装备属性记录
 *
 * @param manager 数据库连接管理器
 * @param item_stat 装备属性数据结构指针
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_Create(DBConnectionManager* manager, CharacItemStat* item_stat);

/**
 * 根据角色编号查询装备属性
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param item_stat 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacItemStat* item_stat);

/**
 * 更新角色装备属性
 *
 * @param manager 数据库连接管理器
 * @param item_stat 装备属性数据结构指针
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_Update(DBConnectionManager* manager, CharacItemStat* item_stat);

/**
 * 删除角色装备属性记录
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_Delete(DBConnectionManager* manager, int charac_no);

/* ========================================================================
 * 辅助查询函数
 * ======================================================================== */

/**
 * 检查角色是否有装备属性记录
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回true，不存在返回false
 */
bool CharacItemStat_Exists(DBConnectionManager* manager, int charac_no);

/**
 * 获取指定BLOB字段的数据大小
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param field_name 字段名（cooltime_item, effect_item, check_flag）
 * @return 成功返回字节数，失败返回-1
 */
int CharacItemStat_GetBlobSize(DBConnectionManager* manager, int charac_no, const char* field_name);

/**
 * 获取装备属性记录总数
 *
 * @param manager 数据库连接管理器
 * @return 成功返回记录数，失败返回-1
 */
int CharacItemStat_GetCount(DBConnectionManager* manager);

/**
 * 批量查询有装备属性的角色编号列表
 *
 * @param manager 数据库连接管理器
 * @param charac_nos 输出参数，存储角色编号数组
 * @param max_count 最大查询数量
 * @return 成功返回实际数量，失败返回-1
 */
int CharacItemStat_GetAllCharacNos(DBConnectionManager* manager, int* charac_nos, int max_count);

/* ========================================================================
 * 工具函数
 * ======================================================================== */

/**
 * 初始化CharacItemStat结构体
 *
 * @param item_stat 结构体指针
 */
void CharacItemStat_Init(CharacItemStat* item_stat);

/**
 * 释放CharacItemStat结构体中的动态内存
 *
 * @param item_stat 结构体指针
 */
void CharacItemStat_Free(CharacItemStat* item_stat);

/**
 * 打印装备属性信息（用于调试）
 *
 * @param item_stat 结构体指针
 */
void CharacItemStat_Print(const CharacItemStat* item_stat);

/**
 * 分配BLOB字段的内存空间
 *
 * @param item_stat 结构体指针
 * @param field 字段标识（1=cooltime, 2=effect, 3=check_flag）
 * @param size 分配大小（字节）
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_AllocateBlob(CharacItemStat* item_stat, int field, int size);

/**
 * 复制装备属性数据
 *
 * @param dest 目标结构体指针
 * @param src 源结构体指针
 * @return 成功返回0，失败返回-1
 */
int CharacItemStat_Copy(CharacItemStat* dest, const CharacItemStat* src);

#ifdef __cplusplus
}
#endif

#endif /* CHARAC_ITEM_STAT_ORM_H */
