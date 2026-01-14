#ifndef CLEANUP_DUNGEON_LIST_ORM_H
#define CLEANUP_DUNGEON_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int dungeon_idx;
} CleanupDungeonList;

/* CRUD Operations */
int CleanupDungeonList_Add(DBConnectionManager* manager, const CleanupDungeonList* record);
int CleanupDungeonList_GetAll(DBConnectionManager* manager, CleanupDungeonList* records, int max_count, int* actual_count);

#endif /* CLEANUP_DUNGEON_LIST_ORM_H */