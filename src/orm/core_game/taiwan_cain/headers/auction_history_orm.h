#ifndef AUCTION_HISTORY_ORM_H
#define AUCTION_HISTORY_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Auction History ORM - 拍卖历史记录系统
 *
 * 表: auction_history (taiwan_cain 数据库)
 * 描述: 拍卖行历史交易记录表
 * 主键: auction_id (bigint unsigned)
 *
 * 字段说明:
 * - auction_id: 拍卖ID (BIGINT UNSIGNED, PRIMARY KEY)
 * - start_time: 开始时间 (DATETIME)
 * - occ_time: 成交时间 (DATETIME)
 * - event_type: 事件类型 (TINYINT: 上架/下架/成交/过期)
 * - owner_id: 卖家ID (INT)
 * - buyer_id: 买家ID (INT)
 * - price: 成交价格 (INT)
 * - seal_flag: 封印标志 (TINYINT)
 * - item_id: 物品ID (INT UNSIGNED)
 * - add_info: 附加信息 (INT)
 * - upgrade: 强化等级 (TINYINT UNSIGNED)
 * - amplify_option: 增幅选项 (TINYINT UNSIGNED)
 * - amplify_value: 增幅数值 (MEDIUMINT UNSIGNED)
 * - seal_cnt: 封印次数 (TINYINT UNSIGNED)
 * - endurance: 耐久度 (SMALLINT UNSIGNED)
 * - extend_info: 扩展信息 (INT UNSIGNED)
 * - owner_postal_id: 卖家邮件ID (INT UNSIGNED)
 * - buyer_postal_id: 买家邮件ID (INT UNSIGNED)
 * - unit_price: 单价 (INT UNSIGNED)
 *
 * 业务用途:
 * - 记录拍卖行所有交易历史
 * - 支持按卖家/买家/物品查询
 * - 统计成交价格和交易量
 * - 追踪物品流通记录
 *
 * API设计: 13个函数
 * - CRUD操作: 5个
 * - 查询操作: 6个
 * - 统计操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #64/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 拍卖历史记录结构 */
typedef struct {
    unsigned long long auction_id;    /* 拍卖ID (主键) */
    char start_time[20];              /* 开始时间 (YYYY-MM-DD HH:MM:SS) */
    char occ_time[20];                /* 成交时间 (YYYY-MM-DD HH:MM:SS) */
    char event_type;                  /* 事件类型 */
    int owner_id;                     /* 卖家ID */
    int buyer_id;                     /* 买家ID */
    int price;                        /* 成交价格 */
    char seal_flag;                   /* 封印标志 */
    unsigned int item_id;             /* 物品ID */
    int add_info;                     /* 附加信息 */
    unsigned char upgrade;            /* 强化等级 */
    unsigned char amplify_option;     /* 增幅选项 */
    unsigned int amplify_value;       /* 增幅数值 */
    unsigned char seal_cnt;           /* 封印次数 */
    unsigned short endurance;         /* 耐久度 */
    unsigned int extend_info;         /* 扩展信息 */
    unsigned int owner_postal_id;     /* 卖家邮件ID */
    unsigned int buyer_postal_id;     /* 买家邮件ID */
    unsigned int unit_price;          /* 单价 */
} AuctionHistory;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加拍卖历史记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int AuctionHistory_Add(DBConnectionManager* manager, const AuctionHistory* record);

/**
 * 获取指定拍卖历史记录
 * @param manager 数据库连接管理器
 * @param auction_id 拍卖ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int AuctionHistory_Get(DBConnectionManager* manager, unsigned long long auction_id, AuctionHistory* record);

/**
 * 更新拍卖历史记录
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int AuctionHistory_Update(DBConnectionManager* manager, const AuctionHistory* record);

/**
 * 删除拍卖历史记录
 * @param manager 数据库连接管理器
 * @param auction_id 拍卖ID
 * @return 成功返回0，失败返回-1
 */
int AuctionHistory_Delete(DBConnectionManager* manager, unsigned long long auction_id);

/**
 * 检查拍卖历史记录是否存在
 * @param manager 数据库连接管理器
 * @param auction_id 拍卖ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int AuctionHistory_Exists(DBConnectionManager* manager, unsigned long long auction_id);

/*******************************************************************************
 * 查询操作 (6个函数)
 ******************************************************************************/

/**
 * 获取历史记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int AuctionHistory_Count(DBConnectionManager* manager);

/**
 * 获取所有历史记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuctionHistory_GetAll(DBConnectionManager* manager, AuctionHistory* records, int max_records);

/**
 * 获取指定卖家的历史记录
 * @param manager 数据库连接管理器
 * @param owner_id 卖家ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuctionHistory_GetByOwner(DBConnectionManager* manager, int owner_id, AuctionHistory* records, int max_records);

/**
 * 获取指定买家的历史记录
 * @param manager 数据库连接管理器
 * @param buyer_id 买家ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuctionHistory_GetByBuyer(DBConnectionManager* manager, int buyer_id, AuctionHistory* records, int max_records);

/**
 * 获取指定物品的历史记录
 * @param manager 数据库连接管理器
 * @param item_id 物品ID
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuctionHistory_GetByItem(DBConnectionManager* manager, unsigned int item_id, AuctionHistory* records, int max_records);

/**
 * 获取指定事件类型的历史记录
 * @param manager 数据库连接管理器
 * @param event_type 事件类型
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int AuctionHistory_GetByEventType(DBConnectionManager* manager, char event_type, AuctionHistory* records, int max_records);

/*******************************************************************************
 * 统计操作 (1个函数)
 ******************************************************************************/

/**
 * 获取指定物品的平均成交价格
 * @param manager 数据库连接管理器
 * @param item_id 物品ID
 * @return >=0平均价格，<0失败或无成交记录
 */
int AuctionHistory_GetAveragePrice(DBConnectionManager* manager, unsigned int item_id);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印拍卖历史记录信息（用于调试）
 * @param record 拍卖历史记录指针
 */
void AuctionHistory_PrintInfo(const AuctionHistory* record);

#endif /* AUCTION_HISTORY_ORM_H */
