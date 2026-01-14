#ifndef LOG_CUBE_STAT_ORM_H
#define LOG_CUBE_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int level;
    int item_index;
    int type;
    int item_count;
} LogCubeStat;

/* CRUD Operations */
int LogCubeStat_Add(DBConnectionManager* manager, const LogCubeStat* record);
int LogCubeStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, int item_index, int type, LogCubeStat* record);
int LogCubeStat_Update(DBConnectionManager* manager, const LogCubeStat* record);
int LogCubeStat_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, int item_index, int type);
int LogCubeStat_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, int item_index, int type);
int LogCubeStat_GetAll(DBConnectionManager* manager, LogCubeStat* records, int max_count, int* actual_count);

#endif /* LOG_CUBE_STAT_ORM_H */