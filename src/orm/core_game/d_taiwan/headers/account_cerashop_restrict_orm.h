#ifndef ACCOUNT_CERASHOP_RESTRICT_ORM_H
#define ACCOUNT_CERASHOP_RESTRICT_ORM_H

#include "db_connection_manager.h"

/**
 * @file account_cerashop_restrict_orm.h
 * @brief 账户Cera商店购买限制表 ORM 接口
 *
 * 数据库: d_taiwan
 * 表名: account_cerashop_restrict
 * 用途: 限制账户在特定IPG商品的购买次数和时间
 */

/* 数据结构定义 */
typedef struct {
    unsigned int m_id;              /* 账户ID (复合主键1) */
    unsigned int ipg_no;            /* IPG商品编号 (复合主键2) */
    unsigned int count;             /* 已购买次数 */
    unsigned int next_date;         /* 下次可购买日期 (Unix时间戳) */
    unsigned int end_date;          /* 限制结束日期 (Unix时间戳) */
    unsigned int last_access_date;  /* 最后访问日期 (Unix时间戳) */
} AccountCerashopRestrict;

/* CRUD 操作 */

/**
 * @brief 添加或更新账户Cera商店限制记录
 * @param manager 数据库连接管理器
 * @param record 限制记录指针
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_Add(DBConnectionManager* manager, const AccountCerashopRestrict* record);

/**
 * @brief 获取账户的特定商品限制信息
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @param record 输出参数，存储查询结果
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_Get(DBConnectionManager* manager, unsigned int m_id,
                                unsigned int ipg_no, AccountCerashopRestrict* record);

/**
 * @brief 更新账户Cera商店限制记录
 * @param manager 数据库连接管理器
 * @param record 限制记录指针
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_Update(DBConnectionManager* manager, const AccountCerashopRestrict* record);

/**
 * @brief 删除账户的特定商品限制
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no);

/**
 * @brief 检查账户的特定商品限制是否存在
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @return 1=存在, 0=不存在, <0=错误
 */
int AccountCerashopRestrict_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no);

/* 业务操作 */

/**
 * @brief 增加购买次数
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @param increment 增加数量 (默认1)
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_IncrementCount(DBConnectionManager* manager, unsigned int m_id,
                                           unsigned int ipg_no, unsigned int increment);

/**
 * @brief 更新最后访问时间
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @param timestamp 时间戳 (0表示使用当前时间)
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_UpdateLastAccess(DBConnectionManager* manager, unsigned int m_id,
                                             unsigned int ipg_no, unsigned int timestamp);

/**
 * @brief 检查是否允许购买 (检查日期和次数限制)
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @param current_time 当前时间戳
 * @return 1=允许, 0=禁止, <0=错误
 */
int AccountCerashopRestrict_CanPurchase(DBConnectionManager* manager, unsigned int m_id,
                                        unsigned int ipg_no, unsigned int current_time);

/**
 * @brief 重置购买次数 (例如新周期开始)
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @param ipg_no IPG商品编号
 * @return 0=成功, <0=失败
 */
int AccountCerashopRestrict_ResetCount(DBConnectionManager* manager, unsigned int m_id, unsigned int ipg_no);

/**
 * @brief 获取账户的所有商品限制数量
 * @param manager 数据库连接管理器
 * @param m_id 账户ID
 * @return 限制数量, <0=错误
 */
int AccountCerashopRestrict_GetCountByAccount(DBConnectionManager* manager, unsigned int m_id);

/* 工具函数 */

/**
 * @brief 打印限制记录信息（调试用）
 * @param record 限制记录指针
 */
void AccountCerashopRestrict_PrintInfo(const AccountCerashopRestrict* record);

#endif /* ACCOUNT_CERASHOP_RESTRICT_ORM_H */
