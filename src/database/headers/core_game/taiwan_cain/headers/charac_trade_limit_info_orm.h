#ifndef CHARAC_TRADE_LIMIT_INFO_ORM_H
#define CHARAC_TRADE_LIMIT_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * 角色交易限制信息 ORM
 *
 * 表: charac_trade_limit_info
 * 主键: charac_no
 * 功能: 角色交易限制跟踪、交易金额统计、交易次数管理
 */

/* 数据结构 */
typedef struct {
    unsigned int m_id;              /* 会员ID */
    unsigned int charac_no;         /* 角色编号 (主键) */
    char last_trade_time[20];       /* 最后交易时间 */
    unsigned int total_trade_gold;  /* 总交易金额 */
    unsigned short trade_count;     /* 交易次数 */
    unsigned char nexon_user;       /* Nexon用户标识 */
} CharacTradeLimitInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色交易限制信息
 */
int CharacTradeLimitInfo_Add(DBConnectionManager* manager, const CharacTradeLimitInfo* record);

/**
 * 获取角色交易限制信息
 */
int CharacTradeLimitInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacTradeLimitInfo* record);

/**
 * 更新角色交易限制信息
 */
int CharacTradeLimitInfo_Update(DBConnectionManager* manager, const CharacTradeLimitInfo* record);

/**
 * 删除角色交易限制信息
 */
int CharacTradeLimitInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色交易限制信息是否存在
 */
int CharacTradeLimitInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新最后交易时间
 */
int CharacTradeLimitInfo_UpdateLastTradeTime(DBConnectionManager* manager, unsigned int charac_no, const char* trade_time);

/**
 * 增加交易金额
 */
int CharacTradeLimitInfo_IncrementTradeGold(DBConnectionManager* manager, unsigned int charac_no, unsigned int gold);

/**
 * 增加交易次数
 */
int CharacTradeLimitInfo_IncrementTradeCount(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 重置交易统计
 */
int CharacTradeLimitInfo_ResetTradeStats(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 更新Nexon用户标识
 */
int CharacTradeLimitInfo_UpdateNexonUser(DBConnectionManager* manager, unsigned int charac_no, unsigned char nexon_user);

/* ==================== 查询操作 ==================== */

/**
 * 获取指定会员的所有角色交易信息
 */
int CharacTradeLimitInfo_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                       CharacTradeLimitInfo* records, int max_result);

/**
 * 获取交易金额大于指定值的角色
 */
int CharacTradeLimitInfo_GetByMinGold(DBConnectionManager* manager, unsigned int min_gold,
                                      CharacTradeLimitInfo* records, int max_result);

/**
 * 获取交易次数大于指定值的角色
 */
int CharacTradeLimitInfo_GetByMinTradeCount(DBConnectionManager* manager, unsigned short min_count,
                                             CharacTradeLimitInfo* records, int max_result);

/**
 * 获取指定时间之后交易的角色
 */
int CharacTradeLimitInfo_GetByTradeTimeAfter(DBConnectionManager* manager, const char* after_time,
                                              CharacTradeLimitInfo* records, int max_result);

/**
 * 获取Nexon用户
 */
int CharacTradeLimitInfo_GetNexonUsers(DBConnectionManager* manager, CharacTradeLimitInfo* records, int max_result);

/**
 * 批量重置交易统计
 */
int CharacTradeLimitInfo_BatchResetTradeStats(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色交易限制信息
 */
void CharacTradeLimitInfo_PrintInfo(const CharacTradeLimitInfo* record);

#endif /* CHARAC_TRADE_LIMIT_INFO_ORM_H */
