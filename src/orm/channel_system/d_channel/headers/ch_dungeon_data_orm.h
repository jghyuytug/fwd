#ifndef CH_DUNGEON_DATA_ORM_H
#define CH_DUNGEON_DATA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char kind_name[21];
    char dungeon_name[31];
} ChDungeonData;

/* CRUD Operations */
int ChDungeonData_Add(DBConnectionManager* manager, const ChDungeonData* record);
int ChDungeonData_GetAll(DBConnectionManager* manager, ChDungeonData* records, int max_count, int* actual_count);

#endif /* CH_DUNGEON_DATA_ORM_H */