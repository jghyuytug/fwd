/**
 * DNF游戏服务器 - MemberPunishHack表ORM实现
 * 黑客处罚记录系统 - 用于管理玩家黑客行为的处罚记录
 */

#ifndef MEMBER_PUNISH_HACK_ORM_H
#define MEMBER_PUNISH_HACK_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>
#include <time.h>

/* ========================================================================
 * 处罚标志枚举
 * ======================================================================== */
typedef enum {
    HACK_FLAG_INACTIVE = 0,             /* 未生效/已过期 */
    HACK_FLAG_ACTIVE = 1                /* 生效中 */
} HackNowFlag;

typedef enum {
    HACK_AUTO_MANUAL = 0,               /* 手动处罚 */
    HACK_AUTO_SYSTEM = 1                /* 系统自动处罚 */
} HackAutoFlag;

/* ========================================================================
 * 黑客处罚记录结构体
 * ======================================================================== */
typedef struct {
    unsigned int m_id;                  /* 会员ID（主键） */
    unsigned int occ_time;              /* 发生时间（Unix时间戳） */
    unsigned int period;                /* 封禁周期（秒） */
    int now_flag;                       /* 当前状态标志 */
    int auto_flag;                      /* 自动处罚标志 */
    char reason[256];                   /* 处罚原因 */
    char hack_ip[16];                   /* 黑客IP地址 */
} MemberPunishHack;

/* ========================================================================
 * 核心CRUD操作
 * ======================================================================== */

/**
 * 添加黑客处罚记录
 * 使用 REPLACE INTO 实现幂等性（因为主键是m_id）
 * @return 0=成功, -1=失败
 */
int MemberPunishHack_Add(DBConnectionManager* manager, MemberPunishHack* hack);

/**
 * 获取指定会员的黑客处罚记录
 * @return 0=成功找到, -1=未找到或失败
 */
int MemberPunishHack_Get(DBConnectionManager* manager, unsigned int m_id,
                         MemberPunishHack* hack);

/**
 * 更新黑客处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishHack_Update(DBConnectionManager* manager, MemberPunishHack* hack);

/**
 * 删除黑客处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishHack_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查记录是否存在
 * @return true=存在, false=不存在
 */
bool MemberPunishHack_Exists(DBConnectionManager* manager, unsigned int m_id);

/**
 * 获取所有黑客处罚记录（分页）
 * @param offset 偏移量
 * @param limit 限制数量
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetAll(DBConnectionManager* manager,
                             MemberPunishHack* hacks, int offset, int limit);

/* ========================================================================
 * 业务逻辑查询函数
 * ======================================================================== */

/**
 * 检查会员是否有生效的黑客处罚
 * @return true=有生效处罚, false=无或已过期
 */
bool MemberPunishHack_HasActiveHackPunish(DBConnectionManager* manager, unsigned int m_id);

/**
 * 按IP地址查询处罚记录
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetByIP(DBConnectionManager* manager, const char* hack_ip,
                              MemberPunishHack* hacks, int max_count);

/**
 * 获取自动处罚的记录
 * @param offset 偏移量
 * @param limit 限制数量
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetAutoFlag(DBConnectionManager* manager,
                                  MemberPunishHack* hacks, int offset, int limit);

/**
 * 获取手动处罚的记录
 * @param offset 偏移量
 * @param limit 限制数量
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetManualFlag(DBConnectionManager* manager,
                                    MemberPunishHack* hacks, int offset, int limit);

/**
 * 更新当前状态标志
 * @return 0=成功, -1=失败
 */
int MemberPunishHack_UpdateNowFlag(DBConnectionManager* manager, unsigned int m_id, int now_flag);

/**
 * 获取处罚记录总数
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetCount(DBConnectionManager* manager);

/**
 * 检查处罚是否已过期（occ_time + period < 当前时间）
 * @return true=已过期, false=未过期或失败
 */
bool MemberPunishHack_IsExpired(DBConnectionManager* manager, unsigned int m_id);

/**
 * 获取已过期的记录列表
 * @return 记录数, -1=失败
 */
int MemberPunishHack_GetExpiredRecords(DBConnectionManager* manager,
                                        MemberPunishHack* hacks, int max_count);

/* ========================================================================
 * 批量操作
 * ======================================================================== */

/**
 * 批量添加处罚记录
 * @return 成功添加的记录数
 */
int MemberPunishHack_AddBatch(DBConnectionManager* manager, MemberPunishHack* hacks, int count);

/**
 * 批量更新过期记录的now_flag为INACTIVE
 * @return 更新的记录数, -1=失败
 */
int MemberPunishHack_UpdateExpiredFlags(DBConnectionManager* manager);

/* ========================================================================
 * 工具函数
 * ======================================================================== */

/**
 * 初始化黑客处罚记录结构
 */
void MemberPunishHack_Init(MemberPunishHack* hack);

/**
 * 打印黑客处罚记录信息
 */
void MemberPunishHack_Print(const MemberPunishHack* hack);

/**
 * 获取标志名称
 */
const char* MemberPunishHack_GetNowFlagName(int now_flag);
const char* MemberPunishHack_GetAutoFlagName(int auto_flag);

/**
 * 计算过期时间（Unix时间戳）
 * @param period_seconds 周期秒数
 * @return 过期时间戳
 */
unsigned int MemberPunishHack_CalculateExpireTime(unsigned int period_seconds);

#endif /* MEMBER_PUNISH_HACK_ORM_H */
