#ifndef CH_DUNGEON_LIST_OLD_ORM_H
#define CH_DUNGEON_LIST_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char kind_name[21];
    short dungeon_id;
} ChDungeonListOld;

/* CRUD Operations */
int ChDungeonListOld_Add(DBConnectionManager* manager, const ChDungeonListOld* record);
int ChDungeonListOld_Get(DBConnectionManager* manager, short dungeon_id, ChDungeonListOld* record);
int ChDungeonListOld_Update(DBConnectionManager* manager, const ChDungeonListOld* record);
int ChDungeonListOld_Delete(DBConnectionManager* manager, short dungeon_id);
int ChDungeonListOld_Exists(DBConnectionManager* manager, short dungeon_id);
int ChDungeonListOld_GetAll(DBConnectionManager* manager, ChDungeonListOld* records, int max_count, int* actual_count);

#endif /* CH_DUNGEON_LIST_OLD_ORM_H */