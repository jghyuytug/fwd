#ifndef CHARAC_ACHIEVEMENT_ORM_H
#define CHARAC_ACHIEVEMENT_ORM_H

#include "db_connection_manager.h"
#include <time.h>

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_ACHIEVEMENT_DATA_LEN 4096

typedef struct {
    unsigned int charac_no;                        /* 角色编号 (主键) */
    unsigned char achievement[MAX_ACHIEVEMENT_DATA_LEN];  /* 成就数据 (BLOB字段, 二进制数据) */
    int achievement_len;                           /* 成就数据实际长度 */
    char last_update_time[32];                     /* 最后更新时间 (DATETIME) */
} CharacAchievement;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色成就数据 (使用 REPLACE INTO 实现幂等性)
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_Add(DBConnectionManager* manager, const CharacAchievement* record);

/**
 * 根据角色编号获取成就数据
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacAchievement_Get(DBConnectionManager* manager, unsigned int charac_no, CharacAchievement* record);

/**
 * 更新角色的成就数据
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param new_achievement 新的成就数据
 * @param new_achievement_len 新成就数据的长度
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_Update(DBConnectionManager* manager, unsigned int charac_no,
                               const unsigned char* new_achievement, int new_achievement_len);

/**
 * 删除角色的成就记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色是否有成就记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacAchievement_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务查询操作
 ******************************************************************************/

/**
 * 按最后更新时间范围查询成就记录
 * @param manager 数据库连接管理器
 * @param start_time 开始时间 (格式: "YYYY-MM-DD HH:MM:SS")
 * @param end_time 结束时间 (格式: "YYYY-MM-DD HH:MM:SS")
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacAchievement_GetByLastUpdateTimeRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 CharacAchievement* records, int max_records);

/**
 * 获取最近更新的成就记录
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储查询结果数组
 * @param top_n 返回最近的N条记录
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacAchievement_GetRecentUpdates(DBConnectionManager* manager,
                                         CharacAchievement* records, int top_n);

/**
 * 获取成就数据为空的角色列表
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacAchievement_GetEmptyAchievements(DBConnectionManager* manager,
                                             CharacAchievement* records, int max_records);

/**
 * 获取有成就数据的角色列表
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacAchievement_GetNonEmptyAchievements(DBConnectionManager* manager,
                                                CharacAchievement* records, int max_records);

/**
 * 获取角色成就数据的大小
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回数据大小（字节），失败返回-1
 */
int CharacAchievement_GetAchievementSize(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 按成就数据大小范围查询角色
 * @param manager 数据库连接管理器
 * @param min_size 最小数据大小（字节）
 * @param max_size 最大数据大小（字节）
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacAchievement_GetByAchievementSize(DBConnectionManager* manager,
                                             int min_size, int max_size,
                                             CharacAchievement* records, int max_records);

/**
 * 清空角色的成就数据（设为空BLOB）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_ClearAchievement(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 统计有成就数据的角色数量
 * @param manager 数据库连接管理器
 * @return 成功返回角色数量，失败返回-1
 */
int CharacAchievement_CountNonEmptyAchievements(DBConnectionManager* manager);

/**
 * 更新角色的最后更新时间
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_UpdateTimestamp(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 批量操作
 ******************************************************************************/

/**
 * 批量添加/更新角色成就数据
 * @param manager 数据库连接管理器
 * @param records 要添加的记录数组
 * @param count 记录数量
 * @return 成功返回成功添加的记录数，失败返回-1
 */
int CharacAchievement_BulkAdd(DBConnectionManager* manager,
                               const CharacAchievement* records, int count);

/**
 * 批量删除指定角色编号的成就数据
 * @param manager 数据库连接管理器
 * @param charac_nos 角色编号数组
 * @param count 编号数量
 * @return 成功返回0，失败返回-1
 */
int CharacAchievement_BulkDeleteByCharacNos(DBConnectionManager* manager,
                                              const unsigned int* charac_nos, int count);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印角色成就记录信息（用于调试）
 * @param record 要打印的记录
 */
void CharacAchievement_PrintInfo(const CharacAchievement* record);

#endif /* CHARAC_ACHIEVEMENT_ORM_H */
