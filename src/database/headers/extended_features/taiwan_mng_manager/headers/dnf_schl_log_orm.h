#ifndef DNF_SCHL_LOG_ORM_H
#define DNF_SCHL_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int schl_log_id;
    char occ_time[20];
    signed char server_group;
    char schl_cd[5];
    char end_date[20];
    char st_date[20];
    char stat[5];
    char description[256];
} DnfSchlLog;

/* CRUD Operations */
int DnfSchlLog_Add(DBConnectionManager* manager, const DnfSchlLog* record);
int DnfSchlLog_Get(DBConnectionManager* manager, int schl_log_id, DnfSchlLog* record);
int DnfSchlLog_Update(DBConnectionManager* manager, const DnfSchlLog* record);
int DnfSchlLog_Delete(DBConnectionManager* manager, int schl_log_id);
int DnfSchlLog_Exists(DBConnectionManager* manager, int schl_log_id);
int DnfSchlLog_GetAll(DBConnectionManager* manager, DnfSchlLog* records, int max_count, int* actual_count);

#endif /* DNF_SCHL_LOG_ORM_H */