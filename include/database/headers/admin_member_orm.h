/**
 * DNF游戏服务器 - AdminMember表ORM层
 *
 * 功能: 管理员账号管理系统
 * 数据库: test_d_taiwan
 * 表: admin_member
 */

#ifndef ADMIN_MEMBER_ORM_H
#define ADMIN_MEMBER_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// 管理员结构定义
// ========================================================================

/**
 * 管理员等级定义
 */
typedef enum {
    ADMIN_LEVEL_NONE = 0,           // 非管理员
    ADMIN_LEVEL_MODERATOR = 1,      // 版主 (警告/踢人)
    ADMIN_LEVEL_GM = 2,             // GM (常规管理)
    ADMIN_LEVEL_SENIOR_GM = 3,      // 高级GM (物品管理)
    ADMIN_LEVEL_ADMIN = 4,          // 管理员 (账号管理)
    ADMIN_LEVEL_SUPER_ADMIN = 5,    // 超级管理员 (全部权限)
    ADMIN_LEVEL_SYSTEM = 9          // 系统管理员 (最高权限)
} AdminLevel;

/**
 * 管理员信息结构
 */
typedef struct {
    int m_id;                       // 账号ID (主键, 外键关联accounts表)
    int admin_level;                // 管理员等级 (0-9)
    char create_time[20];           // 创建时间 (YYYY-MM-DD HH:MM:SS)
    char update_time[20];           // 更新时间 (YYYY-MM-DD HH:MM:SS)
} AdminMember;

// ========================================================================
// CRUD操作函数
// ========================================================================

/**
 * 创建管理员记录
 *
 * @param manager 数据库连接管理器
 * @param admin 管理员信息结构
 * @return 0=成功, -1=失败
 */
int AdminMember_Create(DBConnectionManager* manager, AdminMember* admin);

/**
 * 根据账号ID获取管理员信息
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @param admin 输出参数，管理员信息
 * @return 0=成功, -1=失败（不存在或错误）
 */
int AdminMember_GetByID(DBConnectionManager* manager, int m_id, AdminMember* admin);

/**
 * 更新管理员信息（完整更新）
 *
 * @param manager 数据库连接管理器
 * @param admin 管理员信息结构
 * @return 0=成功, -1=失败
 */
int AdminMember_Update(DBConnectionManager* manager, AdminMember* admin);

/**
 * 更新管理员等级
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @param new_level 新的管理员等级
 * @return 0=成功, -1=失败
 */
int AdminMember_UpdateLevel(DBConnectionManager* manager, int m_id, int new_level);

/**
 * 删除管理员记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @return 0=成功, -1=失败
 */
int AdminMember_Delete(DBConnectionManager* manager, int m_id);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查账号是否为管理员
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @return true=是管理员, false=不是
 */
bool AdminMember_Exists(DBConnectionManager* manager, int m_id);

/**
 * 检查账号是否具有指定管理员等级或更高
 *
 * @param manager 数据库连接管理器
 * @param m_id 账号ID
 * @param required_level 要求的管理员等级
 * @return true=具有权限, false=权限不足或不存在
 */
bool AdminMember_HasLevel(DBConnectionManager* manager, int m_id, int required_level);

/**
 * 获取指定等级的所有管理员
 *
 * @param manager 数据库连接管理器
 * @param level 管理员等级
 * @param admins 输出数组
 * @param max_count 数组最大容量
 * @return 实际管理员数量, -1=失败
 */
int AdminMember_GetByLevel(DBConnectionManager* manager, int level,
                           AdminMember* admins, int max_count);

/**
 * 获取所有管理员列表
 *
 * @param manager 数据库连接管理器
 * @param admins 输出数组
 * @param max_count 数组最大容量
 * @return 实际管理员数量, -1=失败
 */
int AdminMember_GetAll(DBConnectionManager* manager, AdminMember* admins, int max_count);

/**
 * 获取管理员总数
 *
 * @param manager 数据库连接管理器
 * @return 管理员总数, -1=失败
 */
int AdminMember_GetCount(DBConnectionManager* manager);

/**
 * 获取指定等级的管理员数量
 *
 * @param manager 数据库连接管理器
 * @param level 管理员等级
 * @return 指定等级管理员数量, -1=失败
 */
int AdminMember_GetCountByLevel(DBConnectionManager* manager, int level);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化管理员结构（设置默认值）
 *
 * @param admin 管理员结构指针
 */
void AdminMember_Init(AdminMember* admin);

/**
 * 打印管理员信息（用于调试）
 *
 * @param admin 管理员结构指针
 */
void AdminMember_Print(const AdminMember* admin);

/**
 * 获取管理员等级名称
 *
 * @param level 管理员等级
 * @return 等级名称字符串
 */
const char* AdminMember_GetLevelName(int level);

#endif // ADMIN_MEMBER_ORM_H
