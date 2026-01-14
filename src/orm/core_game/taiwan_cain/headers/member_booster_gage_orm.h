#ifndef MEMBER_BOOSTER_GAGE_ORM_H
#define MEMBER_BOOSTER_GAGE_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Member Booster Gage ORM - 会员增益量表管理
 *
 * 表: member_booster_gage (taiwan_cain 数据库)
 * 描述: 会员增益量表/能量值追踪系统
 * 主键: m_id (会员ID)
 *
 * 字段说明:
 * - m_id: 会员ID (UNSIGNED INT, 主键)
 * - gage: 增益量表/能量值 (TINYINT UNSIGNED, 0-255)
 *
 * 业务用途:
 * - 追踪会员的增益量表状态
 * - 管理能量值系统（可能与增益道具/Buff相关）
 * - 支持量表的原子递增/递减操作
 *
 * API设计: 17个函数
 * - CRUD操作: 5个
 * - 业务查询: 9个
 * - 批量操作: 2个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-14
 * ORM模块编号: #38/156
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* Member Booster Gage 记录结构 */
typedef struct {
    unsigned int m_id;          /* 会员ID (主键) */
    unsigned char gage;         /* 增益量表 (0-255) */
} MemberBoosterGage;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或替换会员增益量表记录
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员增益量表记录指针
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_Add(DBConnectionManager* manager, const MemberBoosterGage* record);

/**
 * 获取会员增益量表记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int MemberBoosterGage_Get(DBConnectionManager* manager, unsigned int m_id, MemberBoosterGage* record);

/**
 * 更新会员增益量表值
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param new_gage 新的量表值 (0-255)
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_Update(DBConnectionManager* manager, unsigned int m_id, unsigned char new_gage);

/**
 * 删除会员增益量表记录
 * 幂等操作：无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 始终返回0（DELETE操作幂等）
 */
int MemberBoosterGage_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查会员增益量表记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int MemberBoosterGage_Exists(DBConnectionManager* manager, unsigned int m_id);

/*******************************************************************************
 * 业务查询 (9个函数)
 ******************************************************************************/

/**
 * 按增益量表范围查询会员
 *
 * @param manager 数据库连接管理器
 * @param min_gage 最小量表值
 * @param max_gage 最大量表值
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetByGageRange(DBConnectionManager* manager,
                                      unsigned char min_gage, unsigned char max_gage,
                                      MemberBoosterGage* records, int max_records);

/**
 * 查询增益量表值大于等于指定值的会员
 *
 * @param manager 数据库连接管理器
 * @param min_gage 最小量表值阈值
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetByMinGage(DBConnectionManager* manager,
                                    unsigned char min_gage,
                                    MemberBoosterGage* records, int max_records);

/**
 * 查询特定增益量表值的所有会员
 *
 * @param manager 数据库连接管理器
 * @param gage_value 目标量表值
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetByGageValue(DBConnectionManager* manager,
                                      unsigned char gage_value,
                                      MemberBoosterGage* records, int max_records);

/**
 * 查询增益量表为0的会员（无增益状态）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetZeroGage(DBConnectionManager* manager,
                                   MemberBoosterGage* records, int max_records);

/**
 * 查询增益量表非0的会员（有增益状态）
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetNonZeroGage(DBConnectionManager* manager,
                                      MemberBoosterGage* records, int max_records);

/**
 * 获取增益量表最高的TOP N会员
 * 按 gage DESC 排序
 *
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param top_n 返回前N条记录
 * @return 实际返回的记录数，失败返回-1
 */
int MemberBoosterGage_GetTopGageMembers(DBConnectionManager* manager,
                                         MemberBoosterGage* records, int top_n);

/**
 * 原子递增会员增益量表（+1）
 * 使用 UPDATE ... SET gage = gage + 1 确保原子性
 * 不会超过 255（TINYINT UNSIGNED 最大值）
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_IncrementGage(DBConnectionManager* manager, unsigned int m_id);

/**
 * 原子递减会员增益量表（-1）
 * 使用 UPDATE ... SET gage = IF(gage > 0, gage - 1, 0) 确保不会小于0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_DecrementGage(DBConnectionManager* manager, unsigned int m_id);

/**
 * 批量设置所有符合条件的会员的量表值
 * 示例: 将所有量表为0的会员设置为初始值10
 *
 * @param manager 数据库连接管理器
 * @param old_gage 原量表值（条件）
 * @param new_gage 新量表值
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_SetGageValue(DBConnectionManager* manager,
                                    unsigned char old_gage, unsigned char new_gage);

/*******************************************************************************
 * 批量操作 (2个函数)
 ******************************************************************************/

/**
 * 批量添加会员增益量表记录
 * 使用事务确保原子性
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 记录数量
 * @return 成功添加的记录数，失败返回-1
 */
int MemberBoosterGage_BulkAdd(DBConnectionManager* manager,
                               const MemberBoosterGage* records, int count);

/**
 * 批量删除多个会员的增益量表记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 会员数量
 * @return 成功返回0，失败返回-1
 */
int MemberBoosterGage_BulkDeleteByMemberIds(DBConnectionManager* manager,
                                              const unsigned int* m_ids, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印会员增益量表记录信息（用于调试）
 *
 * @param record 会员增益量表记录指针
 */
void MemberBoosterGage_PrintInfo(const MemberBoosterGage* record);

#endif /* MEMBER_BOOSTER_GAGE_ORM_H */
