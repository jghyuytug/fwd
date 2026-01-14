#ifndef LOG_OTHERSERVER_LOAD_STAT_ORM_H
#define LOG_OTHERSERVER_LOAD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_type;
    signed char kind;
    int q_cnt;
} LogOtherserverLoadStat;

/* CRUD Operations */
int LogOtherserverLoadStat_Add(DBConnectionManager* manager, const LogOtherserverLoadStat* record);
int LogOtherserverLoadStat_Get(DBConnectionManager* manager, const char* occ_time, signed char server_type, signed char kind, LogOtherserverLoadStat* record);
int LogOtherserverLoadStat_Update(DBConnectionManager* manager, const LogOtherserverLoadStat* record);
int LogOtherserverLoadStat_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_type, signed char kind);
int LogOtherserverLoadStat_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_type, signed char kind);
int LogOtherserverLoadStat_GetAll(DBConnectionManager* manager, LogOtherserverLoadStat* records, int max_count, int* actual_count);

#endif /* LOG_OTHERSERVER_LOAD_STAT_ORM_H */