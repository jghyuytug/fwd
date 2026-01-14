#ifndef COMMON_INDEX_DAILY_ORM_H
#define COMMON_INDEX_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char server_group;
    int spec_id;
    int share_rate;
    int crash_village;
    int crash_dungeon;
    int crash_challenge;
    int crash_wararea;
    int crash_fight_village;
    int crash_dead_tower;
} CommonIndexDaily;

/* CRUD Operations */
int CommonIndexDaily_Add(DBConnectionManager* manager, const CommonIndexDaily* record);
int CommonIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, signed char server_group, int spec_id, CommonIndexDaily* record);
int CommonIndexDaily_Update(DBConnectionManager* manager, const CommonIndexDaily* record);
int CommonIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, signed char server_group, int spec_id);
int CommonIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, signed char server_group, int spec_id);
int CommonIndexDaily_GetAll(DBConnectionManager* manager, CommonIndexDaily* records, int max_count, int* actual_count);

#endif /* COMMON_INDEX_DAILY_ORM_H */