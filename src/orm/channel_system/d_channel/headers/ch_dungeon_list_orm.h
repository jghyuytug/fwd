#ifndef CH_DUNGEON_LIST_ORM_H
#define CH_DUNGEON_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char kind_name[21];
    short dungeon_id;
} ChDungeonList;

/* CRUD Operations */
int ChDungeonList_Add(DBConnectionManager* manager, const ChDungeonList* record);
int ChDungeonList_GetAll(DBConnectionManager* manager, ChDungeonList* records, int max_count, int* actual_count);

#endif /* CH_DUNGEON_LIST_ORM_H */