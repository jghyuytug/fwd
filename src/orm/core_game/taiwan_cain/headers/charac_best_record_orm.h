#ifndef CHARAC_BEST_RECORD_ORM_H
#define CHARAC_BEST_RECORD_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

typedef struct {
    unsigned int charac_no;      /* 角色编号 (主键1) */
    short dungeon_no;            /* 副本编号 (主键2) */
    short difficulty;            /* 难度 (主键3) */
    int style;                   /* 风格分数 */
    int technic;                 /* 技巧分数 */
    int attacked;                /* 被攻击次数 */
    int rank;                    /* 排名 */
} CharacBestRecord;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色最佳记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_Add(DBConnectionManager* manager, const CharacBestRecord* record);

/**
 * 根据复合主键获取最佳记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param difficulty 难度
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacBestRecord_Get(DBConnectionManager* manager, unsigned int charac_no,
                         short dungeon_no, short difficulty,
                         CharacBestRecord* record);

/**
 * 更新最佳记录
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_Update(DBConnectionManager* manager, const CharacBestRecord* record);

/**
 * 删除最佳记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param difficulty 难度
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_Delete(DBConnectionManager* manager, unsigned int charac_no,
                            short dungeon_no, short difficulty);

/**
 * 检查最佳记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param difficulty 难度
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacBestRecord_Exists(DBConnectionManager* manager, unsigned int charac_no,
                            short dungeon_no, short difficulty);

/**
 * 删除角色的所有最佳记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_DeleteByCharac(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 更新评分（风格、技巧、被攻击次数）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param difficulty 难度
 * @param style 风格分数
 * @param technic 技巧分数
 * @param attacked 被攻击次数
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_UpdateScores(DBConnectionManager* manager, unsigned int charac_no,
                                  short dungeon_no, short difficulty,
                                  int style, int technic, int attacked);

/**
 * 更新排名
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param difficulty 难度
 * @param rank 新排名
 * @return 成功返回0，失败返回-1
 */
int CharacBestRecord_UpdateRank(DBConnectionManager* manager, unsigned int charac_no,
                                short dungeon_no, short difficulty, int rank);

/**
 * 获取角色在某副本的所有难度记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon_no 副本编号
 * @param records 输出参数，存储记录数组（调用者需要提前分配足够空间）
 * @param max_count 最大记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int CharacBestRecord_GetByDungeon(DBConnectionManager* manager, unsigned int charac_no,
                                  short dungeon_no, CharacBestRecord* records,
                                  int max_count);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印最佳记录信息（用于调试）
 * @param record 要打印的记录
 */
void CharacBestRecord_PrintInfo(const CharacBestRecord* record);

#endif /* CHARAC_BEST_RECORD_ORM_H */
