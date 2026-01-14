#ifndef LOG_LEVEL_DUNGEON_PLAY_COUNT_ORM_H
#define LOG_LEVEL_DUNGEON_PLAY_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short level;
    int total_charac_count;
    long long total_dungeon_play_count;
} LogLevelDungeonPlayCount;

/* CRUD Operations */
int LogLevelDungeonPlayCount_Add(DBConnectionManager* manager, const LogLevelDungeonPlayCount* record);
int LogLevelDungeonPlayCount_Get(DBConnectionManager* manager, short level, LogLevelDungeonPlayCount* record);
int LogLevelDungeonPlayCount_Update(DBConnectionManager* manager, const LogLevelDungeonPlayCount* record);
int LogLevelDungeonPlayCount_Delete(DBConnectionManager* manager, short level);
int LogLevelDungeonPlayCount_Exists(DBConnectionManager* manager, short level);
int LogLevelDungeonPlayCount_GetAll(DBConnectionManager* manager, LogLevelDungeonPlayCount* records, int max_count, int* actual_count);

#endif /* LOG_LEVEL_DUNGEON_PLAY_COUNT_ORM_H */