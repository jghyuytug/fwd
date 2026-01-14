#ifndef EXP_LEVEL_REF_ORM_H
#define EXP_LEVEL_REF_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Exp Level Ref ORM - 经验等级参考系统
 *
 * 表: exp_level_ref (taiwan_cain 数据库)
 * 描述: 经验等级参考配置表
 * 唯一标识: lev (等级)
 *
 * 字段说明:
 * - exp: 经验值 (UNSIGNED INT)
 * - lev: 等级 (UNSIGNED INT, 唯一标识)
 *
 * 业务用途:
 * - 存储等级与经验值的映射关系
 * - 用于角色升级判定
 * - 支持经验计算和等级查询
 *
 * 注意事项:
 * - 表无主键，使用lev作为唯一标识
 * - 配置表，主要用于查询操作
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #52/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 经验等级参考记录结构 */
typedef struct {
    unsigned int exp;    /* 经验值 */
    unsigned int lev;    /* 等级 (唯一标识) */
} ExpLevelRef;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新经验等级配置（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int ExpLevelRef_Add(DBConnectionManager* manager, const ExpLevelRef* record);

/**
 * 获取指定等级的经验值配置
 * @param manager 数据库连接管理器
 * @param lev 等级
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int ExpLevelRef_Get(DBConnectionManager* manager, unsigned int lev, ExpLevelRef* record);

/**
 * 更新经验等级配置
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int ExpLevelRef_Update(DBConnectionManager* manager, const ExpLevelRef* record);

/**
 * 删除经验等级配置
 * @param manager 数据库连接管理器
 * @param lev 等级
 * @return 成功返回0，失败返回-1
 */
int ExpLevelRef_Delete(DBConnectionManager* manager, unsigned int lev);

/**
 * 检查经验等级配置是否存在
 * @param manager 数据库连接管理器
 * @param lev 等级
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int ExpLevelRef_Exists(DBConnectionManager* manager, unsigned int lev);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取经验等级配置总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int ExpLevelRef_Count(DBConnectionManager* manager);

/**
 * 获取所有经验等级配置
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ExpLevelRef_GetAll(DBConnectionManager* manager, ExpLevelRef* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除经验等级配置
 * @param manager 数据库连接管理器
 * @param levs 等级数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int ExpLevelRef_BatchDelete(DBConnectionManager* manager, const unsigned int* levs, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印经验等级信息（用于调试）
 * @param record 经验等级记录指针
 */
void ExpLevelRef_PrintInfo(const ExpLevelRef* record);

#endif /* EXP_LEVEL_REF_ORM_H */
