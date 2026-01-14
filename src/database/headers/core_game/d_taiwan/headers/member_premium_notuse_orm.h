/**
 * member_premium_notuse_orm.h
 *
 * ORM header for member_premium_notuse table
 * 会员高级服务未使用记录表 - 记录会员未使用的高级服务
 *
 * Table: member_premium_notuse (6 fields)
 * Primary Key: (event_id, pre_type, server_id, m_id, service_start) - 复合主键
 *
 * Generated: 2025-11-14
 */

#ifndef MEMBER_PREMIUM_NOTUSE_ORM_H
#define MEMBER_PREMIUM_NOTUSE_ORM_H

#include "db_connection_manager.h"

/**
 * Structure representing a member_premium_notuse record
 * 表示会员高级服务未使用记录的结构体
 */
typedef struct {
    int event_id;              /* 活动ID (主键) */
    unsigned char pre_type;    /* 高级服务类型 (主键, 0-255) */
    int m_id;                  /* 会员ID (主键) */
    char service_start[20];    /* 服务开始时间 (主键, YYYY-MM-DD HH:MM:SS) */
    char service_end[20];      /* 服务结束时间 (YYYY-MM-DD HH:MM:SS) */
    unsigned char server_id;   /* 服务器ID (主键, 0-255) */
} MemberPremiumNotuse;

/**
 * Add a new member_premium_notuse record
 * 添加新的高级服务未使用记录（幂等操作，使用REPLACE INTO）
 *
 * @param manager Database connection manager
 * @param record  Record to add
 * @return 0 on success, -1 on error
 */
int MemberPremiumNotuse_Add(DBConnectionManager* manager, const MemberPremiumNotuse* record);

/**
 * Get a member_premium_notuse record by composite primary key
 * 根据复合主键获取高级服务未使用记录
 *
 * @param manager       Database connection manager
 * @param event_id      Event ID
 * @param pre_type      Premium type
 * @param server_id     Server ID
 * @param m_id          Member ID
 * @param service_start Service start time (YYYY-MM-DD HH:MM:SS)
 * @param record        Output record (must be pre-allocated)
 * @return 0 on success, -1 on error or not found
 */
int MemberPremiumNotuse_Get(DBConnectionManager* manager,
                             int event_id, unsigned char pre_type, unsigned char server_id,
                             int m_id, const char* service_start,
                             MemberPremiumNotuse* record);

/**
 * Update an existing member_premium_notuse record
 * 更新现有高级服务未使用记录（仅更新service_end）
 *
 * @param manager Database connection manager
 * @param record  Record with updated values
 * @return 0 on success, -1 on error
 */
int MemberPremiumNotuse_Update(DBConnectionManager* manager, const MemberPremiumNotuse* record);

/**
 * Delete a member_premium_notuse record by composite primary key
 * 根据复合主键删除高级服务未使用记录（幂等操作）
 *
 * @param manager       Database connection manager
 * @param event_id      Event ID
 * @param pre_type      Premium type
 * @param server_id     Server ID
 * @param m_id          Member ID
 * @param service_start Service start time (YYYY-MM-DD HH:MM:SS)
 * @return 0 on success (always returns 0, even if record doesn't exist)
 */
int MemberPremiumNotuse_Delete(DBConnectionManager* manager,
                                 int event_id, unsigned char pre_type, unsigned char server_id,
                                 int m_id, const char* service_start);

/**
 * Check if a member_premium_notuse record exists
 * 检查高级服务未使用记录是否存在
 *
 * @param manager       Database connection manager
 * @param event_id      Event ID
 * @param pre_type      Premium type
 * @param server_id     Server ID
 * @param m_id          Member ID
 * @param service_start Service start time (YYYY-MM-DD HH:MM:SS)
 * @return 1 if exists, 0 if not, -1 on error
 */
int MemberPremiumNotuse_Exists(DBConnectionManager* manager,
                                 int event_id, unsigned char pre_type, unsigned char server_id,
                                 int m_id, const char* service_start);

/**
 * Get records by event_id
 * 按活动ID查询记录
 *
 * @param manager     Database connection manager
 * @param event_id    Event ID
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByEventId(DBConnectionManager* manager,
                                       int event_id,
                                       MemberPremiumNotuse* records, int max_records);

/**
 * Get records by member_id
 * 按会员ID查询记录
 *
 * @param manager     Database connection manager
 * @param m_id        Member ID
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByMemberId(DBConnectionManager* manager,
                                        int m_id,
                                        MemberPremiumNotuse* records, int max_records);

/**
 * Get records by pre_type
 * 按高级服务类型查询记录
 *
 * @param manager     Database connection manager
 * @param pre_type    Premium type
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByPreType(DBConnectionManager* manager,
                                       unsigned char pre_type,
                                       MemberPremiumNotuse* records, int max_records);

/**
 * Get records by server_id
 * 按服务器ID查询记录
 *
 * @param manager     Database connection manager
 * @param server_id   Server ID
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByServerId(DBConnectionManager* manager,
                                        unsigned char server_id,
                                        MemberPremiumNotuse* records, int max_records);

/**
 * Get records by service_start time range
 * 按服务开始时间范围查询记录
 *
 * @param manager     Database connection manager
 * @param start_time  Start time (YYYY-MM-DD HH:MM:SS)
 * @param end_time    End time (YYYY-MM-DD HH:MM:SS)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByServiceStartRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 MemberPremiumNotuse* records, int max_records);

/**
 * Get records by service_end time range
 * 按服务结束时间范围查询记录
 *
 * @param manager     Database connection manager
 * @param start_time  Start time (YYYY-MM-DD HH:MM:SS)
 * @param end_time    End time (YYYY-MM-DD HH:MM:SS)
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByServiceEndRange(DBConnectionManager* manager,
                                               const char* start_time, const char* end_time,
                                               MemberPremiumNotuse* records, int max_records);

/**
 * Get active services (service_end > NOW())
 * 查询当前有效的服务（服务结束时间 > 当前时间）
 *
 * @param manager     Database connection manager
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetActiveServices(DBConnectionManager* manager,
                                            MemberPremiumNotuse* records, int max_records);

/**
 * Get expired services (service_end < NOW())
 * 查询已过期的服务（服务结束时间 < 当前时间）
 *
 * @param manager     Database connection manager
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetExpiredServices(DBConnectionManager* manager,
                                             MemberPremiumNotuse* records, int max_records);

/**
 * Get records by event_id and m_id
 * 按活动ID和会员ID查询记录
 *
 * @param manager     Database connection manager
 * @param event_id    Event ID
 * @param m_id        Member ID
 * @param records     Output array (must be pre-allocated)
 * @param max_records Maximum records to retrieve
 * @return Number of records retrieved, -1 on error
 */
int MemberPremiumNotuse_GetByEventIdAndMemberId(DBConnectionManager* manager,
                                                  int event_id, int m_id,
                                                  MemberPremiumNotuse* records, int max_records);

/**
 * Bulk add member_premium_notuse records
 * 批量添加高级服务未使用记录（幂等操作）
 *
 * @param manager Database connection manager
 * @param records Array of records to add
 * @param count   Number of records
 * @return Number of records successfully added, -1 on error
 */
int MemberPremiumNotuse_BulkAdd(DBConnectionManager* manager,
                                  const MemberPremiumNotuse* records, int count);

/**
 * Bulk delete member_premium_notuse records by member_id
 * 批量删除指定会员的所有高级服务未使用记录（幂等操作）
 *
 * @param manager Database connection manager
 * @param m_ids   Array of member IDs
 * @param count   Number of IDs
 * @return 0 on success, -1 on error
 */
int MemberPremiumNotuse_BulkDeleteByMemberId(DBConnectionManager* manager,
                                               const int* m_ids, int count);

/**
 * Print member_premium_notuse record information
 * 打印高级服务未使用记录信息（用于调试）
 *
 * @param record Record to print
 */
void MemberPremiumNotuse_PrintInfo(const MemberPremiumNotuse* record);

#endif /* MEMBER_PREMIUM_NOTUSE_ORM_H */
