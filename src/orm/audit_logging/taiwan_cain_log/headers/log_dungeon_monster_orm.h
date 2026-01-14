#ifndef LOG_DUNGEON_MONSTER_ORM_H
#define LOG_DUNGEON_MONSTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    int dungeon_index;
    int dungeon_diff;
    int monster_index;
    int damage_total;
    int damage_count;
    char last_time[20];
} LogDungeonMonster;

/* CRUD Operations */
int LogDungeonMonster_Add(DBConnectionManager* manager, const LogDungeonMonster* record);
int LogDungeonMonster_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int monster_index, LogDungeonMonster* record);
int LogDungeonMonster_Update(DBConnectionManager* manager, const LogDungeonMonster* record);
int LogDungeonMonster_Delete(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int monster_index);
int LogDungeonMonster_Exists(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int monster_index);
int LogDungeonMonster_GetAll(DBConnectionManager* manager, LogDungeonMonster* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_MONSTER_ORM_H */