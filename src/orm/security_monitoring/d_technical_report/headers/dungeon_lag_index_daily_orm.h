#ifndef DUNGEON_LAG_INDEX_DAILY_ORM_H
#define DUNGEON_LAG_INDEX_DAILY_ORM_H

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
} DungeonLagIndexDaily;

/* CRUD Operations */
int DungeonLagIndexDaily_Add(DBConnectionManager* manager, const DungeonLagIndexDaily* record);
int DungeonLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, DungeonLagIndexDaily* record);
int DungeonLagIndexDaily_Update(DBConnectionManager* manager, const DungeonLagIndexDaily* record);
int DungeonLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int DungeonLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int DungeonLagIndexDaily_GetAll(DBConnectionManager* manager, DungeonLagIndexDaily* records, int max_count, int* actual_count);

#endif /* DUNGEON_LAG_INDEX_DAILY_ORM_H */