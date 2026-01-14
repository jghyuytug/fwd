#ifndef DNF_HACK_LOG_ORM_H
#define DNF_HACK_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    short hack_type;
    char last_time[20];
    int etc;
    int cnt;
    char occ_date[11];
} DnfHackLog;

/* CRUD Operations */
int DnfHackLog_Add(DBConnectionManager* manager, const DnfHackLog* record);
int DnfHackLog_Get(DBConnectionManager* manager, int m_id, short hack_type, const char* occ_date, DnfHackLog* record);
int DnfHackLog_Update(DBConnectionManager* manager, const DnfHackLog* record);
int DnfHackLog_Delete(DBConnectionManager* manager, int m_id, short hack_type, const char* occ_date);
int DnfHackLog_Exists(DBConnectionManager* manager, int m_id, short hack_type, const char* occ_date);
int DnfHackLog_GetAll(DBConnectionManager* manager, DnfHackLog* records, int max_count, int* actual_count);

#endif /* DNF_HACK_LOG_ORM_H */