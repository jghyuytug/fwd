#ifndef GAME_CHANNEL_ORM_H
#define GAME_CHANNEL_ORM_H

#include "db_connection_manager.h"

/* game_channel 表数据结构 */
typedef struct {
    unsigned int gc_no;                /* 频道编号（主键，自增）*/
    unsigned short gc_now;             /* 当前在线人数 */
    char gc_ip[65];                    /* 服务器IP地址 */
    unsigned short gc_port;            /* 服务器端口 */
    unsigned short gc_max;             /* 最大容纳人数 */
    unsigned char gc_game;             /* 游戏类型 */
    char gc_channel[33];               /* 频道名称 */
    short gc_ch_group;                 /* 频道组 */
    char gc_channeltype[26];           /* 频道类型 */
    char gc_up_time[20];               /* 更新时间 */
    unsigned short gc_swordman_cnt;    /* 鬼剑士数量 */
    unsigned short gc_fighter_cnt;     /* 格斗家数量 */
    unsigned short gc_gunner_cnt;      /* 神枪手数量 */
    unsigned short gc_mage_cnt;        /* 魔法师数量 */
    unsigned short gc_priest_cnt;      /* 圣职者数量 */
    unsigned short gc_at_gunner_cnt;   /* 枪剑士数量 */
    unsigned short gc_thief_cnt;       /* 盗贼数量 */
    unsigned short gc_hangame;         /* Hangame平台玩家数 */
    unsigned short gc_nexon;           /* Nexon平台玩家数 */
    char gc_type;                      /* 类型标志 */
} GameChannel;

/* ========== CRUD 操作 ========== */

/**
 * 添加游戏频道
 * @param manager 数据库连接管理器
 * @param channel 频道记录
 * @return 新插入记录的ID，-1失败
 */
int GameChannel_Add(DBConnectionManager* manager, const GameChannel* channel);

/**
 * 获取游戏频道
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @param channel 输出的频道记录
 * @return 0成功，-1失败
 */
int GameChannel_Get(DBConnectionManager* manager, unsigned int gc_no, GameChannel* channel);

/**
 * 更新游戏频道
 * @param manager 数据库连接管理器
 * @param channel 频道记录
 * @return 0成功，-1失败
 */
int GameChannel_Update(DBConnectionManager* manager, const GameChannel* channel);

/**
 * 删除游戏频道
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @return 0成功，-1失败
 */
int GameChannel_Delete(DBConnectionManager* manager, unsigned int gc_no);

/**
 * 检查频道是否存在
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @return 1存在，0不存在，-1错误
 */
int GameChannel_Exists(DBConnectionManager* manager, unsigned int gc_no);

/* ========== 专项操作 ========== */

/**
 * 更新当前在线人数
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @param gc_now 当前在线人数
 * @return 0成功，-1失败
 */
int GameChannel_UpdateOnlineCount(DBConnectionManager* manager, unsigned int gc_no, unsigned short gc_now);

/**
 * 更新职业人数统计
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @param swordman 鬼剑士数量
 * @param fighter 格斗家数量
 * @param gunner 神枪手数量
 * @param mage 魔法师数量
 * @param priest 圣职者数量
 * @param at_gunner 枪剑士数量
 * @param thief 盗贼数量
 * @return 0成功，-1失败
 */
int GameChannel_UpdateJobCounts(DBConnectionManager* manager, unsigned int gc_no,
                                unsigned short swordman, unsigned short fighter, unsigned short gunner,
                                unsigned short mage, unsigned short priest, unsigned short at_gunner,
                                unsigned short thief);

/**
 * 更新频道时间戳
 * @param manager 数据库连接管理器
 * @param gc_no 频道编号
 * @return 0成功，-1失败
 */
int GameChannel_UpdateTimestamp(DBConnectionManager* manager, unsigned int gc_no);

/* ========== 查询操作 ========== */

/**
 * 获取所有频道总数
 * @param manager 数据库连接管理器
 * @return 频道总数，-1失败
 */
int GameChannel_Count(DBConnectionManager* manager);

/**
 * 获取指定频道组的频道数量
 * @param manager 数据库连接管理器
 * @param gc_ch_group 频道组
 * @return 频道数量，-1失败
 */
int GameChannel_CountByGroup(DBConnectionManager* manager, short gc_ch_group);

/**
 * 获取所有频道的总在线人数
 * @param manager 数据库连接管理器
 * @return 总在线人数，-1失败
 */
int GameChannel_GetTotalOnline(DBConnectionManager* manager);

/**
 * 获取指定职业的总人数
 * @param manager 数据库连接管理器
 * @param job_type 职业类型（0=鬼剑士,1=格斗家,2=神枪手,3=魔法师,4=圣职者,5=枪剑士,6=盗贼）
 * @return 职业总人数，-1失败
 */
int GameChannel_GetTotalJobCount(DBConnectionManager* manager, int job_type);

/**
 * 删除指定频道组的所有频道
 * @param manager 数据库连接管理器
 * @param gc_ch_group 频道组
 * @return 0成功，-1失败
 */
int GameChannel_DeleteByGroup(DBConnectionManager* manager, short gc_ch_group);

/* ========== 工具函数 ========== */

/**
 * 打印游戏频道信息（调试用）
 * @param channel 频道记录
 */
void GameChannel_PrintInfo(const GameChannel* channel);

#endif /* GAME_CHANNEL_ORM_H */
