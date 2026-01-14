#ifndef CHARAC_DUNGEON_ORM_H
#define CHARAC_DUNGEON_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_DUNGEON_DATA_LEN 8192
#define MAX_BEST_CLEAR_TIME_LEN 4096
#define MAX_NOTICE_LEN 255

typedef struct {
    unsigned int charac_no;                              /* 角色编号 (主键) */
    unsigned char dungeon[MAX_DUNGEON_DATA_LEN];         /* 副本数据 (BLOB) */
    int dungeon_len;                                     /* 副本数据实际长度 */
    unsigned char best_clear_time[MAX_BEST_CLEAR_TIME_LEN]; /* 最佳通关时间 (BLOB) */
    int best_clear_time_len;                             /* 通关时间实际长度 */
    unsigned char blue_marble_enter_count;               /* 蓝色大理石进入次数 */
    char charac_inform_notice[MAX_NOTICE_LEN + 1];      /* 角色通知信息 */
} CharacDungeon;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色副本记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_Add(DBConnectionManager* manager, const CharacDungeon* record);

/**
 * 根据角色编号获取副本记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacDungeon_Get(DBConnectionManager* manager, unsigned int charac_no,
                      CharacDungeon* record);

/**
 * 更新副本基础信息
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_Update(DBConnectionManager* manager, const CharacDungeon* record);

/**
 * 删除副本记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查副本记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacDungeon_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 更新副本数据
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param dungeon 新的副本数据
 * @param dungeon_len 副本数据长度
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_UpdateDungeonData(DBConnectionManager* manager, unsigned int charac_no,
                                    const unsigned char* dungeon, int dungeon_len);

/**
 * 更新最佳通关时间
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param best_clear_time 新的最佳通关时间数据
 * @param best_clear_time_len 数据长度
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_UpdateBestClearTime(DBConnectionManager* manager, unsigned int charac_no,
                                      const unsigned char* best_clear_time,
                                      int best_clear_time_len);

/**
 * 更新通知信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param notice 新的通知信息
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_UpdateNotice(DBConnectionManager* manager, unsigned int charac_no,
                               const char* notice);

/**
 * 递增蓝色大理石进入次数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param increment 增加的次数（默认为1）
 * @return 成功返回0，失败返回-1
 */
int CharacDungeon_IncrementBlueMarbleCount(DBConnectionManager* manager,
                                           unsigned int charac_no,
                                           unsigned int increment);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印副本信息（用于调试）
 * @param record 要打印的记录
 */
void CharacDungeon_PrintInfo(const CharacDungeon* record);

#endif /* CHARAC_DUNGEON_ORM_H */
