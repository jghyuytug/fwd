#ifndef MEMBER_JOIN_INFO_ORM_H
#define MEMBER_JOIN_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * member_join_info - 会员注册信息表
 *
 * 表结构:
 *   m_id              INT(10) UNSIGNED    主键 - 会员ID
 *   reg_date          INT(11)             注册日期（时间戳）
 *   ip                VARCHAR(15)         注册IP地址
 *   contry_code       TINYINT(3) UNSIGNED 国家代码
 *   login_time        INT(11)             登录时间（时间戳）
 *   error_type        TINYINT(3) UNSIGNED 错误类型
 *   login_ip          VARCHAR(15)         登录IP地址
 *   game_use_history  TINYINT(3) UNSIGNED 游戏使用历史
 */

/* 结构体定义 */
typedef struct {
    unsigned int m_id;              /* 会员ID (主键) */
    int reg_date;                   /* 注册日期（时间戳） */
    char ip[16];                    /* 注册IP地址 */
    unsigned char contry_code;      /* 国家代码 */
    int login_time;                 /* 登录时间（时间戳） */
    unsigned char error_type;       /* 错误类型 */
    char login_ip[16];              /* 登录IP地址 */
    unsigned char game_use_history; /* 游戏使用历史 */
} MemberJoinInfo;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加会员注册信息（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员注册信息
 * @return 0=成功, -1=失败
 */
int MemberJoinInfo_Add(DBConnectionManager* manager, const MemberJoinInfo* record);

/**
 * 根据会员ID获取注册信息
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 0=成功, -1=失败或不存在
 */
int MemberJoinInfo_Get(DBConnectionManager* manager, unsigned int m_id, MemberJoinInfo* record);

/**
 * 更新会员注册信息
 *
 * @param manager 数据库连接管理器
 * @param record 新的会员注册信息
 * @return 0=成功, -1=失败
 */
int MemberJoinInfo_Update(DBConnectionManager* manager, const MemberJoinInfo* record);

/**
 * 删除会员注册信息（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberJoinInfo_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查会员注册信息是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberJoinInfo_Exists(DBConnectionManager* manager, unsigned int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 根据注册IP查询会员
 *
 * @param manager 数据库连接管理器
 * @param ip 注册IP地址
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetByRegIP(DBConnectionManager* manager, const char* ip,
                                MemberJoinInfo* records, int max_count);

/**
 * 根据国家代码查询会员
 *
 * @param manager 数据库连接管理器
 * @param contry_code 国家代码
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetByCountryCode(DBConnectionManager* manager, unsigned char contry_code,
                                      MemberJoinInfo* records, int max_count);

/**
 * 根据注册日期范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_date 起始时间戳
 * @param end_date 结束时间戳
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetByRegDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                       MemberJoinInfo* records, int max_count);

/**
 * 根据错误类型查询
 *
 * @param manager 数据库连接管理器
 * @param error_type 错误类型
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetByErrorType(DBConnectionManager* manager, unsigned char error_type,
                                    MemberJoinInfo* records, int max_count);

/**
 * 根据游戏使用历史查询
 *
 * @param manager 数据库连接管理器
 * @param game_use_history 游戏使用历史
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetByGameUseHistory(DBConnectionManager* manager, unsigned char game_use_history,
                                         MemberJoinInfo* records, int max_count);

/**
 * 获取最近注册的会员
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetRecentRegistered(DBConnectionManager* manager, MemberJoinInfo* records,
                                         int limit);

/**
 * 获取所有会员注册信息（分页）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param offset 偏移量
 * @param limit 返回数量限制
 * @return 实际查询到的记录数, -1=失败
 */
int MemberJoinInfo_GetAll(DBConnectionManager* manager, MemberJoinInfo* records,
                           int offset, int limit);

/**
 * 获取会员注册信息总数
 *
 * @param manager 数据库连接管理器
 * @return 总记录数, -1=失败
 */
int MemberJoinInfo_GetCount(DBConnectionManager* manager);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加会员注册信息
 *
 * @param manager 数据库连接管理器
 * @param records 会员注册信息数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberJoinInfo_BulkAdd(DBConnectionManager* manager, const MemberJoinInfo* records,
                            int count);

/**
 * 批量删除会员注册信息
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberJoinInfo_BulkDelete(DBConnectionManager* manager, const unsigned int* m_ids,
                               int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印会员注册信息（用于调试）
 *
 * @param record 会员注册信息
 */
void MemberJoinInfo_PrintInfo(const MemberJoinInfo* record);

#endif /* MEMBER_JOIN_INFO_ORM_H */
