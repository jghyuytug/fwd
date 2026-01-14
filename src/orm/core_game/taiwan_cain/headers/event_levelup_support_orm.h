#ifndef EVENT_LEVELUP_SUPPORT_ORM_H
#define EVENT_LEVELUP_SUPPORT_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Event Levelup Support ORM - 升级支援活动系统
 *
 * 表: event_levelup_support (taiwan_cain 数据库)
 * 描述: 升级支援活动进度记录表
 * 唯一标识: 复合主键 (charac_no, level)
 *
 * 字段说明:
 * - charac_no: 角色编号 (INT, 复合主键)
 * - level: 等级 (INT, 复合主键)
 * - state: 状态 (INT, nullable)
 *
 * 业务用途:
 * - 记录角色在升级支援活动中各等级的领取状态
 * - 支持按角色和等级查询活动进度
 * - 用于活动奖励发放控制
 *
 * 注意事项:
 * - 使用复合主键 (charac_no, level)
 * - state 字段可为 NULL
 *
 * API设计: 12个函数
 * - CRUD操作: 5个（需传入复合主键）
 * - 专项操作: 2个
 * - 查询操作: 4个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #100/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 升级支援活动记录结构 */
typedef struct {
    int charac_no;    /* 角色编号 (复合主键) */
    int level;        /* 等级 (复合主键) */
    int state;        /* 状态 */
} EventLevelupSupport;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新升级支援活动记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int EventLevelupSupport_Add(DBConnectionManager* manager, const EventLevelupSupport* record);

/**
 * 获取指定角色和等级的升级支援活动记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param level 等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int EventLevelupSupport_Get(DBConnectionManager* manager, int charac_no, int level, 
                            EventLevelupSupport* record);

/**
 * 更新升级支援活动记录
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int EventLevelupSupport_Update(DBConnectionManager* manager, const EventLevelupSupport* record);

/**
 * 删除升级支援活动记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param level 等级
 * @return 成功返回0，失败返回-1
 */
int EventLevelupSupport_Delete(DBConnectionManager* manager, int charac_no, int level);

/**
 * 检查升级支援活动记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param level 等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int EventLevelupSupport_Exists(DBConnectionManager* manager, int charac_no, int level);

/*******************************************************************************
 * 专项操作 (2个函数)
 ******************************************************************************/

/**
 * 更新状态
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param level 等级
 * @param state 新状态值
 * @return 成功返回0，失败返回-1
 */
int EventLevelupSupport_UpdateState(DBConnectionManager* manager, int charac_no, int level, int state);

/**
 * 批量删除指定角色的所有记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int EventLevelupSupport_DeleteByCharac(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取指定角色的所有记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int EventLevelupSupport_GetByCharac(DBConnectionManager* manager, int charac_no,
                                    EventLevelupSupport* records, int max_records);

/**
 * 获取指定等级的所有记录
 * @param manager 数据库连接管理器
 * @param level 等级
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int EventLevelupSupport_GetByLevel(DBConnectionManager* manager, int level,
                                   EventLevelupSupport* records, int max_records);

/**
 * 获取指定状态的所有记录
 * @param manager 数据库连接管理器
 * @param state 状态
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int EventLevelupSupport_GetByState(DBConnectionManager* manager, int state,
                                   EventLevelupSupport* records, int max_records);

/**
 * 统计指定角色的记录数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return >=0记录数，<0失败
 */
int EventLevelupSupport_CountByCharac(DBConnectionManager* manager, int charac_no);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印升级支援活动信息（用于调试）
 * @param record 升级支援活动记录指针
 */
void EventLevelupSupport_PrintInfo(const EventLevelupSupport* record);

#endif /* EVENT_LEVELUP_SUPPORT_ORM_H */
