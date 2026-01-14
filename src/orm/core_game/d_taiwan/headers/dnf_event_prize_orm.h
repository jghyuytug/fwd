#ifndef DNF_EVENT_PRIZE_ORM_H
#define DNF_EVENT_PRIZE_ORM_H

#include "db_connection_manager.h"

/* dnf_event_prize 表数据结构
 * DNF活动奖品管理
 * 主键: (prize_id, m_id)
 * 索引: m_id
 */
typedef struct {
    int prize_id;      // 奖品ID
    int m_id;          // 会员ID
    int check_time;    // 检查时间（Unix时间戳）
} DnfEventPrize;

/* CRUD 操作 */

/**
 * 添加或更新活动奖品记录
 * @param manager 数据库连接管理器
 * @param record 活动奖品记录
 * @return 0成功, <0失败
 */
int DnfEventPrize_Add(DBConnectionManager* manager, const DnfEventPrize* record);

/**
 * 获取活动奖品记录
 * @param manager 数据库连接管理器
 * @param prize_id 奖品ID
 * @param m_id 会员ID
 * @param record 输出参数，活动奖品记录
 * @return 0成功, <0失败
 */
int DnfEventPrize_Get(DBConnectionManager* manager, int prize_id, int m_id, DnfEventPrize* record);

/**
 * 更新活动奖品记录
 * @param manager 数据库连接管理器
 * @param record 活动奖品记录
 * @return 0成功, <0失败
 */
int DnfEventPrize_Update(DBConnectionManager* manager, const DnfEventPrize* record);

/**
 * 删除活动奖品记录
 * @param manager 数据库连接管理器
 * @param prize_id 奖品ID
 * @param m_id 会员ID
 * @return 0成功, <0失败
 */
int DnfEventPrize_Delete(DBConnectionManager* manager, int prize_id, int m_id);

/**
 * 检查活动奖品记录是否存在
 * @param manager 数据库连接管理器
 * @param prize_id 奖品ID
 * @param m_id 会员ID
 * @return 1存在, 0不存在, <0失败
 */
int DnfEventPrize_Exists(DBConnectionManager* manager, int prize_id, int m_id);

/* 业务操作 */

/**
 * 根据奖品ID获取所有获奖记录
 * @param manager 数据库连接管理器
 * @param prize_id 奖品ID
 * @param records 输出参数，活动奖品记录数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventPrize_GetByPrizeId(DBConnectionManager* manager, int prize_id, DnfEventPrize** records, int* count);

/**
 * 根据会员ID获取所有获奖记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出参数，活动奖品记录数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventPrize_GetByMemberId(DBConnectionManager* manager, int m_id, DnfEventPrize** records, int* count);

/**
 * 根据时间范围获取奖品记录
 * @param manager 数据库连接管理器
 * @param start_time 起始时间（Unix时间戳）
 * @param end_time 结束时间（Unix时间戳）
 * @param records 输出参数，活动奖品记录数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventPrize_GetByTimeRange(DBConnectionManager* manager, int start_time, int end_time, DnfEventPrize** records, int* count);

/**
 * 获取所有活动奖品记录
 * @param manager 数据库连接管理器
 * @param records 输出参数，活动奖品记录数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventPrize_GetAll(DBConnectionManager* manager, DnfEventPrize** records, int* count);

/**
 * 统计指定奖品的获奖人数
 * @param manager 数据库连接管理器
 * @param prize_id 奖品ID
 * @return 获奖人数, <0失败
 */
int DnfEventPrize_CountByPrizeId(DBConnectionManager* manager, int prize_id);

/**
 * 统计指定会员获得的奖品数量
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 奖品数量, <0失败
 */
int DnfEventPrize_CountByMemberId(DBConnectionManager* manager, int m_id);

/* 工具函数 */

/**
 * 打印活动奖品记录信息
 * @param record 活动奖品记录
 */
void DnfEventPrize_PrintInfo(const DnfEventPrize* record);

/**
 * 释放记录数组内存
 * @param records 活动奖品记录数组
 */
void DnfEventPrize_FreeRecords(DnfEventPrize* records);

#endif /* DNF_EVENT_PRIZE_ORM_H */
