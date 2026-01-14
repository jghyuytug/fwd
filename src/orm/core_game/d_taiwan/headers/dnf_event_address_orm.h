#ifndef DNF_EVENT_ADDRESS_ORM_H
#define DNF_EVENT_ADDRESS_ORM_H

#include "db_connection_manager.h"

/* dnf_event_address 表数据结构
 * DNF活动地址信息
 * 主键: (event_id, m_id)
 */
typedef struct {
    int event_id;              // 活动ID
    int m_id;                  // 会员ID
    char occ_date[20];         // 发生日期 (YYYY-MM-DD HH:MM:SS)
    char zipcode[8];           // 邮编 (varchar(7) + '\0')
    char address[151];         // 地址 (varchar(150) + '\0')
    char phone_no[16];         // 电话号码 (varchar(15) + '\0')
} DnfEventAddress;

/* CRUD 操作 */

/**
 * 添加或更新活动地址
 * @param manager 数据库连接管理器
 * @param record 活动地址记录
 * @return 0成功, <0失败
 */
int DnfEventAddress_Add(DBConnectionManager* manager, const DnfEventAddress* record);

/**
 * 获取活动地址
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 会员ID
 * @param record 输出参数，活动地址记录
 * @return 0成功, <0失败
 */
int DnfEventAddress_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventAddress* record);

/**
 * 更新活动地址
 * @param manager 数据库连接管理器
 * @param record 活动地址记录
 * @return 0成功, <0失败
 */
int DnfEventAddress_Update(DBConnectionManager* manager, const DnfEventAddress* record);

/**
 * 删除活动地址
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 会员ID
 * @return 0成功, <0失败
 */
int DnfEventAddress_Delete(DBConnectionManager* manager, int event_id, int m_id);

/**
 * 检查活动地址是否存在
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param m_id 会员ID
 * @return 1存在, 0不存在, <0失败
 */
int DnfEventAddress_Exists(DBConnectionManager* manager, int event_id, int m_id);

/* 业务操作 */

/**
 * 根据活动ID获取所有地址
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param records 输出参数，活动地址数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventAddress_GetByEventId(DBConnectionManager* manager, int event_id, DnfEventAddress** records, int* count);

/**
 * 根据会员ID获取所有活动地址
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出参数，活动地址数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventAddress_GetByMemberId(DBConnectionManager* manager, int m_id, DnfEventAddress** records, int* count);

/**
 * 根据邮编搜索地址
 * @param manager 数据库连接管理器
 * @param zipcode 邮编
 * @param records 输出参数，活动地址数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventAddress_GetByZipcode(DBConnectionManager* manager, const char* zipcode, DnfEventAddress** records, int* count);

/**
 * 根据日期范围获取活动地址
 * @param manager 数据库连接管理器
 * @param start_date 起始日期 (YYYY-MM-DD HH:MM:SS)
 * @param end_date 结束日期 (YYYY-MM-DD HH:MM:SS)
 * @param records 输出参数，活动地址数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventAddress_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, DnfEventAddress** records, int* count);

/**
 * 获取所有活动地址
 * @param manager 数据库连接管理器
 * @param records 输出参数，活动地址数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventAddress_GetAll(DBConnectionManager* manager, DnfEventAddress** records, int* count);

/* 工具函数 */

/**
 * 打印活动地址信息
 * @param record 活动地址记录
 */
void DnfEventAddress_PrintInfo(const DnfEventAddress* record);

/**
 * 释放记录数组内存
 * @param records 活动地址数组
 */
void DnfEventAddress_FreeRecords(DnfEventAddress* records);

#endif /* DNF_EVENT_ADDRESS_ORM_H */
