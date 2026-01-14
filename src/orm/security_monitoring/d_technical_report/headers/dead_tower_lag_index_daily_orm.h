#ifndef DEAD_TOWER_LAG_INDEX_DAILY_ORM_H
#define DEAD_TOWER_LAG_INDEX_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int spec_id;
    int win_fps;
    int win_fps_cnt;
    int full_fps;
    int full_fps_cnt;
    int full_win_fps;
    int full_win_fps_cnt;
    int full_win_nosync_fps;
    int full_win_nosync_fps_cnt;
} DeadTowerLagIndexDaily;

/* CRUD Operations */
int DeadTowerLagIndexDaily_Add(DBConnectionManager* manager, const DeadTowerLagIndexDaily* record);
int DeadTowerLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, DeadTowerLagIndexDaily* record);
int DeadTowerLagIndexDaily_Update(DBConnectionManager* manager, const DeadTowerLagIndexDaily* record);
int DeadTowerLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int DeadTowerLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int DeadTowerLagIndexDaily_GetAll(DBConnectionManager* manager, DeadTowerLagIndexDaily* records, int max_count, int* actual_count);

#endif /* DEAD_TOWER_LAG_INDEX_DAILY_ORM_H */