#ifndef DUNGEON_LAG_INDEX_ORM_H
#define DUNGEON_LAG_INDEX_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int spec_id;
    char occ_time[20];
    signed char server_group;
    short min_fps;
    short avg_fps;
    short max_fps;
    short win_fps;
    short full_fps;
    short full_win_fps;
    short full_win_nosync_fps;
    int frame1;
    float time1;
    int frame2;
    float time2;
    int frame3;
    float time3;
    int frame4;
    float time4;
    int frame5;
    float time5;
    int frame6;
    float time6;
    int share_rate;
} DungeonLagIndex;

/* CRUD Operations */
int DungeonLagIndex_Add(DBConnectionManager* manager, const DungeonLagIndex* record);
int DungeonLagIndex_GetAll(DBConnectionManager* manager, DungeonLagIndex* records, int max_count, int* actual_count);

#endif /* DUNGEON_LAG_INDEX_ORM_H */