#ifndef EVENT_ARAD_BIRTHDAY_6TH_ORM_H
#define EVENT_ARAD_BIRTHDAY_6TH_ORM_H

#include "db_connection_manager.h"

/* event_arad_birthday_6th 表数据结构
 * 阿拉德6周年活动服务器积分表
 * 主键: server
 */
typedef struct {
    unsigned int server;  // 服务器ID（主键）
    unsigned int point;   // 积分
} EventAradBirthday6th;

/* CRUD 操作 */
int EventAradBirthday6th_Add(DBConnectionManager* manager, const EventAradBirthday6th* record);
int EventAradBirthday6th_Get(DBConnectionManager* manager, unsigned int server, EventAradBirthday6th* record);
int EventAradBirthday6th_Update(DBConnectionManager* manager, const EventAradBirthday6th* record);
int EventAradBirthday6th_Delete(DBConnectionManager* manager, unsigned int server);
int EventAradBirthday6th_Exists(DBConnectionManager* manager, unsigned int server);

/* 业务操作 */
int EventAradBirthday6th_GetAll(DBConnectionManager* manager, EventAradBirthday6th** records, int* count);
int EventAradBirthday6th_GetTopServers(DBConnectionManager* manager, int limit, EventAradBirthday6th** records, int* count);
int EventAradBirthday6th_AddPoints(DBConnectionManager* manager, unsigned int server, unsigned int points);
unsigned int EventAradBirthday6th_GetTotalPoints(DBConnectionManager* manager);

/* 工具函数 */
void EventAradBirthday6th_PrintInfo(const EventAradBirthday6th* record);
void EventAradBirthday6th_FreeRecords(EventAradBirthday6th* records);

#endif /* EVENT_ARAD_BIRTHDAY_6TH_ORM_H */
