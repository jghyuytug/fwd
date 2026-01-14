#ifndef LOG_GAMESERVER_LOAD_STAT_DEL_ORM_H
#define LOG_GAMESERVER_LOAD_STAT_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    char occ_time[20];
    signed char server_id;
    int channel_no;
    int dbq_cnt;
    int logdbq_cnt;
    int netq_cnt;
    int packetq_cnt;
} LogGameserverLoadStatDel;

/* CRUD Operations */
int LogGameserverLoadStatDel_Add(DBConnectionManager* manager, const LogGameserverLoadStatDel* record);
int LogGameserverLoadStatDel_GetAll(DBConnectionManager* manager, LogGameserverLoadStatDel* records, int max_count, int* actual_count);

#endif /* LOG_GAMESERVER_LOAD_STAT_DEL_ORM_H */