#ifndef MEMBER_PASSWD_MOD_ORM_H
#define MEMBER_PASSWD_MOD_ORM_H

#include "db_connection_manager.h"

/**
 * member_passwd_mod - 会员密码修改记录表
 *
 * 表结构:
 *   m_id         INT(11)             主键 - 会员ID
 *   first_time   DATETIME            首次修改时间
 *   last_time    DATETIME            最后修改时间
 *   cnt          TINYINT(3) UNSIGNED 修改次数
 */

/* 结构体定义 */
typedef struct {
    int m_id;                       /* 会员ID (主键) */
    char first_time[20];            /* 首次修改时间 */
    char last_time[20];             /* 最后修改时间 */
    unsigned char cnt;              /* 修改次数 */
} MemberPasswdMod;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加密码修改记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 密码修改记录
 * @return 0=成功, -1=失败
 */
int MemberPasswdMod_Add(DBConnectionManager* manager, const MemberPasswdMod* record);

/**
 * 根据会员ID获取密码修改记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberPasswdMod_Get(DBConnectionManager* manager, int m_id, MemberPasswdMod* record);

/**
 * 更新密码修改记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的密码修改记录
 * @return 0=成功, -1=失败
 */
int MemberPasswdMod_Update(DBConnectionManager* manager, const MemberPasswdMod* record);

/**
 * 删除密码修改记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberPasswdMod_Delete(DBConnectionManager* manager, int m_id);

/**
 * 检查密码修改记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberPasswdMod_Exists(DBConnectionManager* manager, int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 增加修改次数并更新最后修改时间
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param current_time 当前时间字符串
 * @return 0=成功, -1=失败
 */
int MemberPasswdMod_IncrementCount(DBConnectionManager* manager, int m_id, const char* current_time);

/**
 * 更新最后修改时间
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param last_time 最后修改时间
 * @return 0=成功, -1=失败
 */
int MemberPasswdMod_UpdateLastTime(DBConnectionManager* manager, int m_id, const char* last_time);

/**
 * 根据修改次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_cnt 最小修改次数
 * @param max_cnt 最大修改次数
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPasswdMod_GetByCountRange(DBConnectionManager* manager, int min_cnt, int max_cnt,
                                     MemberPasswdMod* records, int max_count);

/**
 * 根据首次修改时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 起始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPasswdMod_GetByFirstTimeRange(DBConnectionManager* manager, const char* start_time,
                                         const char* end_time, MemberPasswdMod* records,
                                         int max_count);

/**
 * 根据最后修改时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 起始时间
 * @param end_time 结束时间
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPasswdMod_GetByLastTimeRange(DBConnectionManager* manager, const char* start_time,
                                        const char* end_time, MemberPasswdMod* records,
                                        int max_count);

/**
 * 获取最近修改的记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPasswdMod_GetRecentModified(DBConnectionManager* manager, MemberPasswdMod* records,
                                       int limit);

/**
 * 获取所有密码修改记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberPasswdMod_GetAll(DBConnectionManager* manager, MemberPasswdMod* records,
                            int offset, int limit);

/**
 * 获取密码修改记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberPasswdMod_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加密码修改记录
 *
 * @param manager 数据库连接管理器
 * @param records 密码修改记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberPasswdMod_BulkAdd(DBConnectionManager* manager, const MemberPasswdMod* records,
                             int count);

/**
 * 批量删除密码修改记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberPasswdMod_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印密码修改记录信息（用于调试）
 *
 * @param record 密码修改记录
 */
void MemberPasswdMod_PrintInfo(const MemberPasswdMod* record);

#endif /* MEMBER_PASSWD_MOD_ORM_H */
