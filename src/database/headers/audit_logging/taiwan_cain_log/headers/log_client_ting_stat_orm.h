#ifndef LOG_CLIENT_TING_STAT_ORM_H
#define LOG_CLIENT_TING_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int reason;
    int cnt;
} LogClientTingStat;

/* CRUD Operations */
int LogClientTingStat_Add(DBConnectionManager* manager, const LogClientTingStat* record);
int LogClientTingStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason, LogClientTingStat* record);
int LogClientTingStat_Update(DBConnectionManager* manager, const LogClientTingStat* record);
int LogClientTingStat_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason);
int LogClientTingStat_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason);
int LogClientTingStat_GetAll(DBConnectionManager* manager, LogClientTingStat* records, int max_count, int* actual_count);

#endif /* LOG_CLIENT_TING_STAT_ORM_H */