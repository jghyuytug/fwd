#ifndef GUILD_INFO_ORM_H
#define GUILD_INFO_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// GuildInfo 结构体定义
// ========================================================================

typedef struct {
    int guild_id;                    // 公会ID（主键，自增）
    char guild_name[51];             // 公会名称（最大50字符）
    int guild_level;                 // 公会等级
    int guild_exp;                   // 公会经验
    int guild_master_charac_no;      // 公会会长角色编号
    char guild_notice[501];          // 公会公告（最大500字符）
    int member_count;                // 当前成员数量
    int max_member_count;            // 最大成员数量
    char create_time[20];            // 创建时间 (YYYY-MM-DD HH:MM:SS)
    char update_time[20];            // 更新时间 (YYYY-MM-DD HH:MM:SS)
} GuildInfo;

// ========================================================================
// CRUD 操作函数
// ========================================================================

/**
 * 创建新公会
 * @param manager 数据库连接管理器
 * @param guild 公会信息（不需要设置guild_id，将自动生成）
 * @return 成功返回新创建的guild_id，失败返回-1
 */
int GuildInfo_Create(DBConnectionManager* manager, GuildInfo* guild);

/**
 * 根据公会ID查询公会信息
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param guild 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_GetByID(DBConnectionManager* manager, int guild_id, GuildInfo* guild);

/**
 * 根据公会名称查询公会信息
 * @param manager 数据库连接管理器
 * @param guild_name 公会名称
 * @param guild 输出参数，存储查询结果
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_GetByName(DBConnectionManager* manager, const char* guild_name, GuildInfo* guild);

/**
 * 更新公会信息
 * @param manager 数据库连接管理器
 * @param guild 公会信息
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_Update(DBConnectionManager* manager, const GuildInfo* guild);

/**
 * 更新公会等级和经验
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param level 新等级
 * @param exp 新经验
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_UpdateLevelExp(DBConnectionManager* manager, int guild_id, int level, int exp);

/**
 * 更新公会公告
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param notice 新公告内容
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_UpdateNotice(DBConnectionManager* manager, int guild_id, const char* notice);

/**
 * 更新公会会长
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param new_master_charac_no 新会长角色编号
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_UpdateMaster(DBConnectionManager* manager, int guild_id, int new_master_charac_no);

/**
 * 更新公会成员数量
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param member_count 成员数量（可以是增量或绝对值，由调用者决定）
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_UpdateMemberCount(DBConnectionManager* manager, int guild_id, int member_count);

/**
 * 删除公会
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @return 成功返回0，失败返回-1
 */
int GuildInfo_Delete(DBConnectionManager* manager, int guild_id);

// ========================================================================
// 辅助查询函数
// ========================================================================

/**
 * 检查公会是否存在
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @return 存在返回true，否则返回false
 */
bool GuildInfo_Exists(DBConnectionManager* manager, int guild_id);

/**
 * 检查公会名称是否已被使用
 * @param manager 数据库连接管理器
 * @param guild_name 公会名称
 * @return 已使用返回true，否则返回false
 */
bool GuildInfo_NameExists(DBConnectionManager* manager, const char* guild_name);

/**
 * 获取公会总数
 * @param manager 数据库连接管理器
 * @return 成功返回公会总数，失败返回-1
 */
int GuildInfo_GetCount(DBConnectionManager* manager);

/**
 * 获取指定等级的公会数量
 * @param manager 数据库连接管理器
 * @param level 公会等级
 * @return 成功返回公会数量，失败返回-1
 */
int GuildInfo_GetCountByLevel(DBConnectionManager* manager, int level);

/**
 * 获取所有公会列表（按创建时间排序）
 * @param manager 数据库连接管理器
 * @param guilds 输出参数，存储公会列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的公会数量，失败返回-1
 */
int GuildInfo_GetAll(DBConnectionManager* manager, GuildInfo* guilds, int max_count);

/**
 * 获取指定等级范围的公会列表
 * @param manager 数据库连接管理器
 * @param min_level 最小等级
 * @param max_level 最大等级
 * @param guilds 输出参数，存储公会列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的公会数量，失败返回-1
 */
int GuildInfo_GetByLevelRange(DBConnectionManager* manager, int min_level, int max_level,
                               GuildInfo* guilds, int max_count);

/**
 * 搜索公会（按名称模糊匹配）
 * @param manager 数据库连接管理器
 * @param keyword 搜索关键词
 * @param guilds 输出参数，存储公会列表
 * @param max_count 最大返回数量
 * @return 成功返回实际获取的公会数量，失败返回-1
 */
int GuildInfo_Search(DBConnectionManager* manager, const char* keyword,
                     GuildInfo* guilds, int max_count);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化GuildInfo结构体
 * @param guild 公会信息
 */
void GuildInfo_Init(GuildInfo* guild);

/**
 * 打印GuildInfo信息（用于调试）
 * @param guild 公会信息
 */
void GuildInfo_Print(const GuildInfo* guild);

/**
 * 复制GuildInfo结构体
 * @param dest 目标
 * @param src 源
 */
void GuildInfo_Copy(GuildInfo* dest, const GuildInfo* src);

#endif // GUILD_INFO_ORM_H
