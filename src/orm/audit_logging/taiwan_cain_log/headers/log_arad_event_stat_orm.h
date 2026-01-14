#ifndef LOG_ARAD_EVENT_STAT_ORM_H
#define LOG_ARAD_EVENT_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    int charac_no;
    int item_id;
    int item_cnt;
} LogAradEventStat;

/* CRUD Operations */
int LogAradEventStat_Add(DBConnectionManager* manager, const LogAradEventStat* record);
int LogAradEventStat_Get(DBConnectionManager* manager, const char* occ_time, int m_id, LogAradEventStat* record);
int LogAradEventStat_Update(DBConnectionManager* manager, const LogAradEventStat* record);
int LogAradEventStat_Delete(DBConnectionManager* manager, const char* occ_time, int m_id);
int LogAradEventStat_Exists(DBConnectionManager* manager, const char* occ_time, int m_id);
int LogAradEventStat_GetAll(DBConnectionManager* manager, LogAradEventStat* records, int max_count, int* actual_count);

#endif /* LOG_ARAD_EVENT_STAT_ORM_H */