#ifndef LOG_GAMESERVER_LOAD_STAT_ORM_H
#define LOG_GAMESERVER_LOAD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_id;
    int channel_no;
    int dbq_cnt;
    int logdbq_cnt;
    int netq_cnt;
    int packetq_cnt;
} LogGameserverLoadStat;

/* CRUD Operations */
int LogGameserverLoadStat_Add(DBConnectionManager* manager, const LogGameserverLoadStat* record);
int LogGameserverLoadStat_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, int channel_no, LogGameserverLoadStat* record);
int LogGameserverLoadStat_Update(DBConnectionManager* manager, const LogGameserverLoadStat* record);
int LogGameserverLoadStat_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_id, int channel_no);
int LogGameserverLoadStat_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_id, int channel_no);
int LogGameserverLoadStat_GetAll(DBConnectionManager* manager, LogGameserverLoadStat* records, int max_count, int* actual_count);

#endif /* LOG_GAMESERVER_LOAD_STAT_ORM_H */