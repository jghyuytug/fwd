#ifndef MEMBER_DUNGEON_ORM_H
#define MEMBER_DUNGEON_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_DUNGEON_DATA_LEN 4096

typedef struct {
    unsigned int m_id;                      /* 会员ID (主键) */
    char dungeon[MAX_DUNGEON_DATA_LEN];    /* 地下城数据 (TEXT字段, 序列化数据) */
} MemberDungeon;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新会员地下城数据 (使用 REPLACE INTO 实现幂等性)
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_Add(DBConnectionManager* manager, const MemberDungeon* record);

/**
 * 根据会员ID获取地下城数据
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int MemberDungeon_Get(DBConnectionManager* manager, unsigned int m_id, MemberDungeon* record);

/**
 * 更新会员的地下城数据
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param new_dungeon_data 新的地下城数据
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_Update(DBConnectionManager* manager, unsigned int m_id, const char* new_dungeon_data);

/**
 * 删除会员的地下城记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查会员是否有地下城记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int MemberDungeon_Exists(DBConnectionManager* manager, unsigned int m_id);

/*******************************************************************************
 * 业务查询操作
 ******************************************************************************/

/**
 * 获取地下城数据为空的会员列表
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int MemberDungeon_GetEmptyDungeonMembers(DBConnectionManager* manager,
                                          MemberDungeon* records, int max_records);

/**
 * 获取有地下城数据的会员列表
 * @param manager 数据库连接管理器
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int MemberDungeon_GetNonEmptyDungeonMembers(DBConnectionManager* manager,
                                              MemberDungeon* records, int max_records);

/**
 * 获取会员地下城数据的长度
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 成功返回数据长度，失败返回-1
 */
int MemberDungeon_GetDungeonLength(DBConnectionManager* manager, unsigned int m_id);

/**
 * 按关键字搜索地下城数据
 * @param manager 数据库连接管理器
 * @param keyword 搜索关键字
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int MemberDungeon_SearchDungeonByKeyword(DBConnectionManager* manager,
                                          const char* keyword,
                                          MemberDungeon* records, int max_records);

/**
 * 清空会员的地下城数据（设为空字符串）
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_ClearDungeonData(DBConnectionManager* manager, unsigned int m_id);

/**
 * 追加数据到会员的地下城字段（使用 CONCAT）
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param append_data 要追加的数据
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_AppendDungeonData(DBConnectionManager* manager,
                                     unsigned int m_id, const char* append_data);

/**
 * 按地下城数据大小范围获取会员
 * @param manager 数据库连接管理器
 * @param min_size 最小数据长度
 * @param max_size 最大数据长度
 * @param records 输出参数，存储查询结果数组
 * @param max_records 最多返回的记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int MemberDungeon_GetMembersByDungeonSize(DBConnectionManager* manager,
                                            int min_size, int max_size,
                                            MemberDungeon* records, int max_records);

/**
 * 统计有地下城数据的会员数量
 * @param manager 数据库连接管理器
 * @return 成功返回会员数量，失败返回-1
 */
int MemberDungeon_CountNonEmptyDungeons(DBConnectionManager* manager);

/**
 * 截断地下城数据到指定长度
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param max_length 最大长度
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_TruncateDungeonData(DBConnectionManager* manager,
                                       unsigned int m_id, int max_length);

/*******************************************************************************
 * 批量操作
 ******************************************************************************/

/**
 * 批量添加/更新会员地下城数据
 * @param manager 数据库连接管理器
 * @param records 要添加的记录数组
 * @param count 记录数量
 * @return 成功返回成功添加的记录数，失败返回-1
 */
int MemberDungeon_BulkAdd(DBConnectionManager* manager,
                           const MemberDungeon* records, int count);

/**
 * 批量删除指定会员ID的地下城数据
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count ID数量
 * @return 成功返回0，失败返回-1
 */
int MemberDungeon_BulkDeleteByMemberIds(DBConnectionManager* manager,
                                         const unsigned int* m_ids, int count);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印会员地下城记录信息（用于调试）
 * @param record 要打印的记录
 */
void MemberDungeon_PrintInfo(const MemberDungeon* record);

#endif /* MEMBER_DUNGEON_ORM_H */
