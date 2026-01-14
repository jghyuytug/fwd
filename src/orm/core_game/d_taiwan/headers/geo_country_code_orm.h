#ifndef GEO_COUNTRY_CODE_ORM_H
#define GEO_COUNTRY_CODE_ORM_H

#include "db_connection_manager.h"

/**
 * geo_country_code - 国家代码映射表
 *
 * 表结构:
 *   code_no          INT(11)      主键 - 国家编号
 *   country_code_a2  VARCHAR(10)  国家代码(2字符) UNIQUE
 *   country_code_a3  VARCHAR(10)  国家代码(3字符) UNIQUE
 *   country          VARCHAR(255) 国家名称
 */

/* 结构体定义 */
typedef struct {
    int code_no;                   /* 国家编号 (主键) */
    char country_code_a2[11];      /* 国家代码A2 (最大10字符 + \0) */
    char country_code_a3[11];      /* 国家代码A3 (最大10字符 + \0) */
    char country[256];             /* 国家名称 (最大255字符 + \0) */
} GeoCountryCode;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加国家代码记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param code 国家代码记录
 * @return 0=成功, -1=失败
 */
int GeoCountryCode_Add(DBConnectionManager* manager, const GeoCountryCode* code);

/**
 * 根据code_no获取国家代码记录
 *
 * @param manager 数据库连接管理器
 * @param code_no 国家编号
 * @param code 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoCountryCode_Get(DBConnectionManager* manager, int code_no, GeoCountryCode* code);

/**
 * 更新国家代码记录
 *
 * @param manager 数据库连接管理器
 * @param code 新的国家代码记录
 * @return 0=成功, -1=失败
 */
int GeoCountryCode_Update(DBConnectionManager* manager, const GeoCountryCode* code);

/**
 * 删除国家代码记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param code_no 国家编号
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int GeoCountryCode_Delete(DBConnectionManager* manager, int code_no);

/**
 * 检查国家代码是否存在
 *
 * @param manager 数据库连接管理器
 * @param code_no 国家编号
 * @return 1=存在, 0=不存在
 */
int GeoCountryCode_Exists(DBConnectionManager* manager, int code_no);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据A2代码获取国家信息
 *
 * @param manager 数据库连接管理器
 * @param country_code_a2 国家代码A2
 * @param code 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoCountryCode_GetByA2(DBConnectionManager* manager, const char* country_code_a2,
                            GeoCountryCode* code);

/**
 * 根据A3代码获取国家信息
 *
 * @param manager 数据库连接管理器
 * @param country_code_a3 国家代码A3
 * @param code 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoCountryCode_GetByA3(DBConnectionManager* manager, const char* country_code_a3,
                            GeoCountryCode* code);

/**
 * 根据国家名称模糊查询
 *
 * @param manager 数据库连接管理器
 * @param country_pattern 国家名称关键词（支持LIKE查询）
 * @param codes 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoCountryCode_GetByCountryPattern(DBConnectionManager* manager, const char* country_pattern,
                                        GeoCountryCode* codes, int max_count);

/**
 * 根据code_no范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_code 起始编号
 * @param end_code 结束编号
 * @param codes 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoCountryCode_GetByCodeRange(DBConnectionManager* manager, int start_code, int end_code,
                                   GeoCountryCode* codes, int max_count);

/**
 * 获取所有国家代码（分页）
 *
 * @param manager 数据库连接管理器
 * @param codes 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoCountryCode_GetAll(DBConnectionManager* manager, GeoCountryCode* codes,
                           int offset, int limit);

/**
 * 获取国家代码总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int GeoCountryCode_GetCount(DBConnectionManager* manager);

/**
 * 获取前N条国家代码记录
 *
 * @param manager 数据库连接管理器
 * @param codes 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoCountryCode_GetTop(DBConnectionManager* manager, GeoCountryCode* codes, int limit);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加国家代码
 *
 * @param manager 数据库连接管理器
 * @param codes 国家代码数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int GeoCountryCode_BulkAdd(DBConnectionManager* manager, const GeoCountryCode* codes, int count);

/**
 * 批量删除国家代码
 *
 * @param manager 数据库连接管理器
 * @param code_nos 国家编号数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int GeoCountryCode_BulkDelete(DBConnectionManager* manager, const int* code_nos, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印国家代码信息（用于调试）
 *
 * @param code 国家代码记录
 */
void GeoCountryCode_PrintInfo(const GeoCountryCode* code);

#endif /* GEO_COUNTRY_CODE_ORM_H */
