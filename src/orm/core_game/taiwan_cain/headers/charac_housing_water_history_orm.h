#ifndef CHARAC_HOUSING_WATER_HISTORY_ORM_H
#define CHARAC_HOUSING_WATER_HISTORY_ORM_H

#include "db_connection_manager.h"

/**
 * 角色住房浇水历史表 ORM
 *
 * 表: charac_housing_water_history
 * 主键: (charac_no, give_time) 复合主键
 * 功能: 记录角色住房浇水历史，包括接收浇水的角色、浇水时间、浇水者姓名
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;          /* 接收浇水的角色编号 (复合主键1) */
    char give_time[20];              /* 浇水时间 YYYY-MM-DD HH:MM:SS (复合主键2) */
    char give_charac_name[21];       /* 浇水者角色名 */
} CharacHousingWaterHistory;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加浇水历史记录
 */
int CharacHousingWaterHistory_Add(DBConnectionManager* manager, const CharacHousingWaterHistory* record);

/**
 * 获取指定浇水历史记录
 */
int CharacHousingWaterHistory_Get(DBConnectionManager* manager, unsigned int charac_no,
                                   const char* give_time, CharacHousingWaterHistory* record);

/**
 * 更新浇水历史记录
 */
int CharacHousingWaterHistory_Update(DBConnectionManager* manager, const CharacHousingWaterHistory* record);

/**
 * 删除指定浇水历史记录
 */
int CharacHousingWaterHistory_Delete(DBConnectionManager* manager, unsigned int charac_no, const char* give_time);

/**
 * 检查浇水历史记录是否存在
 */
int CharacHousingWaterHistory_Exists(DBConnectionManager* manager, unsigned int charac_no, const char* give_time);

/* ==================== 查询操作 ==================== */

/**
 * 获取角色的所有浇水历史记录数量
 */
int CharacHousingWaterHistory_CountByCharac(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 获取角色在指定时间段内的浇水记录
 * @param start_time 开始时间 YYYY-MM-DD HH:MM:SS
 * @param end_time 结束时间 YYYY-MM-DD HH:MM:SS
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 找到的记录数量
 */
int CharacHousingWaterHistory_GetByTimeRange(DBConnectionManager* manager, unsigned int charac_no,
                                               const char* start_time, const char* end_time,
                                               CharacHousingWaterHistory* records, int max_count);

/**
 * 获取指定浇水者的所有浇水记录
 * @param give_charac_name 浇水者角色名
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 找到的记录数量
 */
int CharacHousingWaterHistory_GetByGiver(DBConnectionManager* manager, const char* give_charac_name,
                                          CharacHousingWaterHistory* records, int max_count);

/**
 * 删除角色的所有浇水历史记录
 */
int CharacHousingWaterHistory_DeleteAllByCharac(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 删除指定时间之前的旧记录
 * @param before_time 时间界限 YYYY-MM-DD HH:MM:SS
 * @return 删除的记录数量
 */
int CharacHousingWaterHistory_DeleteBefore(DBConnectionManager* manager, const char* before_time);

/**
 * 获取最近N天的浇水记录
 * @param charac_no 角色编号
 * @param days 天数
 * @param records 输出数组
 * @param max_count 数组最大容量
 * @return 找到的记录数量
 */
int CharacHousingWaterHistory_GetRecentDays(DBConnectionManager* manager, unsigned int charac_no,
                                              int days, CharacHousingWaterHistory* records, int max_count);

/* ==================== 工具函数 ==================== */

/**
 * 打印浇水历史记录信息
 */
void CharacHousingWaterHistory_PrintInfo(const CharacHousingWaterHistory* record);

#endif /* CHARAC_HOUSING_WATER_HISTORY_ORM_H */
