#ifndef DNF_EVENT_ENTRY_NOTUSE_ORM_H
#define DNF_EVENT_ENTRY_NOTUSE_ORM_H

#include "db_connection_manager.h"

/**
 * @file dnf_event_entry_notuse_orm.h
 * @brief DNF活动入口表（未使用）ORM 接口
 *
 * 数据库: d_taiwan
 * 表名: dnf_event_entry_notuse
 * 用途: 记录玩家参与活动的入口信息（该表标记为未使用）
 */

/* 数据结构定义 */
typedef struct {
    int event_id;           /* 活动ID (复合主键1) */
    int m_id;               /* 账户ID (复合主键2) */
    char occ_date[20];      /* 发生日期时间 (datetime) */
    signed char server_id;  /* 服务器ID (tinyint) */
    int charac_no;          /* 角色编号 */
    char obtain_date[20];   /* 获得日期时间 (datetime) */
} DnfEventEntryNotuse;

/* CRUD 操作 */

/**
 * @brief 添加或更新活动入口记录
 * @param manager 数据库连接管理器
 * @param record 活动入口记录指针
 * @return 0=成功, <0=失败
 */
int DnfEventEntryNotuse_Add(DBConnectionManager* manager, const DnfEventEntryNotuse* record);

/**
 * @brief 获取活动入口记录
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 账户ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, <0=失败
 */
int DnfEventEntryNotuse_Get(DBConnectionManager* manager, int event_id, int m_id,
                            DnfEventEntryNotuse* record);

/**
 * @brief 更新活动入口记录
 * @param manager 数据库连接管理器
 * @param record 活动入口记录指针
 * @return 0=成功, <0=失败
 */
int DnfEventEntryNotuse_Update(DBConnectionManager* manager, const DnfEventEntryNotuse* record);

/**
 * @brief 删除活动入口记录
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 账户ID
 * @return 0=成功, <0=失败
 */
int DnfEventEntryNotuse_Delete(DBConnectionManager* manager, int event_id, int m_id);

/**
 * @brief 检查活动入口记录是否存在
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 账户ID
 * @return 1=存在, 0=不存在, <0=错误
 */
int DnfEventEntryNotuse_Exists(DBConnectionManager* manager, int event_id, int m_id);

/* 业务操作 */

/**
 * @brief 获取账户参与的所有活动数量
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @return 活动数量, <0=错误
 */
int DnfEventEntryNotuse_GetCountByAccount(DBConnectionManager* manager, int m_id);

/**
 * @brief 获取特定活动的参与人数
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @return 参与人数, <0=错误
 */
int DnfEventEntryNotuse_GetCountByEvent(DBConnectionManager* manager, int event_id);

/**
 * @brief 获取角色参与的活动数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 活动数量, <0=错误
 */
int DnfEventEntryNotuse_GetCountByCharacter(DBConnectionManager* manager, int charac_no);

/**
 * @brief 检查账户是否参与了特定活动
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 账户ID
 * @return 1=已参与, 0=未参与, <0=错误
 */
int DnfEventEntryNotuse_HasAccountJoinedEvent(DBConnectionManager* manager, int event_id, int m_id);

/**
 * @brief 删除特定活动的所有记录
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @return 删除的记录数, <0=错误
 */
int DnfEventEntryNotuse_DeleteByEvent(DBConnectionManager* manager, int event_id);

/**
 * @brief 删除特定账户的所有活动记录
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @return 删除的记录数, <0=错误
 */
int DnfEventEntryNotuse_DeleteByAccount(DBConnectionManager* manager, int m_id);

/* 工具函数 */

/**
 * @brief 打印活动入口记录信息（调试用）
 * @param record 活动入口记录指针
 */
void DnfEventEntryNotuse_PrintInfo(const DnfEventEntryNotuse* record);

#endif /* DNF_EVENT_ENTRY_NOTUSE_ORM_H */
