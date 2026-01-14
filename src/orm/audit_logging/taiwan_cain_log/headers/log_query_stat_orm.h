#ifndef LOG_QUERY_STAT_ORM_H
#define LOG_QUERY_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    short q_id;
    int gc_no;
    int total;
    int response_time;
} LogQueryStat;

/* CRUD Operations */
int LogQueryStat_Add(DBConnectionManager* manager, const LogQueryStat* record);
int LogQueryStat_Get(DBConnectionManager* manager, const char* occ_time, short q_id, int gc_no, LogQueryStat* record);
int LogQueryStat_Update(DBConnectionManager* manager, const LogQueryStat* record);
int LogQueryStat_Delete(DBConnectionManager* manager, const char* occ_time, short q_id, int gc_no);
int LogQueryStat_Exists(DBConnectionManager* manager, const char* occ_time, short q_id, int gc_no);
int LogQueryStat_GetAll(DBConnectionManager* manager, LogQueryStat* records, int max_count, int* actual_count);

#endif /* LOG_QUERY_STAT_ORM_H */