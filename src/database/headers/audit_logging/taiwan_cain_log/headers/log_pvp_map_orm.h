#ifndef LOG_PVP_MAP_ORM_H
#define LOG_PVP_MAP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    char occ_date[11];
    int map_id;
    int play_count;
} LogPvpMap;

/* CRUD Operations */
int LogPvpMap_Add(DBConnectionManager* manager, const LogPvpMap* record);
int LogPvpMap_Get(DBConnectionManager* manager, int channel_no, int map_id, const char* occ_date, LogPvpMap* record);
int LogPvpMap_Update(DBConnectionManager* manager, const LogPvpMap* record);
int LogPvpMap_Delete(DBConnectionManager* manager, int channel_no, int map_id, const char* occ_date);
int LogPvpMap_Exists(DBConnectionManager* manager, int channel_no, int map_id, const char* occ_date);
int LogPvpMap_GetAll(DBConnectionManager* manager, LogPvpMap* records, int max_count, int* actual_count);

#endif /* LOG_PVP_MAP_ORM_H */