#ifndef SERVER_STATE_INFO_ORM_H
#define SERVER_STATE_INFO_ORM_H

#include "db_connection_manager.h"

/* 状态数据最大长度 */
#define SERVER_STATE_SIZE 12

/* server_state_info 表数据结构 */
typedef struct {
    int category;                      /* 分类（主键1）*/
    int code;                          /* 代码（主键2）*/
    unsigned char state[SERVER_STATE_SIZE];  /* 状态数据（12字节二进制）*/
    char start_time[20];               /* 开始时间（YYYY-MM-DD HH:MM:SS）*/
    char end_time[20];                 /* 结束时间（YYYY-MM-DD HH:MM:SS）*/
} ServerStateInfo;

/* ========== CRUD 操作 ========== */

/**
 * 添加或更新服务器状态信息
 * @param manager 数据库连接管理器
 * @param record 服务器状态记录
 * @return 0成功，-1失败
 */
int ServerStateInfo_Add(DBConnectionManager* manager, const ServerStateInfo* record);

/**
 * 获取服务器状态信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @param record 输出的服务器状态记录
 * @return 0成功，-1失败
 */
int ServerStateInfo_Get(DBConnectionManager* manager, int category, int code, ServerStateInfo* record);

/**
 * 更新服务器状态信息
 * @param manager 数据库连接管理器
 * @param record 服务器状态记录
 * @return 0成功，-1失败
 */
int ServerStateInfo_Update(DBConnectionManager* manager, const ServerStateInfo* record);

/**
 * 删除服务器状态信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @return 0成功，-1失败
 */
int ServerStateInfo_Delete(DBConnectionManager* manager, int category, int code);

/**
 * 检查服务器状态信息是否存在
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @return 1存在，0不存在，-1错误
 */
int ServerStateInfo_Exists(DBConnectionManager* manager, int category, int code);

/* ========== 专项操作 ========== */

/**
 * 更新状态数据
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @param state 新状态数据
 * @return 0成功，-1失败
 */
int ServerStateInfo_UpdateState(DBConnectionManager* manager, int category, int code, const unsigned char* state);

/**
 * 更新时间范围
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @param start_time 开始时间
 * @param end_time 结束时间
 * @return 0成功，-1失败
 */
int ServerStateInfo_UpdateTimeRange(DBConnectionManager* manager, int category, int code,
                                    const char* start_time, const char* end_time);

/**
 * 延长结束时间
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param code 代码
 * @param seconds 延长的秒数
 * @return 0成功，-1失败
 */
int ServerStateInfo_ExtendEndTime(DBConnectionManager* manager, int category, int code, int seconds);

/* ========== 查询操作 ========== */

/**
 * 获取指定分类的所有状态信息数量
 * @param manager 数据库连接管理器
 * @param category 分类
 * @return 记录数量，-1失败
 */
int ServerStateInfo_CountByCategory(DBConnectionManager* manager, int category);

/**
 * 获取当前有效的状态信息数量（结束时间未到）
 * @param manager 数据库连接管理器
 * @return 记录数量，-1失败
 */
int ServerStateInfo_CountActive(DBConnectionManager* manager);

/**
 * 删除指定分类的所有状态信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @return 0成功，-1失败
 */
int ServerStateInfo_DeleteByCategory(DBConnectionManager* manager, int category);

/**
 * 删除过期的状态信息（结束时间已过）
 * @param manager 数据库连接管理器
 * @return 0成功，-1失败
 */
int ServerStateInfo_DeleteExpired(DBConnectionManager* manager);

/**
 * 获取所有服务器状态信息总数
 * @param manager 数据库连接管理器
 * @return 记录总数，-1失败
 */
int ServerStateInfo_Count(DBConnectionManager* manager);

/* ========== 工具函数 ========== */

/**
 * 打印服务器状态信息（调试用）
 * @param record 服务器状态记录
 */
void ServerStateInfo_PrintInfo(const ServerStateInfo* record);

#endif /* SERVER_STATE_INFO_ORM_H */
