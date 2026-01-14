#ifndef CHANNEL_OCC_INFO_ORM_H
#define CHANNEL_OCC_INFO_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Channel Occ Info ORM - 频道占用信息系统
 *
 * 表: channel_occ_info (taiwan_cain 数据库)
 * 描述: 游戏频道占用人数统计表
 * 主键: (gc_no, age) 复合主键
 *
 * 字段说明:
 * - gc_no: 游戏频道编号 (INT UNSIGNED, PRIMARY KEY 1)
 * - age: 年龄段 (TINYINT UNSIGNED, PRIMARY KEY 2)
 * - occ_num: 当前占用人数 (SMALLINT)
 *
 * 业务用途:
 * - 统计各频道不同年龄段玩家数量
 * - 负载均衡和频道分配
 * - 玩家分布分析
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个 (含按频道查询)
 * - 批量操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #59/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 频道占用信息记录结构 */
typedef struct {
    unsigned int gc_no;       /* 游戏频道编号 (复合主键1) */
    unsigned char age;        /* 年龄段 (复合主键2) */
    short occ_num;            /* 当前占用人数 */
} ChannelOccInfo;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新频道占用信息（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int ChannelOccInfo_Add(DBConnectionManager* manager, const ChannelOccInfo* record);

/**
 * 获取指定频道和年龄段的占用信息
 * @param manager 数据库连接管理器
 * @param gc_no 游戏频道编号
 * @param age 年龄段
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int ChannelOccInfo_Get(DBConnectionManager* manager, unsigned int gc_no, unsigned char age, ChannelOccInfo* record);

/**
 * 更新频道占用信息
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int ChannelOccInfo_Update(DBConnectionManager* manager, const ChannelOccInfo* record);

/**
 * 删除频道占用信息
 * @param manager 数据库连接管理器
 * @param gc_no 游戏频道编号
 * @param age 年龄段
 * @return 成功返回0，失败返回-1
 */
int ChannelOccInfo_Delete(DBConnectionManager* manager, unsigned int gc_no, unsigned char age);

/**
 * 检查频道占用信息是否存在
 * @param manager 数据库连接管理器
 * @param gc_no 游戏频道编号
 * @param age 年龄段
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int ChannelOccInfo_Exists(DBConnectionManager* manager, unsigned int gc_no, unsigned char age);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取占用信息记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int ChannelOccInfo_Count(DBConnectionManager* manager);

/**
 * 获取所有占用信息记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ChannelOccInfo_GetAll(DBConnectionManager* manager, ChannelOccInfo* records, int max_records);

/**
 * 按频道编号获取所有年龄段的占用信息
 * @param manager 数据库连接管理器
 * @param gc_no 游戏频道编号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int ChannelOccInfo_GetByChannel(DBConnectionManager* manager, unsigned int gc_no, ChannelOccInfo* records, int max_records);

/**
 * 统计指定频道的年龄段记录数
 * @param manager 数据库连接管理器
 * @param gc_no 游戏频道编号
 * @return >=0记录数，<0失败
 */
int ChannelOccInfo_CountByChannel(DBConnectionManager* manager, unsigned int gc_no);

/*******************************************************************************
 * 批量操作 (1个函数)
 ******************************************************************************/

/**
 * 批量删除频道占用信息
 * @param manager 数据库连接管理器
 * @param records 要删除的记录数组
 * @param count 数组长度
 * @return 成功返回0，失败返回-1
 */
int ChannelOccInfo_BatchDelete(DBConnectionManager* manager, const ChannelOccInfo* records, int count);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印频道占用信息（用于调试）
 * @param record 频道占用信息记录指针
 */
void ChannelOccInfo_PrintInfo(const ChannelOccInfo* record);

#endif /* CHANNEL_OCC_INFO_ORM_H */
