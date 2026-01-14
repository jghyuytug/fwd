#ifndef GEO_ALLOW_COUNTRY_ORM_H
#define GEO_ALLOW_COUNTRY_ORM_H

#include "db_connection_manager.h"

/**
 * geo_allow_country - 服务器组国家白名单表
 *
 * 表结构:
 *   server_group  TINYINT(4)   主键1 - 服务器组ID
 *   country_code  VARCHAR(10)  主键2 - 国家代码
 *   reg_date      DATETIME     注册时间
 */

/* 结构体定义 */
typedef struct {
    int server_group;              /* 服务器组ID (主键1) */
    char country_code[11];         /* 国家代码 (主键2, 最大10字符 + \0) */
    char reg_date[20];             /* 注册时间 (YYYY-MM-DD HH:MM:SS + \0) */
} GeoAllowCountry;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加国家白名单记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 白名单记录
 * @return 0=成功, -1=失败
 */
int GeoAllowCountry_Add(DBConnectionManager* manager, const GeoAllowCountry* record);

/**
 * 根据复合主键获取白名单记录
 *
 * @param manager 数据库连接管理器
 * @param server_group 服务器组ID
 * @param country_code 国家代码
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoAllowCountry_Get(DBConnectionManager* manager, int server_group,
                         const char* country_code, GeoAllowCountry* record);

/**
 * 更新白名单记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的白名单记录
 * @return 0=成功, -1=失败
 */
int GeoAllowCountry_Update(DBConnectionManager* manager, const GeoAllowCountry* record);

/**
 * 删除白名单记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param server_group 服务器组ID
 * @param country_code 国家代码
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int GeoAllowCountry_Delete(DBConnectionManager* manager, int server_group,
                            const char* country_code);

/**
 * 检查白名单记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param server_group 服务器组ID
 * @param country_code 国家代码
 * @return 1=存在, 0=不存在
 */
int GeoAllowCountry_Exists(DBConnectionManager* manager, int server_group,
                            const char* country_code);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 获取指定服务器组的所有国家白名单
 *
 * @param manager 数据库连接管理器
 * @param server_group 服务器组ID
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetByServerGroup(DBConnectionManager* manager, int server_group,
                                      GeoAllowCountry* records, int max_count);

/**
 * 获取指定国家代码在所有服务器组的白名单记录
 *
 * @param manager 数据库连接管理器
 * @param country_code 国家代码
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                                      GeoAllowCountry* records, int max_count);

/**
 * 根据日期范围查询白名单记录
 *
 * @param manager 数据库连接管理器
 * @param start_date 起始日期 (YYYY-MM-DD HH:MM:SS)
 * @param end_date 结束日期 (YYYY-MM-DD HH:MM:SS)
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, GeoAllowCountry* records, int max_count);

/**
 * 获取最近添加的N条白名单记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetRecentAdded(DBConnectionManager* manager, GeoAllowCountry* records,
                                    int limit);

/**
 * 获取所有白名单记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetAll(DBConnectionManager* manager, GeoAllowCountry* records,
                            int offset, int limit);

/**
 * 获取白名单记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int GeoAllowCountry_GetCount(DBConnectionManager* manager);

/**
 * 获取前N条白名单记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoAllowCountry_GetTop(DBConnectionManager* manager, GeoAllowCountry* records, int limit);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加白名单记录
 *
 * @param manager 数据库连接管理器
 * @param records 白名单记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int GeoAllowCountry_BulkAdd(DBConnectionManager* manager, const GeoAllowCountry* records,
                             int count);

/**
 * 批量删除白名单记录
 *
 * @param manager 数据库连接管理器
 * @param records 要删除的记录数组（使用server_group和country_code）
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int GeoAllowCountry_BulkDelete(DBConnectionManager* manager, const GeoAllowCountry* records,
                                int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印白名单记录信息（用于调试）
 *
 * @param record 白名单记录
 */
void GeoAllowCountry_PrintInfo(const GeoAllowCountry* record);

#endif /* GEO_ALLOW_COUNTRY_ORM_H */
