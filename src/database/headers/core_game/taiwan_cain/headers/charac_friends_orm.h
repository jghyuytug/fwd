#ifndef CHARAC_FRIENDS_ORM_H
#define CHARAC_FRIENDS_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

typedef struct {
    unsigned int charac_no;  /* 角色编号 (主键) */
    unsigned int friend_no;  /* 好友编号 (主键) */
} CharacFriends;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加好友关系（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的好友关系
 * @return 成功返回0，失败返回-1
 */
int CharacFriends_Add(DBConnectionManager* manager, const CharacFriends* record);

/**
 * 删除好友关系
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param friend_no 好友编号
 * @return 成功返回0，失败返回-1
 */
int CharacFriends_Delete(DBConnectionManager* manager, unsigned int charac_no,
                         unsigned int friend_no);

/**
 * 检查好友关系是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param friend_no 好友编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacFriends_Exists(DBConnectionManager* manager, unsigned int charac_no,
                         unsigned int friend_no);

/*******************************************************************************
 * 业务查询操作
 ******************************************************************************/

/**
 * 获取某角色的所有好友列表
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param friend_nos 输出参数，存储好友编号数组
 * @param max_friends 最多返回的好友数
 * @return 成功返回实际好友数，失败返回-1
 */
int CharacFriends_GetFriendsByCharacNo(DBConnectionManager* manager,
                                       unsigned int charac_no,
                                       unsigned int* friend_nos,
                                       int max_friends);

/**
 * 获取好友数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回好友数量，失败返回-1
 */
int CharacFriends_GetFriendsCount(DBConnectionManager* manager,
                                  unsigned int charac_no);

/**
 * 检查双向好友关系（A是B的好友 且 B是A的好友）
 * @param manager 数据库连接管理器
 * @param charac_no1 角色1编号
 * @param charac_no2 角色2编号
 * @return 双向好友返回1，否则返回0，错误返回-1
 */
int CharacFriends_IsMutualFriend(DBConnectionManager* manager,
                                 unsigned int charac_no1,
                                 unsigned int charac_no2);

/*******************************************************************************
 * 批量操作
 ******************************************************************************/

/**
 * 批量添加好友关系
 * @param manager 数据库连接管理器
 * @param records 好友关系数组
 * @param count 记录数量
 * @return 成功返回成功添加的记录数，失败返回-1
 */
int CharacFriends_BulkAdd(DBConnectionManager* manager,
                          const CharacFriends* records, int count);

/**
 * 删除某角色的所有好友关系
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacFriends_DeleteAllByCharacNo(DBConnectionManager* manager,
                                      unsigned int charac_no);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印好友关系信息（用于调试）
 * @param record 要打印的好友关系
 */
void CharacFriends_PrintInfo(const CharacFriends* record);

#endif /* CHARAC_FRIENDS_ORM_H */
