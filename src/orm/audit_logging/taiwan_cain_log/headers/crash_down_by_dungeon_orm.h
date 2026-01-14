#ifndef CRASH_DOWN_BY_DUNGEON_ORM_H
#define CRASH_DOWN_BY_DUNGEON_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int dungeon_index;
    int down_count;
} CrashDownByDungeon;

/* CRUD Operations */
int CrashDownByDungeon_Add(DBConnectionManager* manager, const CrashDownByDungeon* record);
int CrashDownByDungeon_Get(DBConnectionManager* manager, const char* occ_time, int dungeon_index, CrashDownByDungeon* record);
int CrashDownByDungeon_Update(DBConnectionManager* manager, const CrashDownByDungeon* record);
int CrashDownByDungeon_Delete(DBConnectionManager* manager, const char* occ_time, int dungeon_index);
int CrashDownByDungeon_Exists(DBConnectionManager* manager, const char* occ_time, int dungeon_index);
int CrashDownByDungeon_GetAll(DBConnectionManager* manager, CrashDownByDungeon* records, int max_count, int* actual_count);

#endif /* CRASH_DOWN_BY_DUNGEON_ORM_H */