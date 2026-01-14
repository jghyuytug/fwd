#ifndef PVP_GRADE_EXPAND_ORM_H
#define PVP_GRADE_EXPAND_ORM_H

#include "db_connection_manager.h"

/**
 * PVP等级扩展系统 ORM
 *
 * 功能：
 * - 存储角色PVP等级和积分
 * - 追踪最后游戏时间
 * - 支持等级排名和积分排行
 * - 不活跃玩家检测
 *
 * 索引优化：
 * - PRIMARY KEY (charac_no)
 * - INDEX idx_pvp_grade (pvp_grade)
 * - INDEX idx_pvp_point (pvp_point)
 */

/* PVP等级扩展记录结构 */
typedef struct {
    int charac_no;              /* 角色编号（主键） */
    int pvp_grade;              /* PVP等级 */
    int pvp_point;              /* PVP积分 */
    char last_play_time[20];    /* 最后游戏时间（YYYY-MM-DD HH:MM:SS） */
} PvpGradeExpand;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加PVP等级扩展记录（使用 REPLACE）
 *
 * @param manager 数据库连接管理器
 * @param record PVP等级扩展记录
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_Add(DBConnectionManager* manager, const PvpGradeExpand* record);

/**
 * 获取PVP等级扩展记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int PvpGradeExpand_Get(DBConnectionManager* manager, int charac_no, PvpGradeExpand* record);

/**
 * 更新PVP等级扩展记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param record PVP等级扩展记录（必须包含有效的charac_no）
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_Update(DBConnectionManager* manager, const PvpGradeExpand* record);

/**
 * 删除PVP等级扩展记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查PVP等级扩展记录是否存在（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 1 存在, 0 不存在, <0 错误
 */
int PvpGradeExpand_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新PVP积分（支持增加或减少）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param point_delta 积分变化量（正数增加，负数减少）
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_UpdatePoints(DBConnectionManager* manager, int charac_no, int point_delta);

/**
 * 更新PVP等级
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param new_grade 新等级
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_UpdateGrade(DBConnectionManager* manager, int charac_no, int new_grade);

/**
 * 更新最后游戏时间（设置为当前时间）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no);

/**
 * 重置玩家PVP数据（等级归零、积分清空）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int PvpGradeExpand_Reset(DBConnectionManager* manager, int charac_no);

/* ==================== 查询操作 ==================== */

/**
 * 按PVP积分排名（前N名）
 *
 * @param manager 数据库连接管理器
 * @param limit 返回记录数量
 * @param records 输出参数，接收记录数组
 * @return >=0 实际返回数量, <0 失败
 */
int PvpGradeExpand_GetTopRanking(DBConnectionManager* manager, int limit, PvpGradeExpand* records);

/**
 * 统计指定等级的玩家数量
 *
 * @param manager 数据库连接管理器
 * @param pvp_grade PVP等级
 * @return >=0 玩家数量, <0 失败
 */
int PvpGradeExpand_CountByGrade(DBConnectionManager* manager, int pvp_grade);

/**
 * 查询不活跃玩家数量（超过指定天数未游戏）
 *
 * @param manager 数据库连接管理器
 * @param days 天数阈值
 * @return >=0 不活跃玩家数, <0 失败
 */
int PvpGradeExpand_CountInactivePlayers(DBConnectionManager* manager, int days);

/**
 * 按等级范围查询玩家数量
 *
 * @param manager 数据库连接管理器
 * @param min_grade 最低等级
 * @param max_grade 最高等级
 * @return >=0 玩家数量, <0 失败
 */
int PvpGradeExpand_CountByGradeRange(DBConnectionManager* manager, int min_grade, int max_grade);

/**
 * 按积分范围查询玩家数量
 *
 * @param manager 数据库连接管理器
 * @param min_point 最低积分
 * @param max_point 最高积分
 * @return >=0 玩家数量, <0 失败
 */
int PvpGradeExpand_CountByPointRange(DBConnectionManager* manager, int min_point, int max_point);

/**
 * 查询总玩家数
 *
 * @param manager 数据库连接管理器
 * @return >=0 总玩家数, <0 失败
 */
int PvpGradeExpand_CountAll(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印PVP等级扩展记录（调试用）
 *
 * @param record PVP等级扩展记录
 */
void PvpGradeExpand_PrintInfo(const PvpGradeExpand* record);

#endif /* PVP_GRADE_EXPAND_ORM_H */
