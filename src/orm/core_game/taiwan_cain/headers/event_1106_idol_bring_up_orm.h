#ifndef EVENT_1106_IDOL_BRING_UP_ORM_H
#define EVENT_1106_IDOL_BRING_UP_ORM_H

#include "db_connection_manager.h"

/**
 * 偶像养成活动 ORM (1106期)
 *
 * 功能：
 * - 记录角色参与偶像养成活动数据
 * - 追踪浇水次数和花盆类型
 * - 管理活动称号奖励发放状态
 * - 支持活动进度查询和统计
 *
 * 主键：
 * - PRIMARY KEY (charac_no) - 角色编号唯一主键
 *
 * 字段说明：
 * - m_id: 偶像/角色ID
 * - pot_type: 花盆类型（0-N不同等级花盆）
 * - water_cnt: 浇水次数统计
 * - give_title_flag: 第一阶段称号发放标记（0=未发放，1=已发放）
 * - occ_date: 活动参与时间
 * - give_title_flag2: 第二阶段称号发放标记（0=未发放，1=已发放）
 */

/* 偶像养成活动记录结构 */
typedef struct {
    unsigned int charac_no;              /* 角色编号（主键） */
    unsigned int m_id;                   /* 偶像/角色ID */
    signed char pot_type;                /* 花盆类型 */
    signed char water_cnt;               /* 浇水次数 */
    signed char give_title_flag;         /* 第一阶段称号发放标记 */
    char occ_date[20];                   /* 活动参与时间（YYYY-MM-DD HH:MM:SS） */
    signed char give_title_flag2;        /* 第二阶段称号发放标记 */
} Event1106IdolBringUp;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加偶像养成活动记录（使用 REPLACE）
 *
 * @param manager 数据库连接管理器
 * @param record 活动记录
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_Add(DBConnectionManager* manager, const Event1106IdolBringUp* record);

/**
 * 获取偶像养成活动记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int Event1106IdolBringUp_Get(DBConnectionManager* manager, unsigned int charac_no, Event1106IdolBringUp* record);

/**
 * 更新偶像养成活动记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param record 活动记录（必须包含有效的charac_no）
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_Update(DBConnectionManager* manager, const Event1106IdolBringUp* record);

/**
 * 删除偶像养成活动记录（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查偶像养成活动记录是否存在（按角色编号）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 1 存在, 0 不存在, <0 错误
 */
int Event1106IdolBringUp_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加浇水次数
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param increment 增加的浇水次数（可以为负数表示减少）
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_IncrementWaterCount(DBConnectionManager* manager, unsigned int charac_no, int increment);

/**
 * 更新花盆类型
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param pot_type 新的花盆类型
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_UpdatePotType(DBConnectionManager* manager, unsigned int charac_no, signed char pot_type);

/**
 * 标记第一阶段称号已发放
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_MarkTitleGiven(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 标记第二阶段称号已发放
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_MarkTitle2Given(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 重置活动进度（保留记录但重置数据）
 *
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0 成功, <0 失败
 */
int Event1106IdolBringUp_ResetProgress(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 批量删除过期活动数据
 *
 * @param manager 数据库连接管理器
 * @param days 删除N天前的记录
 * @return >=0 删除的记录数, <0 失败
 */
int Event1106IdolBringUp_DeleteOlderThan(DBConnectionManager* manager, int days);

/* ==================== 查询操作 ==================== */

/**
 * 统计参与活动的总角色数
 *
 * @param manager 数据库连接管理器
 * @return >=0 总角色数, <0 失败
 */
int Event1106IdolBringUp_CountAll(DBConnectionManager* manager);

/**
 * 统计已获得第一阶段称号的角色数
 *
 * @param manager 数据库连接管理器
 * @return >=0 角色数, <0 失败
 */
int Event1106IdolBringUp_CountTitleReceivers(DBConnectionManager* manager);

/**
 * 统计已获得第二阶段称号的角色数
 *
 * @param manager 数据库连接管理器
 * @return >=0 角色数, <0 失败
 */
int Event1106IdolBringUp_CountTitle2Receivers(DBConnectionManager* manager);

/**
 * 统计特定花盆类型的角色数
 *
 * @param manager 数据库连接管理器
 * @param pot_type 花盆类型
 * @return >=0 角色数, <0 失败
 */
int Event1106IdolBringUp_CountByPotType(DBConnectionManager* manager, signed char pot_type);

/**
 * 查询浇水次数最多的前N名角色
 *
 * @param manager 数据库连接管理器
 * @param top_n 查询前N名（最大100）
 * @param records 输出参数，接收记录数组
 * @return >=0 实际返回的记录数, <0 失败
 */
int Event1106IdolBringUp_GetTopWaterers(DBConnectionManager* manager, int top_n, Event1106IdolBringUp* records);

/**
 * 查询平均浇水次数
 *
 * @param manager 数据库连接管理器
 * @return >=0 平均浇水次数, <0 失败
 */
int Event1106IdolBringUp_GetAvgWaterCount(DBConnectionManager* manager);

/**
 * 统计未获得任何称号的角色数
 *
 * @param manager 数据库连接管理器
 * @return >=0 角色数, <0 失败
 */
int Event1106IdolBringUp_CountNoTitleReceived(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印偶像养成活动记录（调试用）
 *
 * @param record 活动记录
 */
void Event1106IdolBringUp_PrintInfo(const Event1106IdolBringUp* record);

#endif /* EVENT_1106_IDOL_BRING_UP_ORM_H */
