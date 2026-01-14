#ifndef CHARAC_RIDABLE_STAT_ORM_H
#define CHARAC_RIDABLE_STAT_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Ridable Stat ORM - 角色坐骑属性系统
 *
 * 表: charac_ridable_stat (taiwan_cain 数据库)
 * 描述: 角色坐骑冷却时间等属性数据
 * 主键: charac_no (角色编号)
 *
 * 字段说明:
 * - charac_no: 角色编号 (UNSIGNED INT, 主键)
 * - cooltime: 冷却时间数据 (BLOB)
 *
 * 业务用途:
 * - 存储角色坐骑的冷却时间状态
 * - 管理坐骑相关的二进制属性数据
 * - 支持坐骑技能冷却控制
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 2个
 * - 工具函数: 2个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #47/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_COOLTIME_LEN 8192  /* BLOB字段最大长度 */

/* 角色坐骑属性记录结构 */
typedef struct {
    unsigned int charac_no;                      /* 角色编号 (主键) */
    unsigned char cooltime[MAX_COOLTIME_LEN];   /* 冷却时间数据 (BLOB) */
    int cooltime_len;                            /* 冷却时间数据实际长度 */
} CharacRidableStat;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新角色坐骑属性记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacRidableStat_Add(DBConnectionManager* manager, const CharacRidableStat* record);

/**
 * 获取角色坐骑属性记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacRidableStat_Get(DBConnectionManager* manager, unsigned int charac_no, CharacRidableStat* record);

/**
 * 更新角色坐骑冷却时间
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int CharacRidableStat_Update(DBConnectionManager* manager, const CharacRidableStat* record);

/**
 * 删除角色坐骑属性记录
 * 幂等操作：无论记录是否存在都返回0
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 始终返回0（DELETE操作幂等）
 */
int CharacRidableStat_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色坐骑属性记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacRidableStat_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取坐骑数据记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int CharacRidableStat_Count(DBConnectionManager* manager);

/**
 * 获取所有坐骑数据记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacRidableStat_GetAll(DBConnectionManager* manager, CharacRidableStat* records, int max_records);

/*******************************************************************************
 * 批量操作 (2个函数)
 ******************************************************************************/

/**
 * 清空角色坐骑冷却时间（保留记录但清空数据）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacRidableStat_ClearCooltime(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 批量删除角色坐骑数据
 * @param manager 数据库连接管理器
 * @param charac_nos 角色编号数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int CharacRidableStat_BatchDelete(DBConnectionManager* manager,
                                  const unsigned int* charac_nos, int count);

/*******************************************************************************
 * 工具函数 (2个函数)
 ******************************************************************************/

/**
 * 检查并初始化角色坐骑数据（如果不存在则创建）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0（已存在或新创建），失败返回-1
 */
int CharacRidableStat_EnsureExists(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 打印坐骑属性记录信息（用于调试）
 * @param record 坐骑属性记录指针
 */
void CharacRidableStat_PrintInfo(const CharacRidableStat* record);

#endif /* CHARAC_RIDABLE_STAT_ORM_H */
