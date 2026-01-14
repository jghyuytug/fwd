/**
 * member_passwd_mod_orm.h
 *
 * ORM header for member_passwd_mod table
 * 密码修改历史追踪表 - 记录会员密码修改的次数和时间信息
 *
 * Table: member_passwd_mod (4 fields)
 * Primary Key: m_id
 *
 * Generated: 2025-11-14
 */

#ifndef MEMBER_PASSWD_MOD_ORM_H
#define MEMBER_PASSWD_MOD_ORM_H

#include "db_connection_manager.h"

/**
 * Structure representing a member_passwd_mod record
 * 表示密码修改记录的结构体
 */
typedef struct {
    int m_id;                  /* 会员ID (主键) */
    char first_time[20];       /* 首次修改密码时间 (YYYY-MM-DD HH:MM:SS) */
    char last_time[20];        /* 最后修改密码时间 (YYYY-MM-DD HH:MM:SS) */
    unsigned char cnt;         /* 修改次数 (0-255) */
} MemberPasswdMod;

/**
 * Add a new member_passwd_mod record
 * 添加新的密码修改记录（幂等操作，使用REPLACE INTO）
 *
 * @param manager Database connection manager
 * @param record  Record to add
 * @return 0 on success, -1 on error
 */
int MemberPasswdMod_Add(DBConnectionManager* manager, const MemberPasswdMod* record);

/**
 * Get a member_passwd_mod record by m_id
 * 根据会员ID获取密码修改记录
 *
 * @param manager Database connection manager
 * @param m_id    Member ID
 * @param record  Output record (must be pre-allocated)
 * @return 0 on success, -1 on error or not found
 */
int MemberPasswdMod_Get(DBConnectionManager* manager, int m_id, MemberPasswdMod* record);

/**
 * Update an existing member_passwd_mod record
 * 更新现有密码修改记录
 *
 * @param manager Database connection manager
 * @param record  Record with updated values
 * @return 0 on success, -1 on error
 */
int MemberPasswdMod_Update(DBConnectionManager* manager, const MemberPasswdMod* record);

/**
 * Delete a member_passwd_mod record by m_id
 * 根据会员ID删除密码修改记录（幂等操作）
 *
 * @param manager Database connection manager
 * @param m_id    Member ID
 * @return 0 on success (always returns 0, even if record doesn't exist)
 */
int MemberPasswdMod_Delete(DBConnectionManager* manager, int m_id);

/**
 * Check if a member_passwd_mod record exists
 * 检查密码修改记录是否存在
 *
 * @param manager Database connection manager
 * @param m_id    Member ID
 * @return 1 if exists, 0 if not, -1 on error
 */
int MemberPasswdMod_Exists(DBConnectionManager* manager, int m_id);

/**
 * Atomically increment the modification count and update last_time to NOW()
 * 原子递增修改次数并更新最后修改时间为当前时间
 *
 * @param manager Database connection manager
 * @param m_id    Member ID
 * @return 0 on success, -1 on error
 */
int MemberPasswdMod_IncrementCnt(DBConnectionManager* manager, int m_id);

/**
 * Get records by first_time range
 * 按首次修改时间范围查询记录
 *
 * @param manager     Database connection manager
 * @param start_time  Start time (YYYY-MM-DD HH:MM:SS)
 * @param end_time    End time (YYYY-MM-DD HH:MM:SS)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetByFirstTimeRange(DBConnectionManager* manager,
                                         const char* start_time, const char* end_time,
                                         MemberPasswdMod* records, int max_records);

/**
 * Get records by last_time range
 * 按最后修改时间范围查询记录
 *
 * @param manager     Database connection manager
 * @param start_time  Start time (YYYY-MM-DD HH:MM:SS)
 * @param end_time    End time (YYYY-MM-DD HH:MM:SS)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetByLastTimeRange(DBConnectionManager* manager,
                                        const char* start_time, const char* end_time,
                                        MemberPasswdMod* records, int max_records);

/**
 * Get records by cnt range
 * 按修改次数范围查询记录
 *
 * @param manager     Database connection manager
 * @param min_cnt     Minimum count (inclusive)
 * @param max_cnt     Maximum count (inclusive)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetByCntRange(DBConnectionManager* manager,
                                   int min_cnt, int max_cnt,
                                   MemberPasswdMod* records, int max_records);

/**
 * Get frequent modifiers (users who have modified password >= threshold times)
 * 获取频繁修改密码的用户（修改次数 >= 阈值）
 *
 * @param manager     Database connection manager
 * @param threshold   Minimum modification count
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetFrequentModifiers(DBConnectionManager* manager,
                                          int threshold,
                                          MemberPasswdMod* records, int max_records);

/**
 * Get recent modifiers (sorted by last_time DESC)
 * 获取最近修改密码的用户（按最后修改时间降序排序）
 *
 * @param manager     Database connection manager
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetRecentModifiers(DBConnectionManager* manager,
                                        MemberPasswdMod* records, int max_records);

/**
 * Update last_time to NOW()
 * 更新最后修改时间为当前时间
 *
 * @param manager Database connection manager
 * @param m_id    Member ID
 * @return 0 on success, -1 on error
 */
int MemberPasswdMod_UpdateLastTime(DBConnectionManager* manager, int m_id);

/**
 * Get users who have never modified their password (cnt = 0)
 * 获取从未修改过密码的用户（修改次数 = 0）
 *
 * @param manager     Database connection manager
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetNeverModified(DBConnectionManager* manager,
                                      MemberPasswdMod* records, int max_records);

/**
 * Get users with modification count >= specified value
 * 获取修改次数大于等于指定值的用户
 *
 * @param manager     Database connection manager
 * @param min_cnt     Minimum count (inclusive)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPasswdMod_GetByMinCnt(DBConnectionManager* manager,
                                 int min_cnt,
                                 MemberPasswdMod* records, int max_records);

/**
 * Bulk add member_passwd_mod records
 * 批量添加密码修改记录（幂等操作）
 *
 * @param manager Database connection manager
 * @param records Array of records to add
 * @param count   Number of records
 * @return Number of records successfully added, -1 on error
 */
int MemberPasswdMod_BulkAdd(DBConnectionManager* manager,
                             const MemberPasswdMod* records, int count);

/**
 * Bulk delete member_passwd_mod records by m_id array
 * 批量删除密码修改记录（幂等操作）
 *
 * @param manager Database connection manager
 * @param m_ids   Array of member IDs
 * @param count   Number of IDs
 * @return 0 on success, -1 on error
 */
int MemberPasswdMod_BulkDelete(DBConnectionManager* manager,
                                const int* m_ids, int count);

/**
 * Print member_passwd_mod record information
 * 打印密码修改记录信息（用于调试）
 *
 * @param record Record to print
 */
void MemberPasswdMod_PrintInfo(const MemberPasswdMod* record);

#endif /* MEMBER_PASSWD_MOD_ORM_H */
