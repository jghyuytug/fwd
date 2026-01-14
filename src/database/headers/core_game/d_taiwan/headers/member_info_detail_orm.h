#ifndef MEMBER_INFO_DETAIL_ORM_H
#define MEMBER_INFO_DETAIL_ORM_H

#include <time.h>
#include "db_connection_manager.h"

/**
 * member_info_detail - 会员详细信息表（地址信息）
 *
 * 表结构:
 *   m_id          INT(11)      主键 - 会员ID
 *   zipcode       VARCHAR(7)   邮编
 *   address       VARCHAR(80)  地址
 *   address_detail VARCHAR(70) 详细地址
 *   occ_date      DATETIME     记录日期
 */

/* 结构体定义 */
typedef struct {
    int m_id;                    /* 会员ID (主键) */
    char zipcode[8];             /* 邮编 (最大7字符 + \0) */
    char address[81];            /* 地址 (最大80字符 + \0) */
    char address_detail[71];     /* 详细地址 (最大70字符 + \0) */
    char occ_date[20];           /* 记录日期 (格式: YYYY-MM-DD HH:MM:SS) */
} MemberInfoDetail;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员详细信息（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param detail 会员详细信息
 * @return 0=成功, -1=失败
 */
int MemberInfoDetail_Add(DBConnectionManager* manager, const MemberInfoDetail* detail);

/**
 * 根据m_id获取会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param detail 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberInfoDetail_Get(DBConnectionManager* manager, int m_id, MemberInfoDetail* detail);

/**
 * 更新会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param detail 新的会员详细信息
 * @return 0=成功, -1=失败
 */
int MemberInfoDetail_Update(DBConnectionManager* manager, const MemberInfoDetail* detail);

/**
 * 删除会员详细信息（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberInfoDetail_Delete(DBConnectionManager* manager, int m_id);

/**
 * 检查会员详细信息是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberInfoDetail_Exists(DBConnectionManager* manager, int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据邮编获取会员详细信息列表
 *
 * @param manager 数据库连接管理器
 * @param zipcode 邮编
 * @param details 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetByZipcode(DBConnectionManager* manager, const char* zipcode,
                                  MemberInfoDetail* details, int max_count);

/**
 * 根据地址模糊查询会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param address_pattern 地址关键词（支持LIKE查询）
 * @param details 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetByAddressPattern(DBConnectionManager* manager, const char* address_pattern,
                                         MemberInfoDetail* details, int max_count);

/**
 * 根据日期范围查询会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param start_date 开始日期 (格式: YYYY-MM-DD)
 * @param end_date 结束日期 (格式: YYYY-MM-DD)
 * @param details 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, MemberInfoDetail* details,
                                    int max_count);

/**
 * 获取所有会员详细信息（分页）
 *
 * @param manager 数据库连接管理器
 * @param details 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetAll(DBConnectionManager* manager, MemberInfoDetail* details,
                            int offset, int limit);

/**
 * 获取会员详细信息总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberInfoDetail_GetCount(DBConnectionManager* manager);

/**
 * 获取最近添加的N条会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param details 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetRecent(DBConnectionManager* manager, MemberInfoDetail* details, int limit);

/**
 * 获取指定天数之前的旧记录
 *
 * @param manager 数据库连接管理器
 * @param days_ago 多少天之前
 * @param details 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberInfoDetail_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                   MemberInfoDetail* details, int max_count);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param details 会员详细信息数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberInfoDetail_BulkAdd(DBConnectionManager* manager, const MemberInfoDetail* details, int count);

/**
 * 批量删除会员详细信息
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberInfoDetail_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印会员详细信息（用于调试）
 *
 * @param detail 会员详细信息
 */
void MemberInfoDetail_PrintInfo(const MemberInfoDetail* detail);

#endif /* MEMBER_INFO_DETAIL_ORM_H */
