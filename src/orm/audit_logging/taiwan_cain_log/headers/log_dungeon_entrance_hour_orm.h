#ifndef LOG_DUNGEON_ENTRANCE_HOUR_ORM_H
#define LOG_DUNGEON_ENTRANCE_HOUR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[20];
    short channel_no;
    short dungeon_index;
    int hour_enter_count;
    int cnt_enter_count;
} LogDungeonEntranceHour;

/* CRUD Operations */
int LogDungeonEntranceHour_Add(DBConnectionManager* manager, const LogDungeonEntranceHour* record);
int LogDungeonEntranceHour_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index, LogDungeonEntranceHour* record);
int LogDungeonEntranceHour_Update(DBConnectionManager* manager, const LogDungeonEntranceHour* record);
int LogDungeonEntranceHour_Delete(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index);
int LogDungeonEntranceHour_Exists(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index);
int LogDungeonEntranceHour_GetAll(DBConnectionManager* manager, LogDungeonEntranceHour* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_ENTRANCE_HOUR_ORM_H */