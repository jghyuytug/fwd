#ifndef CHARAC_OPTION_ORM_H
#define CHARAC_OPTION_ORM_H

#include "db_connection_manager.h"

/**
 * 角色选项配置表 ORM
 *
 * 表: charac_option
 * 主键: charac_no
 * 功能: 记录角色配置选项（选项数据、最佳通关时间、蓝色弹珠、通知信息）
 */

#define MAX_BLOB_SIZE 65535
#define MAX_NOTICE_LEN 256

/* 数据结构 */
typedef struct {
    int charac_no;                          /* 角色编号 (主键) */
    unsigned char* options;                 /* 选项数据 (blob) */
    unsigned int options_len;               /* 选项数据长度 */
    unsigned char* best_clear_time;         /* 最佳通关时间 (blob) */
    unsigned int best_clear_time_len;       /* 最佳通关时间长度 */
    unsigned char blue_marble_enter_count;  /* 蓝色弹珠进入次数 */
    char charac_inform_notice[MAX_NOTICE_LEN]; /* 角色通知信息 */
} CharacOption;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色选项配置
 */
int CharacOption_Add(DBConnectionManager* manager, const CharacOption* record);

/**
 * 获取角色选项配置
 */
int CharacOption_Get(DBConnectionManager* manager, int charac_no, CharacOption* record);

/**
 * 更新角色选项配置
 */
int CharacOption_Update(DBConnectionManager* manager, const CharacOption* record);

/**
 * 删除角色选项配置
 */
int CharacOption_Delete(DBConnectionManager* manager, int charac_no);

/**
 * 检查角色选项配置是否存在
 */
int CharacOption_Exists(DBConnectionManager* manager, int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新选项数据
 */
int CharacOption_UpdateOptions(DBConnectionManager* manager, int charac_no,
                                const unsigned char* options, unsigned int options_len);

/**
 * 更新最佳通关时间
 */
int CharacOption_UpdateBestClearTime(DBConnectionManager* manager, int charac_no,
                                      const unsigned char* best_clear_time, unsigned int time_len);

/**
 * 更新蓝色弹珠进入次数
 */
int CharacOption_UpdateBlueMarbleEnterCount(DBConnectionManager* manager, int charac_no,
                                              unsigned char enter_count);

/**
 * 增加蓝色弹珠进入次数
 */
int CharacOption_IncrementBlueMarbleEnterCount(DBConnectionManager* manager, int charac_no, int increment);

/**
 * 更新角色通知信息
 */
int CharacOption_UpdateInformNotice(DBConnectionManager* manager, int charac_no,
                                     const char* notice);

/* ==================== 查询操作 ==================== */

/**
 * 获取总配置记录数
 */
int CharacOption_GetTotalCount(DBConnectionManager* manager);

/**
 * 获取指定蓝色弹珠进入次数范围的角色列表
 */
int CharacOption_GetByBlueMarbleCountRange(DBConnectionManager* manager,
                                             unsigned char min_count, unsigned char max_count,
                                             int* charac_nos, int max_result);

/**
 * 获取蓝色弹珠总进入次数
 */
long long CharacOption_GetTotalBlueMarbleEnterCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色选项配置信息（不包含二进制数据详情）
 */
void CharacOption_PrintInfo(const CharacOption* record);

/**
 * 释放CharacOption结构中的动态内存
 */
void CharacOption_Free(CharacOption* record);

#endif /* CHARAC_OPTION_ORM_H */
