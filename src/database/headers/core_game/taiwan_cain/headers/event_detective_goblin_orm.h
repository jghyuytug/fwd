#ifndef EVENT_DETECTIVE_GOBLIN_ORM_H
#define EVENT_DETECTIVE_GOBLIN_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * SP Reward ORM - 侦探哥布林活动积分系统
 *
 * 表: event_detective_goblin (taiwan_cain 数据库)
 * 描述: 积分奖励配置表
 * 唯一标识: charac_no (角色编号)
 *
 * 字段说明:
 * - charac_no: 角色编号 (INT, 唯一标识)
 * - point: 积分数量 (INT)
 *
 * 业务用途:
 * - 存储不同角色编号的侦探哥布林活动积分配置
 * - 只读配置表，用于角色升级时发放SP
 * - 支持角色编号-SP映射关系
 *
 * 注意事项:
 * - 表无主键，使用charac_no作为唯一标识
 * - 配置表，主要用于查询操作
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #99/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 侦探哥布林活动积分记录结构 */
typedef struct {
    int charac_no;    /* 角色编号 (唯一标识) */
    int point;       /* 积分数量 */
} EventDetectiveGoblin;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新侦探哥布林活动积分配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int EventDetectiveGoblin_Add(DBConnectionManager* manager, const EventDetectiveGoblin* record);

/**
 * 获取指定角色编号的侦探哥布林活动积分
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int EventDetectiveGoblin_Get(DBConnectionManager* manager, int charac_no, EventDetectiveGoblin* record);

/**
 * 更新侦探哥布林活动积分配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int EventDetectiveGoblin_Update(DBConnectionManager* manager, const EventDetectiveGoblin* record);

/**
 * 删除侦探哥布林活动积分配置
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int EventDetectiveGoblin_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查侦探哥布林活动积分配置是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int EventDetectiveGoblin_Exists(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取侦探哥布林活动积分配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int EventDetectiveGoblin_Count(DBConnectionManager* manager);

/**
 * 获取所有侦探哥布林活动积分配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int EventDetectiveGoblin_GetAll(DBConnectionManager* manager, EventDetectiveGoblin* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除侦探哥布林活动积分配置
 * @param manager 数据库连接管理器
 * @param charac_nos 角色编号数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int EventDetectiveGoblin_BatchDelete(DBConnectionManager* manager, const int* charac_nos, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印侦探哥布林活动积分信息（用于调试）
 * @param record 侦探哥布林活动积分记录指针
 */
void EventDetectiveGoblin_PrintInfo(const EventDetectiveGoblin* record);

#endif /* EVENT_DETECTIVE_GOBLIN_ORM_H */
