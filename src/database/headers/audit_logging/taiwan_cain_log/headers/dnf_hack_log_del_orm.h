#ifndef DNF_HACK_LOG_DEL_ORM_H
#define DNF_HACK_LOG_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    int m_id;
    int charac_no;
    short hack_type;
    char last_time[20];
    int etc;
    int cnt;
    char occ_date[11];
} DnfHackLogDel;

/* CRUD Operations */
int DnfHackLogDel_Add(DBConnectionManager* manager, const DnfHackLogDel* record);
int DnfHackLogDel_Get(DBConnectionManager* manager, const char* sdate, const char* occ_date, int m_id, short hack_type, DnfHackLogDel* record);
int DnfHackLogDel_Update(DBConnectionManager* manager, const DnfHackLogDel* record);
int DnfHackLogDel_Delete(DBConnectionManager* manager, const char* sdate, const char* occ_date, int m_id, short hack_type);
int DnfHackLogDel_Exists(DBConnectionManager* manager, const char* sdate, const char* occ_date, int m_id, short hack_type);
int DnfHackLogDel_GetAll(DBConnectionManager* manager, DnfHackLogDel* records, int max_count, int* actual_count);

#endif /* DNF_HACK_LOG_DEL_ORM_H */