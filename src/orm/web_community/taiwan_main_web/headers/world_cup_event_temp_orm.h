#ifndef WORLD_CUP_EVENT_TEMP_ORM_H
#define WORLD_CUP_EVENT_TEMP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int cnt;
} WorldCupEventTemp;

/* CRUD Operations */
int WorldCupEventTemp_Add(DBConnectionManager* manager, const WorldCupEventTemp* record);
int WorldCupEventTemp_GetAll(DBConnectionManager* manager, WorldCupEventTemp* records, int max_count, int* actual_count);

#endif /* WORLD_CUP_EVENT_TEMP_ORM_H */