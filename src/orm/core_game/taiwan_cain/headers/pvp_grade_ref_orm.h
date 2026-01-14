#ifndef PVP_GRADE_REF_ORM_H
#define PVP_GRADE_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * PVP Grade Ref ORM - PVP等级配置系统
 *
 * 表: pvp_grade_ref (taiwan_cain 数据库)
 * 描述: PVP等级与所需积分对照表
 * 主键: grade
 *
 * 字段说明:
 * - grade: PVP等级 (INT, PRIMARY KEY)
 * - limit_pts: 该等级所需积分下限 (INT)
 *
 * 业务用途:
 * - 定义PVP等级与积分的对应关系
 * - 用于判定玩家PVP等级晋升
 * - 配置PVP排名系统
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 3个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #57/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* PVP等级配置记录结构 */
typedef struct {
    int grade;         /* PVP等级 (主键) */
    int limit_pts;     /* 该等级所需积分下限 */
} PvpGradeRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新PVP等级配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int PvpGradeRef_Add(DBConnectionManager* manager, const PvpGradeRef* record);

/**
 * 获取指定等级的配置
 * @param manager 数据库连接管理器
 * @param grade PVP等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int PvpGradeRef_Get(DBConnectionManager* manager, int grade, PvpGradeRef* record);

/**
 * 更新PVP等级配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int PvpGradeRef_Update(DBConnectionManager* manager, const PvpGradeRef* record);

/**
 * 删除PVP等级配置
 * @param manager 数据库连接管理器
 * @param grade PVP等级
 * @return 成功返回0，失败返回-1
 */
int PvpGradeRef_Delete(DBConnectionManager* manager, int grade);

/**
 * 检查PVP等级配置是否存在
 * @param manager 数据库连接管理器
 * @param grade PVP等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int PvpGradeRef_Exists(DBConnectionManager* manager, int grade);

/*******************************************************************************
 * 查询操作 (3个函数)
 ******************************************************************************/

/**
 * 获取配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int PvpGradeRef_Count(DBConnectionManager* manager);

/**
 * 获取所有等级配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int PvpGradeRef_GetAll(DBConnectionManager* manager, PvpGradeRef* records, int max_records);

/**
 * 根据积分查找对应等级（找到积分所属的最高等级）
 * @param manager 数据库连接管理器
 * @param points 当前积分
 * @param record 输出参数，存储匹配的等级配置
 * @return 成功返回0，未找到或失败返回-1
 */
int PvpGradeRef_GetByPoints(DBConnectionManager* manager, int points, PvpGradeRef* record);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印PVP等级配置信息（用于调试）
 * @param record PVP等级配置记录指针
 */
void PvpGradeRef_PrintInfo(const PvpGradeRef* record);

#endif /* PVP_GRADE_REF_ORM_H */
