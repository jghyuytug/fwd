#ifndef LAG_STAT_DUNGEON_ORM_H
#define LAG_STAT_DUNGEON_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_id;
    int dungeon_idx;
    int first_average;
    int first_deviation;
    int first_count;
    int boss_average;
    int boss_deviation;
    int boss_count;
} LagStatDungeon;

/* CRUD Operations */
int LagStatDungeon_Add(DBConnectionManager* manager, const LagStatDungeon* record);
int LagStatDungeon_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, int dungeon_idx, LagStatDungeon* record);
int LagStatDungeon_Update(DBConnectionManager* manager, const LagStatDungeon* record);
int LagStatDungeon_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_id, int dungeon_idx);
int LagStatDungeon_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_id, int dungeon_idx);
int LagStatDungeon_GetAll(DBConnectionManager* manager, LagStatDungeon* records, int max_count, int* actual_count);

#endif /* LAG_STAT_DUNGEON_ORM_H */