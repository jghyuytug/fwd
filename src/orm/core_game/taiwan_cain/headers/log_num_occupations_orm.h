#ifndef LOG_NUM_OCCUPATIONS_ORM_H
#define LOG_NUM_OCCUPATIONS_ORM_H

#include "db_connection_manager.h"

/**
 * 服务器占用数统计 ORM
 *
 * 功能：
 * - 记录服务器各区域占用人数
 * - 追踪每分钟登录登出统计
 * - 支持时序数据分析和趋势监控
 * - 运营数据报表生成
 *
 * 主键：
 * - PRIMARY KEY (occ_time) - 记录时间唯一主键
 *
 * 字段说明：
 * - num_occupations_charscreen: 角色选择界面占用数
 * - num_occupations_seriaroom: Seria房间占用数
 * - num_login_per_min: 每分钟登录数
 * - num_logout_per_min: 每分钟登出数
 */

/* 服务器占用数记录结构 */
typedef struct {
    char occ_time[20];                          /* 记录时间（YYYY-MM-DD HH:MM:SS，主键） */
    unsigned int num_occupations_charscreen;    /* 角色选择界面占用数 */
    unsigned int num_occupations_seriaroom;     /* Seria房间占用数 */
    unsigned int num_login_per_min;             /* 每分钟登录数 */
    unsigned int num_logout_per_min;            /* 每分钟登出数 */
} LogNumOccupations;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加服务器占用数记录（使用 REPLACE）
 *
 * @param manager 数据库连接管理器
 * @param record 占用数记录
 * @return 0 成功, <0 失败
 */
int LogNumOccupations_Add(DBConnectionManager* manager, const LogNumOccupations* record);

/**
 * 获取服务器占用数记录（按记录时间）
 *
 * @param manager 数据库连接管理器
 * @param occ_time 记录时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int LogNumOccupations_Get(DBConnectionManager* manager, const char* occ_time, LogNumOccupations* record);

/**
 * 更新服务器占用数记录（按记录时间）
 *
 * @param manager 数据库连接管理器
 * @param record 占用数记录（必须包含有效的occ_time）
 * @return 0 成功, <0 失败
 */
int LogNumOccupations_Update(DBConnectionManager* manager, const LogNumOccupations* record);

/**
 * 删除服务器占用数记录（按记录时间）
 *
 * @param manager 数据库连接管理器
 * @param occ_time 记录时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return 0 成功, <0 失败
 */
int LogNumOccupations_Delete(DBConnectionManager* manager, const char* occ_time);

/**
 * 检查服务器占用数记录是否存在（按记录时间）
 *
 * @param manager 数据库连接管理器
 * @param occ_time 记录时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return 1 存在, 0 不存在, <0 错误
 */
int LogNumOccupations_Exists(DBConnectionManager* manager, const char* occ_time);

/* ==================== 专项操作 ==================== */

/**
 * 获取最新记录
 *
 * @param manager 数据库连接管理器
 * @param record 输出参数，接收最新记录
 * @return 0 成功, <0 失败或无记录
 */
int LogNumOccupations_GetLatest(DBConnectionManager* manager, LogNumOccupations* record);

/**
 * 按时间范围删除记录
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 删除的记录数, <0 失败
 */
int LogNumOccupations_DeleteByTimeRange(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 批量删除过期记录（保留最近N天）
 *
 * @param manager 数据库连接管理器
 * @param days 保留天数（删除早于此天数的记录）
 * @return >=0 删除的记录数, <0 失败
 */
int LogNumOccupations_DeleteOlderThan(DBConnectionManager* manager, int days);

/* ==================== 查询操作 ==================== */

/**
 * 统计总记录数
 *
 * @param manager 数据库连接管理器
 * @return >=0 总记录数, <0 失败
 */
int LogNumOccupations_CountAll(DBConnectionManager* manager);

/**
 * 按时间范围查询平均角色选择界面占用数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 平均占用数, <0 失败
 */
int LogNumOccupations_GetAvgCharscreen(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询平均Seria房间占用数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 平均占用数, <0 失败
 */
int LogNumOccupations_GetAvgSeriaroom(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询最高角色选择界面占用数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 最高占用数, <0 失败
 */
int LogNumOccupations_GetMaxCharscreen(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询最高Seria房间占用数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 最高占用数, <0 失败
 */
int LogNumOccupations_GetMaxSeriaroom(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询登录登出比率（login/logout）
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 登录登出比率(*100), <0 失败
 */
int LogNumOccupations_GetLoginLogoutRatio(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询总登录数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 总登录数, <0 失败
 */
int LogNumOccupations_GetTotalLogins(DBConnectionManager* manager, const char* start_time, const char* end_time);

/**
 * 按时间范围查询总登出数
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间（格式：YYYY-MM-DD HH:MM:SS）
 * @param end_time 结束时间（格式：YYYY-MM-DD HH:MM:SS）
 * @return >=0 总登出数, <0 失败
 */
int LogNumOccupations_GetTotalLogouts(DBConnectionManager* manager, const char* start_time, const char* end_time);

/* ==================== 工具函数 ==================== */

/**
 * 打印服务器占用数记录（调试用）
 *
 * @param record 占用数记录
 */
void LogNumOccupations_PrintInfo(const LogNumOccupations* record);

#endif /* LOG_NUM_OCCUPATIONS_ORM_H */
