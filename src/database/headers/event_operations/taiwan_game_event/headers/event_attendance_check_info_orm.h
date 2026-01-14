#ifndef EVENT_ATTENDANCE_CHECK_INFO_ORM_H
#define EVENT_ATTENDANCE_CHECK_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char base_check_date[20];
    signed char base_check_cnt;
    char bonus_check_date[20];
    signed char bonus_check_cnt;
} EventAttendanceCheckInfo;

/* CRUD Operations */
int EventAttendanceCheckInfo_Add(DBConnectionManager* manager, const EventAttendanceCheckInfo* record);
int EventAttendanceCheckInfo_Get(DBConnectionManager* manager, int m_id, EventAttendanceCheckInfo* record);
int EventAttendanceCheckInfo_Update(DBConnectionManager* manager, const EventAttendanceCheckInfo* record);
int EventAttendanceCheckInfo_Delete(DBConnectionManager* manager, int m_id);
int EventAttendanceCheckInfo_Exists(DBConnectionManager* manager, int m_id);
int EventAttendanceCheckInfo_GetAll(DBConnectionManager* manager, EventAttendanceCheckInfo* records, int max_count, int* actual_count);

#endif /* EVENT_ATTENDANCE_CHECK_INFO_ORM_H */