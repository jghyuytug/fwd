#ifndef DUNGEON_LEV_LOG_ORM_H
#define DUNGEON_LEV_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int dungeon_id;
    int sum_lev;
    int used_coin;
    int try_cnt;
    int clear_cnt;
} DungeonLevLog;

/* CRUD Operations */
int DungeonLevLog_Add(DBConnectionManager* manager, const DungeonLevLog* record);
int DungeonLevLog_Get(DBConnectionManager* manager, const char* occ_date, int dungeon_id, DungeonLevLog* record);
int DungeonLevLog_Update(DBConnectionManager* manager, const DungeonLevLog* record);
int DungeonLevLog_Delete(DBConnectionManager* manager, const char* occ_date, int dungeon_id);
int DungeonLevLog_Exists(DBConnectionManager* manager, const char* occ_date, int dungeon_id);
int DungeonLevLog_GetAll(DBConnectionManager* manager, DungeonLevLog* records, int max_count, int* actual_count);

#endif /* DUNGEON_LEV_LOG_ORM_H */