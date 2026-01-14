#ifndef EVENT_1202_FATIGUE_ATTENDANCE_ORM_H
#define EVENT_1202_FATIGUE_ATTENDANCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char occ_day;
    int m_id;
    short fatigue;
} Event1202FatigueAttendance;

/* CRUD Operations */
int Event1202FatigueAttendance_Add(DBConnectionManager* manager, const Event1202FatigueAttendance* record);
int Event1202FatigueAttendance_Get(DBConnectionManager* manager, signed char occ_day, int m_id, Event1202FatigueAttendance* record);
int Event1202FatigueAttendance_Update(DBConnectionManager* manager, const Event1202FatigueAttendance* record);
int Event1202FatigueAttendance_Delete(DBConnectionManager* manager, signed char occ_day, int m_id);
int Event1202FatigueAttendance_Exists(DBConnectionManager* manager, signed char occ_day, int m_id);
int Event1202FatigueAttendance_GetAll(DBConnectionManager* manager, Event1202FatigueAttendance* records, int max_count, int* actual_count);

#endif /* EVENT_1202_FATIGUE_ATTENDANCE_ORM_H */