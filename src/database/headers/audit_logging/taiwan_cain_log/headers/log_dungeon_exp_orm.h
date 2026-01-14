#ifndef LOG_DUNGEON_EXP_ORM_H
#define LOG_DUNGEON_EXP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int lev;
    long long exp;
    long long fatigue;
    int user_count;
} LogDungeonExp;

/* CRUD Operations */
int LogDungeonExp_Add(DBConnectionManager* manager, const LogDungeonExp* record);
int LogDungeonExp_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int lev, LogDungeonExp* record);
int LogDungeonExp_Update(DBConnectionManager* manager, const LogDungeonExp* record);
int LogDungeonExp_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int lev);
int LogDungeonExp_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int lev);
int LogDungeonExp_GetAll(DBConnectionManager* manager, LogDungeonExp* records, int max_count, int* actual_count);

#endif /* LOG_DUNGEON_EXP_ORM_H */