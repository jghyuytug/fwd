#ifndef COMMON_INDEX_ORM_H
#define COMMON_INDEX_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int spec_id;
    char occ_time[20];
    signed char server_group;
    int share_rate;
    short crash_count;
    short village_to_dungeon_lag;
    short dungeon_to_village_lag;
    short crash_village;
    short crash_dungeon;
    short crash_challenge;
    short crash_wararea;
    short crash_fight_village;
    short crash_dead_tower;
    short crash_channel;
    short crash_chaos;
    short crash_load;
} CommonIndex;

/* CRUD Operations */
int CommonIndex_Add(DBConnectionManager* manager, const CommonIndex* record);
int CommonIndex_GetAll(DBConnectionManager* manager, CommonIndex* records, int max_count, int* actual_count);

#endif /* COMMON_INDEX_ORM_H */