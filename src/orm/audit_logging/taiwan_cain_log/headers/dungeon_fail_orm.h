#ifndef DUNGEON_FAIL_ORM_H
#define DUNGEON_FAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char lev;
    int occ_count;
} DungeonFail;

/* CRUD Operations */
int DungeonFail_Add(DBConnectionManager* manager, const DungeonFail* record);
int DungeonFail_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, DungeonFail* record);
int DungeonFail_Update(DBConnectionManager* manager, const DungeonFail* record);
int DungeonFail_Delete(DBConnectionManager* manager, const char* occ_date, signed char lev);
int DungeonFail_Exists(DBConnectionManager* manager, const char* occ_date, signed char lev);
int DungeonFail_GetAll(DBConnectionManager* manager, DungeonFail* records, int max_count, int* actual_count);

#endif /* DUNGEON_FAIL_ORM_H */