/**
 * DNF游戏服务器 - AdminMember表ORM层
 *
 * 功能: 管理员账号管理系统
 * 数据库: d_taiwan
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
 * 管理员信息结构
 */
typedef struct {
    int no;                         // 管理员编号 (主键, auto_increment)
    char user_id[21];               // 用户ID (20 + \0)
    char password[21];              // 密码 (20 + \0)
    char name[21];                  // 姓名 (20 + \0)
    char email[256];                // 邮箱 (255 + \0)
    char phone[256];                // 电话 (255 + \0)
    char msn[51];                   // MSN账号 (50 + \0)
    char* comment;                  // 备注 (text字段，动态分配)
    long long reg_date;             // 注册日期时间戳 (int(13))
    char confirm;                   // 确认状态 ('0'/'1')
    char level[2001];               // 权限级别 (2000 + \0)
    char level_group1[3];           // 权限组1 (2 + \0)
    char level_group2[3];           // 权限组2 (2 + \0)
    char level_group3[3];           // 权限组3 (2 + \0)
    char level_group4[3];           // 权限组4 (2 + \0)
    char level_group5[3];           // 权限组5 (2 + \0)
    char level_group6[3];           // 权限组6 (2 + \0)
} AdminMember;

// ========================================================================
// CRUD操作函数
// ========================================================================

/**
 * 创建管理员记录
 *
 * @param manager 数据库连接管理器
 * @param admin 管理员信息结构
 * @return 新记录的no (>0成功), -1=失败
 */
int AdminMember_Add(DBConnectionManager* manager, AdminMember* admin);

/**
 * 根据no获取管理员信息
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @param admin 输出参数，管理员信息
 * @return 0=成功, -1=失败（不存在或错误）
 */
int AdminMember_Get(DBConnectionManager* manager, int no, AdminMember* admin);

/**
 * 根据user_id获取管理员信息
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @param admin 输出参数，管理员信息
 * @return 0=成功, -1=失败
 */
int AdminMember_GetByUserId(DBConnectionManager* manager, const char* user_id, AdminMember* admin);

/**
 * 更新管理员信息（完整更新）
 *
 * @param manager 数据库连接管理器
 * @param admin 管理员信息结构
 * @return 0=成功, -1=失败
 */
int AdminMember_Update(DBConnectionManager* manager, const AdminMember* admin);

/**
 * 删除管理员记录
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @return 0=成功, -1=失败
 */
int AdminMember_Delete(DBConnectionManager* manager, int no);

// ========================================================================
// 专项操作函数
// ========================================================================

/**
 * 验证管理员登录（用户ID+密码）
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @param password 密码
 * @return 管理员编号no (>0成功), -1=失败
 */
int AdminMember_Verify(DBConnectionManager* manager, const char* user_id, const char* password);

/**
 * 更新管理员密码
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @param new_password 新密码
 * @return 0=成功, -1=失败
 */
int AdminMember_UpdatePassword(DBConnectionManager* manager, int no, const char* new_password);

/**
 * 更新管理员确认状态
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @param confirm 确认状态 ('0'/'1')
 * @return 0=成功, -1=失败
 */
int AdminMember_UpdateConfirm(DBConnectionManager* manager, int no, char confirm);

/**
 * 更新管理员联系信息
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @param email 邮箱（NULL保持不变）
 * @param phone 电话（NULL保持不变）
 * @return 0=成功, -1=失败
 */
int AdminMember_UpdateContact(DBConnectionManager* manager, int no, const char* email, const char* phone);

/**
 * 更新权限级别
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @param level 新权限级别字符串
 * @return 0=成功, -1=失败
 */
int AdminMember_UpdateLevel(DBConnectionManager* manager, int no, const char* level);

/**
 * 检查用户ID是否已存在
 *
 * @param manager 数据库连接管理器
 * @param user_id 用户ID
 * @return true=已存在, false=不存在
 */
bool AdminMember_UserIdExists(DBConnectionManager* manager, const char* user_id);

// ========================================================================
// 查询操作函数
// ========================================================================

/**
 * 检查管理员是否存在
 *
 * @param manager 数据库连接管理器
 * @param no 管理员编号
 * @return true=存在, false=不存在
 */
bool AdminMember_Exists(DBConnectionManager* manager, int no);

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
 * 获取已确认的管理员列表
 *
 * @param manager 数据库连接管理器
 * @param admins 输出数组
 * @param max_count 数组最大容量
 * @return 实际数量, -1=失败
 */
int AdminMember_GetConfirmed(DBConnectionManager* manager, AdminMember* admins, int max_count);

/**
 * 获取管理员总数
 *
 * @param manager 数据库连接管理器
 * @return 管理员总数, -1=失败
 */
int AdminMember_GetCount(DBConnectionManager* manager);

/**
 * 搜索管理员（按用户ID或姓名）
 *
 * @param manager 数据库连接管理器
 * @param keyword 关键词
 * @param admins 输出数组
 * @param max_count 数组最大容量
 * @return 实际数量, -1=失败
 */
int AdminMember_Search(DBConnectionManager* manager, const char* keyword, AdminMember* admins, int max_count);

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
 * 释放管理员结构中的动态内存
 *
 * @param admin 管理员结构指针
 */
void AdminMember_Free(AdminMember* admin);

/**
 * 打印管理员信息（用于调试）
 *
 * @param admin 管理员结构指针
 */
void AdminMember_PrintInfo(const AdminMember* admin);

#endif // ADMIN_MEMBER_ORM_H
