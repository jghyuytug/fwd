#ifndef GEO_ALLOW_ORM_H
#define GEO_ALLOW_ORM_H

#include <time.h>
#include "db_connection_manager.h"

/**
 * geo_allow - IP地理位置白名单表
 *
 * 表结构:
 *   allow_ip     VARCHAR(20)  主键 - 允许的IP地址
 *   allow_c_code VARCHAR(4)   国家代码
 *   allow_date   TIMESTAMP    添加时间（自动）
 */

/* 结构体定义 */
typedef struct {
    char allow_ip[21];         /* IP地址 (最大20字符 + \0) */
    char allow_c_code[5];      /* 国家代码 (最大4字符 + \0) */
    char allow_date[20];       /* 添加时间 (格式: YYYY-MM-DD HH:MM:SS) */
} GeoAllow;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加IP白名单记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param allow IP白名单记录
 * @return 0=成功, -1=失败
 */
int GeoAllow_Add(DBConnectionManager* manager, const GeoAllow* allow);

/**
 * 根据IP地址获取白名单记录
 *
 * @param manager 数据库连接管理器
 * @param allow_ip IP地址
 * @param allow 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoAllow_Get(DBConnectionManager* manager, const char* allow_ip, GeoAllow* allow);

/**
 * 更新IP白名单记录
 *
 * @param manager 数据库连接管理器
 * @param allow 新的白名单记录
 * @return 0=成功, -1=失败
 */
int GeoAllow_Update(DBConnectionManager* manager, const GeoAllow* allow);

/**
 * 删除IP白名单记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param allow_ip IP地址
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int GeoAllow_Delete(DBConnectionManager* manager, const char* allow_ip);

/**
 * 检查IP白名单是否存在
 *
 * @param manager 数据库连接管理器
 * @param allow_ip IP地址
 * @return 1=存在, 0=不存在
 */
int GeoAllow_Exists(DBConnectionManager* manager, const char* allow_ip);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据国家代码获取IP白名单列表
 *
 * @param manager 数据库连接管理器
 * @param country_code 国家代码
 * @param allows 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllow_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                               GeoAllow* allows, int max_count);

/**
 * 根据IP模式查询白名单（支持LIKE）
 *
 * @param manager 数据库连接管理器
 * @param ip_pattern IP模式（如 "192.168.%"）
 * @param allows 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询���的记录数, -1=失败
 */
int GeoAllow_GetByIPPattern(DBConnectionManager* manager, const char* ip_pattern,
                             GeoAllow* allows, int max_count);

/**
 * ���据日期范围查询IP白名单
 *
 * @param manager 数据库连接管理器
 * @param start_date 开始日期 (格式: YYYY-MM-DD)
 * @param end_date 结束日期 (格式: YYYY-MM-DD)
 * @param allows 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllow_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                             const char* end_date, GeoAllow* allows, int max_count);

/**
 * 获取所有IP白名单（分页）
 *
 * @param manager 数据库连接管理器
 * @param allows 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllow_GetAll(DBConnectionManager* manager, GeoAllow* allows, int offset, int limit);

/**
 * 获取IP白名单总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int GeoAllow_GetCount(DBConnectionManager* manager);

/**
 * 获取最近添加的N条IP白名单
 *
 * @param manager 数据库连接管理器
 * @param allows 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllow_GetRecent(DBConnectionManager* manager, GeoAllow* allows, int limit);

/**
 * 获取指定天数之前的旧记录
 *
 * @param manager 数据库连接管理器
 * @param days_ago 多少天之前
 * @param allows 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllow_GetOldRecords(DBConnectionManager* manager, int days_ago,
                            GeoAllow* allows, int max_count);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加IP白名单
 *
 * @param manager 数据库连接管理器
 * @param allows IP白名单数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int GeoAllow_BulkAdd(DBConnectionManager* manager, const GeoAllow* allows, int count);

/**
 * 批量删除IP白名单
 *
 * @param manager 数据库连接管理器
 * @param allow_ips IP地址数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int GeoAllow_BulkDelete(DBConnectionManager* manager, const char** allow_ips, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印IP白名单信息（用于调试）
 *
 * @param allow IP白名单记录
 */
void GeoAllow_PrintInfo(const GeoAllow* allow);

#endif /* GEO_ALLOW_ORM_H */
