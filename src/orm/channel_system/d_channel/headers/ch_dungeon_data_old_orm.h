#ifndef CH_DUNGEON_DATA_OLD_ORM_H
#define CH_DUNGEON_DATA_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char kind_name[21];
    char dungeon_name[31];
} ChDungeonDataOld;

/* CRUD Operations */
int ChDungeonDataOld_Add(DBConnectionManager* manager, const ChDungeonDataOld* record);
int ChDungeonDataOld_Get(DBConnectionManager* manager, const char* kind_name, const char* dungeon_name, ChDungeonDataOld* record);
int ChDungeonDataOld_Update(DBConnectionManager* manager, const ChDungeonDataOld* record);
int ChDungeonDataOld_Delete(DBConnectionManager* manager, const char* kind_name, const char* dungeon_name);
int ChDungeonDataOld_Exists(DBConnectionManager* manager, const char* kind_name, const char* dungeon_name);
int ChDungeonDataOld_GetAll(DBConnectionManager* manager, ChDungeonDataOld* records, int max_count, int* actual_count);

#endif /* CH_DUNGEON_DATA_OLD_ORM_H */