#ifndef DNF_DBQ_LOG_ORM_H
#define DNF_DBQ_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char channel_no;
    int dbq;
} DnfDbqLog;

/* CRUD Operations */
int DnfDbqLog_Add(DBConnectionManager* manager, const DnfDbqLog* record);
int DnfDbqLog_Get(DBConnectionManager* manager, const char* occ_time, signed char channel_no, DnfDbqLog* record);
int DnfDbqLog_Update(DBConnectionManager* manager, const DnfDbqLog* record);
int DnfDbqLog_Delete(DBConnectionManager* manager, const char* occ_time, signed char channel_no);
int DnfDbqLog_Exists(DBConnectionManager* manager, const char* occ_time, signed char channel_no);
int DnfDbqLog_GetAll(DBConnectionManager* manager, DnfDbqLog* records, int max_count, int* actual_count);

#endif /* DNF_DBQ_LOG_ORM_H */