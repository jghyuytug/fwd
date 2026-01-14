/**
 * DNF游戏服务器 - CharacFriends表ORM接口
 *
 * 功能: 角色好友关系管理
 * 表: taiwan_cain.charac_friends
 *
 * 表结构:
 *   - charac_no (int)   - 角色编号
 *   - friend_no (int)   - 好友编号
 *   - PRIMARY KEY (charac_no, friend_no)
 */

#ifndef CHARAC_FRIENDS_ORM_H
#define CHARAC_FRIENDS_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// 数据结构定义
// ========================================================================

/**
 * 好友关系结构
 */
typedef struct {
    int charac_no;     // 角色编号
    int friend_no;     // 好友编号
} CharacFriend;

// ========================================================================
// CRUD操作接口
// ========================================================================

/**
 * 添加好友关系
 *
 * @param manager - 数据库连接管理器
 * @param friend - 好友关系信息
 * @return 0=成功, -1=失败
 *
 * 注意: 幂等操作 - 重复添加相同好友返回成功
 */
int CharacFriends_Add(DBConnectionManager* manager, CharacFriend* friend);

/**
 * 删除好友关系
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @param friend_no - 好友编号
 * @return 0=成功, -1=失败
 *
 * 注意: 幂等操作 - 删除不存在的好友关系返回成功
 */
int CharacFriends_Remove(DBConnectionManager* manager, int charac_no, int friend_no);

/**
 * 获取角色的所有好友列表
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @param friends - 好友列表数组（输出）
 * @param max_count - 数组最大容量
 * @return 好友数量(>=0), -1=失败
 */
int CharacFriends_GetList(DBConnectionManager* manager, int charac_no,
                          CharacFriend* friends, int max_count);

/**
 * 删除角色的所有好友关系
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @return 0=成功, -1=失败
 */
int CharacFriends_RemoveAll(DBConnectionManager* manager, int charac_no);

// ========================================================================
// 查询辅助函数
// ========================================================================

/**
 * 检查是否为好友关系
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @param friend_no - 好友编号
 * @return true=是好友, false=不是好友或查询失败
 */
bool CharacFriends_IsFriend(DBConnectionManager* manager, int charac_no, int friend_no);

/**
 * 获取角色的好友数量
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @return 好友数量(>=0), -1=失败
 */
int CharacFriends_GetCount(DBConnectionManager* manager, int charac_no);

/**
 * 检查双向好友关系（互为好友）
 *
 * @param manager - 数据库连接管理器
 * @param charac_no1 - 角色1编号
 * @param charac_no2 - 角色2编号
 * @return true=互为好友, false=不是或查询失败
 */
bool CharacFriends_IsMutual(DBConnectionManager* manager, int charac_no1, int charac_no2);

/**
 * 获取某角色的粉丝列表（把该角色加为好友的其他角色）
 *
 * @param manager - 数据库连接管理器
 * @param charac_no - 角色编号
 * @param fans - 粉丝列表数组（输出）
 * @param max_count - 数组最大容量
 * @return 粉丝数量(>=0), -1=失败
 */
int CharacFriends_GetFans(DBConnectionManager* manager, int charac_no,
                          CharacFriend* fans, int max_count);

// ========================================================================
// 批量操作接口
// ========================================================================

/**
 * 批量添加好友关系
 *
 * @param manager - 数据库连接管理器
 * @param friends - 好友关系数组
 * @param count - 数组元素数量
 * @return 成功添加的数量(>=0), -1=失败
 */
int CharacFriends_AddBatch(DBConnectionManager* manager, CharacFriend* friends, int count);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化好友关系结构
 *
 * @param friend - 好友关系结构指针
 */
void CharacFriend_Init(CharacFriend* friend);

/**
 * 打印好友关系信息（调试用）
 *
 * @param friend - 好友关系结构指针
 */
void CharacFriend_Print(const CharacFriend* friend);

#endif // CHARAC_FRIENDS_ORM_H
