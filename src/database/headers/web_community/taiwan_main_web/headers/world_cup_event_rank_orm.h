#ifndef WORLD_CUP_EVENT_RANK_ORM_H
#define WORLD_CUP_EVENT_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int cnt;
} WorldCupEventRank;

/* CRUD Operations */
int WorldCupEventRank_Add(DBConnectionManager* manager, const WorldCupEventRank* record);
int WorldCupEventRank_GetAll(DBConnectionManager* manager, WorldCupEventRank* records, int max_count, int* actual_count);

#endif /* WORLD_CUP_EVENT_RANK_ORM_H */