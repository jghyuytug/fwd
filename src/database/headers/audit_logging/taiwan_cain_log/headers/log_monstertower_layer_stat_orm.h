#ifndef LOG_MONSTERTOWER_LAYER_STAT_ORM_H
#define LOG_MONSTERTOWER_LAYER_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int key_id;
    short layer;
    signed char monster_diff;
    signed char charac_job;
    signed char charac_grow;
    signed char level;
    int play_cnt;
    int success_cnt;
    long long play_time;
    int uniq_cnt;
} LogMonstertowerLayerStat;

/* CRUD Operations */
int LogMonstertowerLayerStat_Add(DBConnectionManager* manager, const LogMonstertowerLayerStat* record);
int LogMonstertowerLayerStat_Get(DBConnectionManager* manager, const char* occ_date, int key_id, LogMonstertowerLayerStat* record);
int LogMonstertowerLayerStat_Update(DBConnectionManager* manager, const LogMonstertowerLayerStat* record);
int LogMonstertowerLayerStat_Delete(DBConnectionManager* manager, const char* occ_date, int key_id);
int LogMonstertowerLayerStat_Exists(DBConnectionManager* manager, const char* occ_date, int key_id);
int LogMonstertowerLayerStat_GetAll(DBConnectionManager* manager, LogMonstertowerLayerStat* records, int max_count, int* actual_count);

#endif /* LOG_MONSTERTOWER_LAYER_STAT_ORM_H */