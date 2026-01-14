#ifndef NEW_CHARAC_QUEST_ORM_H
#define NEW_CHARAC_QUEST_ORM_H

#include "db_connection_manager.h"

/* 最大blob字段大小 */
#define MAX_BLOB_SIZE 65535

/* 游戏模式数量 */
#define MAX_PLAY_COUNT 20

/* 游戏模式进度结构 */
typedef struct {
    unsigned short play_id;      /* 游戏模式/任务ID */
    unsigned int trigger;        /* 触发器状态 */
} PlayProgress;

/* new_charac_quest 表数据结构 */
typedef struct {
    unsigned int charac_no;                    /* 角色编号（主键） */

    /* Blob 字段 */
    unsigned char clear_quest[MAX_BLOB_SIZE];  /* 已完成任务列表 */
    unsigned int clear_quest_len;              /* clear_quest 实际长度 */
    unsigned char quest_notify[MAX_BLOB_SIZE]; /* 任务通知数据 */
    unsigned int quest_notify_len;             /* quest_notify 实际长度 */

    /* 20个游戏模式进度 */
    unsigned short play_1;
    unsigned int play_1_trigger;
    unsigned short play_2;
    unsigned int play_2_trigger;
    unsigned short play_3;
    unsigned int play_3_trigger;
    unsigned short play_4;
    unsigned int play_4_trigger;
    unsigned short play_5;
    unsigned int play_5_trigger;
    unsigned short play_6;
    unsigned int play_6_trigger;
    unsigned short play_7;
    unsigned int play_7_trigger;
    unsigned short play_8;
    unsigned int play_8_trigger;
    unsigned short play_9;
    unsigned int play_9_trigger;
    unsigned short play_10;
    unsigned int play_10_trigger;
    unsigned short play_11;
    unsigned int play_11_trigger;
    unsigned short play_12;
    unsigned int play_12_trigger;
    unsigned short play_13;
    unsigned int play_13_trigger;
    unsigned short play_14;
    unsigned int play_14_trigger;
    unsigned short play_15;
    unsigned int play_15_trigger;
    unsigned short play_16;
    unsigned int play_16_trigger;
    unsigned short play_17;
    unsigned int play_17_trigger;
    unsigned short play_18;
    unsigned int play_18_trigger;
    unsigned short play_19;
    unsigned int play_19_trigger;
    unsigned short play_20;
    unsigned int play_20_trigger;

    /* 控制字段 */
    unsigned char auto_clear;                  /* 自动完成标志 */
    short urgent_quest_index;                  /* 紧急任务索引 */
} NewCharacQuest;

/* ========== CRUD 操作 ========== */

/**
 * 添加或更新角色任务记录
 * @param manager 数据库连接管理器
 * @param record 任务记录
 * @return 0成功，-1失败
 */
int NewCharacQuest_Add(DBConnectionManager* manager, const NewCharacQuest* record);

/**
 * 获取角色任务记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出的任务记录
 * @return 0成功，-1失败
 */
int NewCharacQuest_Get(DBConnectionManager* manager, unsigned int charac_no, NewCharacQuest* record);

/**
 * 更新角色任务记录
 * @param manager 数据库连接管理器
 * @param record 任务记录
 * @return 0成功，-1失败
 */
int NewCharacQuest_Update(DBConnectionManager* manager, const NewCharacQuest* record);

/**
 * 删除角色任务记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0成功，-1失败
 */
int NewCharacQuest_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色任务记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 1存在，0不存在，-1错误
 */
int NewCharacQuest_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ========== 专项操作 ========== */

/**
 * 获取指定游戏模式的进度
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param play_index 游戏模式索引 (1-20)
 * @param progress 输出的进度数据
 * @return 0成功，-1失败
 */
int NewCharacQuest_GetPlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index, PlayProgress* progress);

/**
 * 更新指定游戏模式的进度
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param play_index 游戏模式索引 (1-20)
 * @param progress 进度数据
 * @return 0成功，-1失败
 */
int NewCharacQuest_UpdatePlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index, const PlayProgress* progress);

/**
 * 清除角色所有游戏模式进度
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 0成功，-1失败
 */
int NewCharacQuest_ClearAllPlay(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 更新紧急任务索引
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param urgent_index 紧急任务索引
 * @return 0成功，-1失败
 */
int NewCharacQuest_UpdateUrgentQuest(DBConnectionManager* manager, unsigned int charac_no, short urgent_index);

/**
 * 设置自动完成标志
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param auto_clear 自动完成标志
 * @return 0成功，-1失败
 */
int NewCharacQuest_SetAutoClear(DBConnectionManager* manager, unsigned int charac_no, unsigned char auto_clear);

/* ========== 查询操作 ========== */

/**
 * 获取指定紧急任务索引的所有角色
 * @param manager 数据库连接管理器
 * @param urgent_index 紧急任务索引
 * @return 角色数量，-1失败
 */
int NewCharacQuest_CountByUrgentQuest(DBConnectionManager* manager, short urgent_index);

/**
 * 统计启用自动完成的角色数量
 * @param manager 数据库连接管理器
 * @return 角色数量，-1失败
 */
int NewCharacQuest_CountByAutoClear(DBConnectionManager* manager);

/**
 * 获取所有任务记录总数
 * @param manager 数据库连接管理器
 * @return 记录总数，-1失败
 */
int NewCharacQuest_Count(DBConnectionManager* manager);

/**
 * 检查指定游戏模式是否有进度
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param play_index 游戏模式索引 (1-20)
 * @return 1有进度，0无进度，-1错误
 */
int NewCharacQuest_HasPlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index);

/* ========== 工具函数 ========== */

/**
 * 打印角色任务信息（调试用）
 * @param record 任务记录
 */
void NewCharacQuest_PrintInfo(const NewCharacQuest* record);

#endif /* NEW_CHARAC_QUEST_ORM_H */
