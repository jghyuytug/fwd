#ifndef LIMITED_SHOP_MANAGER_ORM_H
#define LIMITED_SHOP_MANAGER_ORM_H

#include "db_connection_manager.h"

/**
 * limited_shop_manager - 限购商店管理系统
 *
 * 表结构：
 * - 复合主键: (no, server_id, ipg_no)
 * - no (int unsigned, auto_increment): 记录编号（自增主键）
 * - server_id (tinyint unsigned): 服务器ID
 * - ipg_no (int unsigned): IPG编号（物品组编号）
 * - 物品信息: item_no, item_cnt
 * - 价格: cera_price (点券), gold_price (金币)
 * - 数量: total_cnt (总库存), sell_cnt (已售数量)
 * - 时间: start_time, end_time, real_end_time
 * - 购买条件: 职业、等级、账户/角色创建时间限制
 * - 其他: status_flag, title, restrict_no, npc_idx等
 *
 * 业务说明：
 * - 限时限购商店商品配置管理
 * - 支持职业、等级、账户时间等多维度限购条件
 * - 支持点券和金币双货币定价
 * - 库存管理和销量统计
 */

/* 状态标志常量 */
#define SHOP_STATUS_INACTIVE  0  /* 未激活 */
#define SHOP_STATUS_ACTIVE    1  /* 激活中 */
#define SHOP_STATUS_PAUSED    2  /* 暂停 */
#define SHOP_STATUS_ENDED     3  /* 已结束 */

/* 数据结构定义 */
typedef struct {
    unsigned int no;                      /* 记录编号（自增主键） */
    unsigned int occ_time;                /* 占用时间（记录创建时间） */
    unsigned char server_id;              /* 服务器ID */
    unsigned int ipg_no;                  /* IPG编号（物品组编号） */
    unsigned int item_no;                 /* 物品编号 */
    unsigned int item_cnt;                /* 物品数量 */
    unsigned int cera_price;              /* 点券价格 */
    unsigned int gold_price;              /* 金币价格 */
    char avatar_period_type;              /* 化身时效类型 */
    int total_cnt;                        /* 总库存数量 */
    unsigned int sell_cnt;                /* 已售数量 */
    unsigned int restrict_no;             /* 限购编号 */
    unsigned int start_time;              /* 开始时间（时间戳） */
    unsigned int end_time;                /* 结束时间（时间戳） */
    unsigned int real_end_time;           /* 实际结束时间（时间戳） */
    unsigned int npc_idx;                 /* NPC索引 */
    unsigned char cond_charac_job;        /* 职业限制 */
    unsigned char cond_lev_begin;         /* 最低等级限制 */
    unsigned char cond_lev_end;           /* 最高等级限制 */
    unsigned int cond_acc_create_time_begin;  /* 账户创建时间最早 */
    unsigned int cond_acc_create_time_end;    /* 账户创建时间最晚 */
    unsigned int cond_cha_create_time_begin;  /* 角色创建时间最早 */
    unsigned int cond_cha_create_time_end;    /* 角色创建时间最晚 */
    unsigned char status_flag;            /* 状态标志 */
    char title[51];                       /* 商品标题 */
    unsigned char range_section;          /* 范围分区 */
    char reason_etc[201];                 /* 其他原因 */
    char reason_stop[201];                /* 停止原因 */
    char pos_flag;                        /* 位置标志 */
} LimitedShopManager;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新限购商店记录
 * @param manager 数据库连接管理器
 * @param shop 商店记录
 * @return 新插入的no值（成功）, <0（失败）
 */
int LimitedShopManager_Add(DBConnectionManager* manager, const LimitedShopManager* shop);

/**
 * 根据复合主键查询限购商店记录
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param shop 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_Get(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, LimitedShopManager* shop);

/**
 * 更新限购商店记录
 * @param manager 数据库连接管理器
 * @param shop 商店记录
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_Update(DBConnectionManager* manager, const LimitedShopManager* shop);

/**
 * 删除限购商店记录
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_Delete(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no);

/**
 * 检查限购商店记录是否存在
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @return 1=存在, 0=不存在, <0=错误
 */
int LimitedShopManager_Exists(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加销售数量
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param count 增加数量
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_IncrementSellCount(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned int count);

/**
 * 更新商店状态
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param status_flag 状态标志
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_UpdateStatus(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned char status_flag);

/**
 * 设置停止原因
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param reason_stop 停止原因
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_SetStopReason(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, const char* reason_stop);

/**
 * 检查库存是否充足
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param required_count 需要数量
 * @return 1=库存充足, 0=库存不足, <0=错误
 */
int LimitedShopManager_CheckStock(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned int required_count);

/**
 * 获取剩余库存
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param server_id 服务器ID
 * @param ipg_no IPG编号
 * @param remaining 输出剩余库存（调用者提供）
 * @return 0=成功, <0=失败
 */
int LimitedShopManager_GetRemainingStock(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, int* remaining);

/* ==================== 查询操作 ==================== */

/**
 * 按服务器ID查询商店数量
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @return 商店数量, <0=错误
 */
int LimitedShopManager_CountByServer(DBConnectionManager* manager, unsigned char server_id);

/**
 * 按状态查询商店数量
 * @param manager 数据库连接管理器
 * @param status_flag 状态标志
 * @return 商店数量, <0=错误
 */
int LimitedShopManager_CountByStatus(DBConnectionManager* manager, unsigned char status_flag);

/**
 * 按服务器ID和状态查询商店数量
 * @param manager 数据库连接管理器
 * @param server_id 服务器ID
 * @param status_flag 状态标志
 * @return 商店数量, <0=错误
 */
int LimitedShopManager_CountByServerAndStatus(DBConnectionManager* manager, unsigned char server_id, unsigned char status_flag);

/**
 * 查询已过期的商店数量（end_time < 当前时间）
 * @param manager 数据库连接管理器
 * @return 商店数量, <0=错误
 */
int LimitedShopManager_CountExpired(DBConnectionManager* manager);

/**
 * 统计总记录数
 * @param manager 数据库连接管理器
 * @return 记录数, <0=错误
 */
int LimitedShopManager_CountTotal(DBConnectionManager* manager);

/* ==================== 批量操作 ==================== */

/**
 * 批量删除过期商店（end_time < NOW()）
 * @param manager 数据库连接管理器
 * @return 删除数量, <0=错误
 */
int LimitedShopManager_DeleteExpired(DBConnectionManager* manager);

/**
 * 批量更新过期商店状态为已结束
 * @param manager 数据库连接管理器
 * @return 更新数量, <0=错误
 */
int LimitedShopManager_UpdateExpiredStatus(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印商店记录信息（调试用）
 * @param shop 商店记录
 */
void LimitedShopManager_PrintInfo(const LimitedShopManager* shop);

/**
 * 释放商店记录内存（如果有动态分配）
 * @param shop 商店记录
 */
void LimitedShopManager_Free(LimitedShopManager* shop);

#endif /* LIMITED_SHOP_MANAGER_ORM_H */
