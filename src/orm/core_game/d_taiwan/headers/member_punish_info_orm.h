/**
 * DNF游戏服务器 - MemberPunishInfo表ORM实现
 * 处罚信息系统 - 用于管理玩家处罚记录
 */

#ifndef MEMBER_PUNISH_INFO_ORM_H
#define MEMBER_PUNISH_INFO_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>
#include <time.h>

/* ========================================================================
 * 处罚类型枚举
 * ======================================================================== */
typedef enum {
    PUNISH_TYPE_CHAT_BAN = 1,           /* 禁言 */
    PUNISH_TYPE_TRADE_BAN = 2,          /* 禁止交易 */
    PUNISH_TYPE_ACCOUNT_BAN = 3,        /* 封号 */
    PUNISH_TYPE_IP_BAN = 4,             /* IP封禁 */
    PUNISH_TYPE_MAC_BAN = 5,            /* MAC封禁 */
    PUNISH_TYPE_TEMP_BAN = 10,          /* 临时封禁 */
    PUNISH_TYPE_PERM_BAN = 11,          /* 永久封禁 */
    PUNISH_TYPE_WARNING = 20            /* 警告 */
} PunishType;

/* ========================================================================
 * 处罚状态枚举
 * ======================================================================== */
typedef enum {
    PUNISH_STATUS_PENDING = 0,          /* 待生效 */
    PUNISH_STATUS_ACTIVE = 1,           /* 生效中 */
    PUNISH_STATUS_EXPIRED = 2,          /* 已过期 */
    PUNISH_STATUS_LIFTED = 3            /* 已解除 */
} PunishStatus;

/* ========================================================================
 * 处罚信息结构体
 * ======================================================================== */
typedef struct {
    int m_id;                           /* 会员ID */
    int punish_type;                    /* 处罚类型 */
    char occ_time[20];                  /* 发生时间 YYYY-MM-DD HH:MM:SS */
    int punish_value;                   /* 处罚值（保留字段，具体用途待确认） */
    int apply_flag;                     /* 状态标记 */
    char start_time[20];                /* 开始时间 */
    char end_time[20];                  /* 结束时间 */
    char admin_id[256];                 /* 管理员ID */
    char reason[256];                   /* 处罚原因 */
    char ip[16];                        /* IP地址 */
    char mac[101];                      /* MAC地址 */
} MemberPunishInfo;

/* ========================================================================
 * 核心CRUD操作
 * ======================================================================== */

/**
 * 添加处罚记录
 * 使用 INSERT IGNORE 实现幂等性
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Add(DBConnectionManager* manager, MemberPunishInfo* punish);

/**
 * 获取指定会员的处罚记录
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetByMember(DBConnectionManager* manager, int m_id,
                                  MemberPunishInfo* punishes, int max_count);

/**
 * 获取指定会员的特定类型处罚记录
 * @return 0=成功找到, -1=未找到或失败
 */
int MemberPunishInfo_Get(DBConnectionManager* manager, int m_id, int punish_type,
                         MemberPunishInfo* punish);

/**
 * 更新处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Update(DBConnectionManager* manager, MemberPunishInfo* punish);

/**
 * 删除处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Delete(DBConnectionManager* manager, int m_id, int punish_type);

/**
 * 删除指定会员的所有处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_DeleteAll(DBConnectionManager* manager, int m_id);

/* ========================================================================
 * 业务逻辑查询函数
 * ======================================================================== */

/**
 * 检查会员是否有指定类型的生效处罚
 * @return true=有生效处罚, false=无或已过期
 */
bool MemberPunishInfo_HasActivePunish(DBConnectionManager* manager, int m_id, int punish_type);

/**
 * 检查会员是否有任何生效处罚
 * @return true=有生效处罚, false=无或已过期
 */
bool MemberPunishInfo_HasAnyActivePunish(DBConnectionManager* manager, int m_id);

/**
 * 获取会员的生效处罚列表
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetActive(DBConnectionManager* manager, int m_id,
                                MemberPunishInfo* punishes, int max_count);

/**
 * 更新处罚状态
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_UpdateStatus(DBConnectionManager* manager, int m_id, int punish_type, int status);

/**
 * 解除处罚（将状态设为LIFTED）
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Lift(DBConnectionManager* manager, int m_id, int punish_type);

/**
 * 获取会员处罚记录总数
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetCount(DBConnectionManager* manager, int m_id);

/**
 * 检查处罚是否已过期（end_time < 当前时间）
 * @return true=已过期, false=未过期或失败
 */
bool MemberPunishInfo_IsExpired(DBConnectionManager* manager, int m_id, int punish_type);

/**
 * 按管理员ID查询处罚记录
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                 MemberPunishInfo* punishes, int max_count);

/* ========================================================================
 * 批量操作
 * ======================================================================== */

/**
 * 批量添加处罚记录
 * @return 成功添加的记录数
 */
int MemberPunishInfo_AddBatch(DBConnectionManager* manager, MemberPunishInfo* punishes, int count);

/**
 * 批量更新过期处罚状态
 * 将所有 end_time < NOW() 且状态为ACTIVE的记录更新为EXPIRED
 * @return 更新的记录数, -1=失败
 */
int MemberPunishInfo_UpdateExpired(DBConnectionManager* manager);

/* ========================================================================
 * 工具函数
 * ======================================================================== */

/**
 * 初始化处罚信息结构
 */
void MemberPunishInfo_Init(MemberPunishInfo* punish);

/**
 * 打印处罚信息
 */
void MemberPunishInfo_Print(const MemberPunishInfo* punish);

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
void MemberPunishInfo_GetTimeString(char* buffer, size_t size);

/**
 * 计算过期时间 (当前时间 + duration_seconds)
 */
void MemberPunishInfo_GetExpireTime(char* buffer, size_t size, int duration_seconds);

/**
 * 解析处罚类型名称
 */
const char* MemberPunishInfo_GetTypeName(int punish_type);

/**
 * 解析处罚状态名称
 */
const char* MemberPunishInfo_GetStatusName(int apply_flag);

#endif /* MEMBER_PUNISH_INFO_ORM_H */
