#ifndef LOG_QUERY_STAT_DEL_ORM_H
#define LOG_QUERY_STAT_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    char occ_time[20];
    short q_id;
    int gc_no;
    int total;
    int response_time;
} LogQueryStatDel;

/* CRUD Operations */
int LogQueryStatDel_Add(DBConnectionManager* manager, const LogQueryStatDel* record);
int LogQueryStatDel_Get(DBConnectionManager* manager, const char* sdate, const char* occ_time, short q_id, LogQueryStatDel* record);
int LogQueryStatDel_Update(DBConnectionManager* manager, const LogQueryStatDel* record);
int LogQueryStatDel_Delete(DBConnectionManager* manager, const char* sdate, const char* occ_time, short q_id);
int LogQueryStatDel_Exists(DBConnectionManager* manager, const char* sdate, const char* occ_time, short q_id);
int LogQueryStatDel_GetAll(DBConnectionManager* manager, LogQueryStatDel* records, int max_count, int* actual_count);

#endif /* LOG_QUERY_STAT_DEL_ORM_H */