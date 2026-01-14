#ifndef DNF_EVENT_INFO_ORM_H
#define DNF_EVENT_INFO_ORM_H

#include "db_connection_manager.h"

/* dnf_event_info 表数据结构
 * DNF活动信息（活动管理核心表）
 * 主键: event_id
 * 唯一键: event_name
 */
typedef struct {
    int event_id;              // 活动ID（主键）
    char event_name[31];       // 活动名称 (varchar(30) + '\0', 唯一)
    char event_explain[101];   // 活动说明 (varchar(100) + '\0')
    char apply_type;           // 申请类型 (tinyint(4))
    char start_date[11];       // 开始日期 (YYYY-MM-DD)
    char end_date[11];         // 结束日期 (YYYY-MM-DD)
} DnfEventInfo;

/* CRUD 操作 */

/**
 * 添加或更新活动信息
 * @param manager 数据库连接管理器
 * @param record 活动信息记录
 * @return 0成功, <0失败
 */
int DnfEventInfo_Add(DBConnectionManager* manager, const DnfEventInfo* record);

/**
 * 根据活动ID获取活动信息
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param record 输出参数，活动信息记录
 * @return 0成功, <0失败
 */
int DnfEventInfo_Get(DBConnectionManager* manager, int event_id, DnfEventInfo* record);

/**
 * 根据活动名称获取活动信息
 * @param manager 数据库连接管理器
 * @param event_name 活动名称
 * @param record 输出参数，活动信息记录
 * @return 0成功, <0失败
 */
int DnfEventInfo_GetByName(DBConnectionManager* manager, const char* event_name, DnfEventInfo* record);

/**
 * 更新活动信息
 * @param manager 数据库连接管理器
 * @param record 活动信息记录
 * @return 0成功, <0失败
 */
int DnfEventInfo_Update(DBConnectionManager* manager, const DnfEventInfo* record);

/**
 * 删除活动信息
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @return 0成功, <0失败
 */
int DnfEventInfo_Delete(DBConnectionManager* manager, int event_id);

/**
 * 检查活动是否存在（根据ID）
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @return 1存在, 0不存在, <0失败
 */
int DnfEventInfo_Exists(DBConnectionManager* manager, int event_id);

/**
 * 检查活动名称是否存在
 * @param manager 数据库连接管理器
 * @param event_name 活动名称
 * @return 1存在, 0不存在, <0失败
 */
int DnfEventInfo_NameExists(DBConnectionManager* manager, const char* event_name);

/* 业务操作 */

/**
 * 根据申请类型获取活动列表
 * @param manager 数据库连接管理器
 * @param apply_type 申请类型
 * @param records 输出参数，活动信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventInfo_GetByApplyType(DBConnectionManager* manager, char apply_type, DnfEventInfo** records, int* count);

/**
 * 获取指定日期正在进行的活动
 * @param manager 数据库连接管理器
 * @param date 查询日期 (YYYY-MM-DD)
 * @param records 输出参数，活动信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventInfo_GetActiveEvents(DBConnectionManager* manager, const char* date, DnfEventInfo** records, int* count);

/**
 * 获取日期范围内的活动
 * @param manager 数据库连接管理器
 * @param start_date 起始日期 (YYYY-MM-DD)
 * @param end_date 结束日期 (YYYY-MM-DD)
 * @param records 输出参数，活动信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventInfo_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, DnfEventInfo** records, int* count);

/**
 * 根据活动名称模糊搜索
 * @param manager 数据库连接管理器
 * @param pattern 搜索模式
 * @param records 输出参数，活动信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventInfo_SearchByName(DBConnectionManager* manager, const char* pattern, DnfEventInfo** records, int* count);

/**
 * 获取所有活动信息
 * @param manager 数据库连接管理器
 * @param records 输出参数，活动信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfEventInfo_GetAll(DBConnectionManager* manager, DnfEventInfo** records, int* count);

/**
 * 检查活动是否已过期
 * @param manager 数据库连接管理器
 * @param event_id 活动ID
 * @param current_date 当前日期 (YYYY-MM-DD)
 * @return 1已过期, 0未过期, <0失败
 */
int DnfEventInfo_IsExpired(DBConnectionManager* manager, int event_id, const char* current_date);

/* 工具函数 */

/**
 * 打印活动信息
 * @param record 活动信息记录
 */
void DnfEventInfo_PrintInfo(const DnfEventInfo* record);

/**
 * 释放记录数组内存
 * @param records 活动信息数组
 */
void DnfEventInfo_FreeRecords(DnfEventInfo* records);

#endif /* DNF_EVENT_INFO_ORM_H */
