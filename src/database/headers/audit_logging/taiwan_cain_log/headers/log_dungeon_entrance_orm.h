#ifndef LOG_DUNGEON_ENTRANCE_ORM_H
#define LOG_DUNGEON_ENTRANCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    short channel_no;
    short dungeon_index;
    int enter_count;
    int exercise_count;
} LogDungeonEntrance;

/* CRUD Operations */
int LogDungeonEntrance_Add(DBConnectionManager* manager, const LogDungeonEntrance* record);
int LogDungeonEntrance_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index, LogDungeonEntrance* record);
int LogDungeonEntrance_Update(DBConnectionManager* manager, const LogDungeonEntrance* record);
int LogDungeonEntrance_Delete(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index);
int LogDungeonEntrance_Exists(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index);
int LogDungeonEntrance_GetAll(DBConnectionManager* manager, LogDungeonEntrance* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_ENTRANCE_ORM_H */