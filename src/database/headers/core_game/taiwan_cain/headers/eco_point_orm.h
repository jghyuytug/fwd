#ifndef ECO_POINT_ORM_H
#define ECO_POINT_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

typedef struct {
    unsigned int charac_no;      /* 角色编号 (主键) */
    unsigned int eco_point;      /* 总生态点数 */
    signed char point_500;       /* 500点档次获取次数 */
    signed char point_300;       /* 300点档次获取次数 */
    signed char point_100;       /* 100点档次获取次数 */
    signed char point_50;        /* 50点档次获取次数 */
    signed char point_20;        /* 20点档次获取次数 */
} EcoPoint;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色生态点数记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_Add(DBConnectionManager* manager, const EcoPoint* record);

/**
 * 根据角色编号获取生态点数记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int EcoPoint_Get(DBConnectionManager* manager, unsigned int charac_no, EcoPoint* record);

/**
 * 更新角色生态点数记录
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_Update(DBConnectionManager* manager, const EcoPoint* record);

/**
 * 删除角色生态点数记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色生态点数记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int EcoPoint_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 增加角色生态点数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param points 要增加的点数（正数）
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_AddPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int points);

/**
 * 减少角色生态点数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param points 要减少的点数（正数）
 * @return 成功返回0，失败返回-1（点数不足也返回-1）
 */
int EcoPoint_SubtractPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int points);

/**
 * 增加指定档次的获取次数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param tier_value 档次值（500/300/100/50/20）
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_IncrementTierCount(DBConnectionManager* manager, unsigned int charac_no, int tier_value);

/**
 * 批量更新所有档次计数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param count_500 500点档次次数
 * @param count_300 300点档次次数
 * @param count_100 100点档次次数
 * @param count_50 50点档次次数
 * @param count_20 20点档次次数
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_UpdateTierCounts(DBConnectionManager* manager, unsigned int charac_no,
                               signed char count_500, signed char count_300,
                               signed char count_100, signed char count_50,
                               signed char count_20);

/**
 * 重置角色生态点数（清零）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int EcoPoint_Reset(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 获取总生态点数排行榜
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储记录数组（调用者需要提前分配足够空间）
 * @param max_count 最大记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int EcoPoint_GetTopRanking(DBConnectionManager* manager, EcoPoint* records, int max_count);

/**
 * 统计总生态点数（所有角色）
 * @param manager 数据库连接管理器
 * @return 成功返回总点数，失败返回-1
 */
int EcoPoint_GetTotalPoints(DBConnectionManager* manager);

/**
 * 统计参与角色数量
 * @param manager 数据库连接管理器
 * @return 成功返回角色数量，失败返回-1
 */
int EcoPoint_CountCharacters(DBConnectionManager* manager);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印生态点数记录信息（用于调试）
 * @param record 要打印的记录
 */
void EcoPoint_PrintInfo(const EcoPoint* record);

#endif /* ECO_POINT_ORM_H */
