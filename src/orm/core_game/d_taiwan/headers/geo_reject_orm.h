#ifndef GEO_REJECT_ORM_H
#define GEO_REJECT_ORM_H

#include "db_connection_manager.h"

/**
 * geo_reject - IP黑名单/拒绝列表表
 *
 * 表结构:
 *   rej_ip         VARCHAR(20)        主键 - 拒绝IP地址
 *   rej_c_code     VARCHAR(4)         国家代码
 *   rej_ip_count   INT(11) UNSIGNED   IP访问计数
 *   rej_last_date  TIMESTAMP          最后访问时间
 *   rej_chk        CHAR(1)            检查标志 (Y/N)
 *   rej_src        ENUM('w','g')      来源 (w=web, g=game)
 */

/* 结构体定义 */
typedef struct {
    char rej_ip[21];               /* 拒绝IP地址 (主键, 最大20字符 + \0) */
    char rej_c_code[5];            /* 国家代码 (最大4字符 + \0) */
    unsigned int rej_ip_count;     /* IP访问计数 */
    char rej_last_date[20];        /* 最后访问时间 (YYYY-MM-DD HH:MM:SS + \0) */
    char rej_chk;                  /* 检查标志 (Y/N) */
    char rej_src;                  /* 来源 (w=web, g=game) */
} GeoReject;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加IP拒绝记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 拒绝记录
 * @return 0=成功, -1=失败
 */
int GeoReject_Add(DBConnectionManager* manager, const GeoReject* record);

/**
 * 根据IP地址获取拒绝记录
 *
 * @param manager 数据库连接管理器
 * @param rej_ip IP地址
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int GeoReject_Get(DBConnectionManager* manager, const char* rej_ip, GeoReject* record);

/**
 * 更新IP拒绝记录
 *
 * @param manager 数据库连接管理器
 * @param record 新的拒绝记录
 * @return 0=成功, -1=失败
 */
int GeoReject_Update(DBConnectionManager* manager, const GeoReject* record);

/**
 * 删除IP拒绝记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param rej_ip IP地址
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int GeoReject_Delete(DBConnectionManager* manager, const char* rej_ip);

/**
 * 检查IP是否在黑名单中
 *
 * @param manager 数据库连接管理器
 * @param rej_ip IP地址
 * @return 1=存在, 0=不存在
 */
int GeoReject_Exists(DBConnectionManager* manager, const char* rej_ip);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据国家代码查询拒绝IP列表
 *
 * @param manager 数据库连接管理器
 * @param country_code 国家代码
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetByCountryCode(DBConnectionManager* manager, const char* country_code,
                                 GeoReject* records, int max_count);

/**
 * 根据来源查询拒绝IP列表
 *
 * @param manager 数据库连接管理器
 * @param source 来源 ('w'=web, 'g'=game)
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetBySource(DBConnectionManager* manager, char source,
                           GeoReject* records, int max_count);

/**
 * 根据检查标志查询拒绝IP列表
 *
 * @param manager 数据库连接管理器
 * @param check_flag 检查标志 ('Y'/'N')
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetByCheckFlag(DBConnectionManager* manager, char check_flag,
                               GeoReject* records, int max_count);

/**
 * 根据访问次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_count 最小访问次数
 * @param max_count_val 最大访问次数
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetByCountRange(DBConnectionManager* manager, int min_count, int max_count_val,
                                GeoReject* records, int max_count);

/**
 * 根据IP模式查询（支持LIKE）
 *
 * @param manager 数据库连接管理器
 * @param ip_pattern IP模式 (例如: "192.168.%")
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetByIPPattern(DBConnectionManager* manager, const char* ip_pattern,
                               GeoReject* records, int max_count);

/**
 * 获取最近更新的记录
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetRecentUpdated(DBConnectionManager* manager, GeoReject* records, int limit);

/**
 * 获取所有拒绝记录（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int GeoReject_GetAll(DBConnectionManager* manager, GeoReject* records, int offset, int limit);

/**
 * 获取拒绝记录总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int GeoReject_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加拒绝记录
 *
 * @param manager 数据库连接管理器
 * @param records 拒绝记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int GeoReject_BulkAdd(DBConnectionManager* manager, const GeoReject* records, int count);

/**
 * 批量删除拒绝记录
 *
 * @param manager 数据库连接管理器
 * @param ip_list IP地址数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int GeoReject_BulkDelete(DBConnectionManager* manager, const char** ip_list, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印拒绝记录信息（用于调试）
 *
 * @param record 拒绝记录
 */
void GeoReject_PrintInfo(const GeoReject* record);

#endif /* GEO_REJECT_ORM_H */
