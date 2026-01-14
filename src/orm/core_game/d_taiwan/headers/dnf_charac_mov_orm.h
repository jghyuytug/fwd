#ifndef DNF_CHARAC_MOV_ORM_H
#define DNF_CHARAC_MOV_ORM_H

#include "db_connection_manager.h"

/**
 * dnf_charac_mov - DNF角色移动（转服）记录表
 *
 * 表结构：
 * - id (int, auto_increment): 自增主键
 * - m_id (int): 会员ID
 * - server_id (tinyint): 原服务器ID
 * - charac_no (int): 原角色编号
 * - move_server_id (tinyint): 目标服务器ID
 * - move_charac_no (int): 目标角色编号
 * - move_check (int): 移动检查状态
 *
 * 唯一索引：(m_id, server_id, charac_no)
 *
 * 业务说明：
 * - 记录角色跨服务器转移的历史
 * - 支持角色转服验证和追踪
 * - move_check 用于标记转服状态和校验结果
 */

/* 数据结构定义 */
typedef struct {
    int id;                     /* 自增主键 */
    int m_id;                   /* 会员ID */
    int server_id;              /* 原服务器ID */
    int charac_no;              /* 原角色编号 */
    int move_server_id;         /* 目标服务器ID */
    int move_charac_no;         /* 目标角色编号 */
    int move_check;             /* 移动检查状态 */
} DnfCharacMov;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加角色转服记录
 * @param manager 数据库连接管理器
 * @param record 转服记录
 * @return 新插入的id值（成功）, <0（失败）
 */
int DnfCharacMov_Add(DBConnectionManager* manager, const DnfCharacMov* record);

/**
 * 根据id查询转服记录
 * @param manager 数据库连接管理器
 * @param id 记录ID
 * @param record 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_Get(DBConnectionManager* manager, int id, DnfCharacMov* record);

/**
 * 根据角色信息查询转服记录（唯一索引查询）
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param server_id 原服务器ID
 * @param charac_no 原角色编号
 * @param record 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_GetByCharacter(DBConnectionManager* manager, int m_id, int server_id, int charac_no, DnfCharacMov* record);

/**
 * 更新转服记录
 * @param manager 数据库连接管理器
 * @param record 转服记录
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_Update(DBConnectionManager* manager, const DnfCharacMov* record);

/**
 * 删除转服记录
 * @param manager 数据库连接管理器
 * @param id 记录ID
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_Delete(DBConnectionManager* manager, int id);

/**
 * 检查转服记录是否存在
 * @param manager 数据库连接管理器
 * @param id 记录ID
 * @return 1=存在, 0=不存在, <0=错误
 */
int DnfCharacMov_Exists(DBConnectionManager* manager, int id);

/* ==================== 专项操作 ==================== */

/**
 * 更新移动检查状态
 * @param manager 数据库连接管理器
 * @param id 记录ID
 * @param move_check 新的检查状态
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_UpdateMoveCheck(DBConnectionManager* manager, int id, int move_check);

/**
 * 查询会员的所有转服记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出记录数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际返回的记录数, <0=失败
 */
int DnfCharacMov_GetByMid(DBConnectionManager* manager, int m_id, DnfCharacMov* records, int max_count);

/**
 * 查询从某服务器转出的记录
 * @param manager 数据库连接管理器
 * @param server_id 原服务器ID
 * @param records 输出记录数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际返回的记录数, <0=失败
 */
int DnfCharacMov_GetBySourceServer(DBConnectionManager* manager, int server_id, DnfCharacMov* records, int max_count);

/**
 * 查询转入某服务器的记录
 * @param manager 数据库连接管理器
 * @param move_server_id 目标服务器ID
 * @param records 输出记录数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际返回的记录数, <0=失败
 */
int DnfCharacMov_GetByTargetServer(DBConnectionManager* manager, int move_server_id, DnfCharacMov* records, int max_count);

/* ==================== 查询操作 ==================== */

/**
 * 统计会员的转服次数
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 转服次数, <0=错误
 */
int DnfCharacMov_CountByMid(DBConnectionManager* manager, int m_id);

/**
 * 按检查状态统计记录数
 * @param manager 数据库连接管理器
 * @param move_check 移动检查状态
 * @return 记录数量, <0=错误
 */
int DnfCharacMov_CountByMoveCheck(DBConnectionManager* manager, int move_check);

/**
 * 统计总转服记录数
 * @param manager 数据库连接管理器
 * @return 总记录数, <0=错误
 */
int DnfCharacMov_CountTotal(DBConnectionManager* manager);

/* ==================== 批量操作 ==================== */

/**
 * 批量删除转服记录（按id列表）
 * @param manager 数据库连接管理器
 * @param ids id数组
 * @param count 数组长度
 * @return 0=成功, <0=失败
 */
int DnfCharacMov_BatchDelete(DBConnectionManager* manager, const int* ids, int count);

/* ==================== 工具函数 ==================== */

/**
 * 打印转服记录信息（调试用）
 * @param record 转服记录
 */
void DnfCharacMov_PrintInfo(const DnfCharacMov* record);

#endif /* DNF_CHARAC_MOV_ORM_H */
