#ifndef MEMBER_BROADCAST_ORM_H
#define MEMBER_BROADCAST_ORM_H

#include "db_connection_manager.h"

/**
 * member_broadcast - 会员广播消息表
 *
 * 表结构:
 *   event_id      INT(11)             主键1 - 事件ID
 *   m_id          INT(11)             主键2 - 会员ID
 *   server_id     TINYINT(4)          主键3 - 服务器ID
 *   charac_no     INT(11)             主键4 - 角色编号
 *   charac_name   VARCHAR(20)         角色名称（唯一索引）
 *   start_time    DATETIME            主键5 - 开始时间
 *   end_time      DATETIME            结束时间
 */

/* 结构体定义 */
typedef struct {
    int event_id;               /* 事件ID (主键1) */
    int m_id;                   /* 会员ID (主键2) */
    unsigned char server_id;    /* 服务器ID (主键3) */
    int charac_no;              /* 角色编号 (主键4) */
    char charac_name[21];       /* 角色名称 */
    char start_time[20];        /* 开始时间 (主键5) */
    char end_time[20];          /* 结束时间 */
} MemberBroadcast;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员广播记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员广播记录
 * @return 0=成功, -1=失败
 */
int MemberBroadcast_Add(DBConnectionManager* manager, const MemberBroadcast* record);

/**
 * 根据复合主键获取会员广播记录
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param m_id 会员ID
 * @param server_id 服务器ID
 * @param charac_no 角色编号
 * @param start_time 开始时间
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberBroadcast_Get(DBConnectionManager* manager, int event_id, int m_id,
                        unsigned char server_id, int charac_no, const char* start_time,
                        MemberBroadcast* record);

/**
 * 更新会员广播记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的会员广播记录
 * @return 0=成功, -1=失败
 */
int MemberBroadcast_Update(DBConnectionManager* manager, const MemberBroadcast* record);

/**
 * 删除会员广播记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param m_id 会员ID
 * @param server_id 服务器ID
 * @param charac_no 角色编号
 * @param start_time 开始时间
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberBroadcast_Delete(DBConnectionManager* manager, int event_id, int m_id,
                           unsigned char server_id, int charac_no, const char* start_time);

/**
 * 检查会员广播记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param m_id 会员ID
 * @param server_id 服务器ID
 * @param charac_no 角色编号
 * @param start_time 开始时间
 * @return 1=存在, 0=不存在
 */
int MemberBroadcast_Exists(DBConnectionManager* manager, int event_id, int m_id,
                           unsigned char server_id, int charac_no, const char* start_time);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据事件ID查询
 *
 * @param manager 数据库连接管理器
 * @param event_id 事件ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetByEventId(DBConnectionManager* manager, int event_id,
                                  MemberBroadcast* records, int max_records);

/**
 * 根据会员ID查询
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   MemberBroadcast* records, int max_records);

/**
 * 根据服务器ID查询
 *
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetByServerId(DBConnectionManager* manager, unsigned char server_id,
                                   MemberBroadcast* records, int max_records);

/**
 * 根据角色名称查询（唯一索引）
 *
 * @param manager 数据库连接管理器
 * @param charac_name 角色名称
 * @param record 输出参数
 * @return 0=成功, -1=失败或不存在
 */
int MemberBroadcast_GetByCharacName(DBConnectionManager* manager, const char* charac_name,
                                     MemberBroadcast* record);

/**
 * 根据时间范围查询（start_time范围）
 *
 * @param manager 数据库连接管理器
 * @param start_begin 开始时间下限
 * @param start_end 开始时间上限
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetByTimeRange(DBConnectionManager* manager, const char* start_begin,
                                    const char* start_end, MemberBroadcast* records,
                                    int max_records);

/**
 * 获取活动中的广播（当前时间在start_time和end_time之间）
 *
 * @param manager 数据库连接管理器
 * @param current_time 当前时间
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetActive(DBConnectionManager* manager, const char* current_time,
                               MemberBroadcast* records, int max_records);

/**
 * 获取所有会员广播记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberBroadcast_GetAll(DBConnectionManager* manager, MemberBroadcast* records,
                            int offset, int limit);

/**
 * 获取会员广播记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberBroadcast_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加会员广播记录
 *
 * @param manager 数据库连接管理器
 * @param records 会员广播记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberBroadcast_BulkAdd(DBConnectionManager* manager, const MemberBroadcast* records,
                             int count);

/**
 * 批量删除会员广播记录（按事件ID）
 *
 * @param manager 数据库连接管理器
 * @param event_ids 事件ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberBroadcast_BulkDeleteByEvent(DBConnectionManager* manager, const int* event_ids,
                                       int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印会员广播记录（用于调试）
 *
 * @param record 会员广播记录
 */
void MemberBroadcast_PrintInfo(const MemberBroadcast* record);

#endif /* MEMBER_BROADCAST_ORM_H */
