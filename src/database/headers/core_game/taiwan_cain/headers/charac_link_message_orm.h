#ifndef CHARAC_LINK_MESSAGE_ORM_H
#define CHARAC_LINK_MESSAGE_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Link Message ORM - 角色链接消息系统
 *
 * 表: charac_link_message (taiwan_cain 数据库)
 * 描述: 角色链接消息标识表
 * 主键: m_id (消息ID)
 *
 * 字段说明:
 * - m_id: 消息ID (UNSIGNED INT, 主键)
 * - message_flag: 消息标志 (TINYINT)
 *
 * 业务用途:
 * - 存储角色链接消息的标识信息
 * - 支持消息状态跟踪
 * - 用于消息查询和管理
 *
 * API设计: 9个函数
 * - CRUD操作: 5个
 * - 查询操作: 2个
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #50/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 角色链接消息记录结构 */
typedef struct {
    unsigned int m_id;       /* 消息ID (主键) */
    char message_flag;       /* 消息标志 */
} CharacLinkMessage;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新角色链接消息（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacLinkMessage_Add(DBConnectionManager* manager, const CharacLinkMessage* record);

/**
 * 获取指定消息ID的链接消息
 * @param manager 数据库连接管理器
 * @param m_id 消息ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacLinkMessage_Get(DBConnectionManager* manager, unsigned int m_id, CharacLinkMessage* record);

/**
 * 更新角色链接消息
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int CharacLinkMessage_Update(DBConnectionManager* manager, const CharacLinkMessage* record);

/**
 * 删除角色链接消息
 * @param manager 数据库连接管理器
 * @param m_id 消息ID
 * @return 成功返回0，失败返回-1
 */
int CharacLinkMessage_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查角色链接消息是否存在
 * @param manager 数据库连接管理器
 * @param m_id 消息ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacLinkMessage_Exists(DBConnectionManager* manager, unsigned int m_id);

/*******************************************************************************
 * 查询操作 (2个函数)
 ******************************************************************************/

/**
 * 获取链接消息总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int CharacLinkMessage_Count(DBConnectionManager* manager);

/**
 * 获取所有链接消息
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacLinkMessage_GetAll(DBConnectionManager* manager, CharacLinkMessage* records, int max_records);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除链接消息
 * @param manager 数据库连接管理器
 * @param m_ids 消息ID数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int CharacLinkMessage_BatchDelete(DBConnectionManager* manager, const unsigned int* m_ids, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印链接消息信息（用于调试）
 * @param record 链接消息记录指针
 */
void CharacLinkMessage_PrintInfo(const CharacLinkMessage* record);

#endif /* CHARAC_LINK_MESSAGE_ORM_H */
