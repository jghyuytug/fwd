#ifndef CHARAC_NPC_ORM_H
#define CHARAC_NPC_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_NPC_DATA_LEN 8192

typedef struct {
    unsigned int charac_no;                      /* 角色编号 (主键) */
    unsigned char npc_cnt;                       /* NPC数量 */
    unsigned char npc_data[MAX_NPC_DATA_LEN];   /* NPC数据 (BLOB) */
    int npc_data_len;                            /* NPC数据实际长度 */
} CharacNpc;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色NPC记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_Add(DBConnectionManager* manager, const CharacNpc* record);

/**
 * 根据角色编号获取NPC记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacNpc_Get(DBConnectionManager* manager, unsigned int charac_no,
                  CharacNpc* record);

/**
 * 更新NPC基础信息
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_Update(DBConnectionManager* manager, const CharacNpc* record);

/**
 * 删除NPC记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查NPC记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacNpc_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 更新NPC数据
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param npc_data 新的NPC数据
 * @param npc_data_len NPC数据长度
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_UpdateNpcData(DBConnectionManager* manager, unsigned int charac_no,
                            const unsigned char* npc_data, int npc_data_len);

/**
 * 更新NPC数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param npc_cnt 新的NPC数量
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_UpdateNpcCnt(DBConnectionManager* manager, unsigned int charac_no,
                          unsigned char npc_cnt);

/**
 * 递增NPC数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param increment 增加的数量（默认为1）
 * @return 成功返回0，失败返回-1
 */
int CharacNpc_IncrementNpcCnt(DBConnectionManager* manager, unsigned int charac_no,
                             unsigned int increment);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印NPC信息（用于调试）
 * @param record 要打印的记录
 */
void CharacNpc_PrintInfo(const CharacNpc* record);

#endif /* CHARAC_NPC_ORM_H */
