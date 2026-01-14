#ifndef COLLECT_INTERVAL_ORM_H
#define COLLECT_INTERVAL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char start_time[20];
    char end_time[20];
    short value;
} CollectInterval;

/* CRUD Operations */
int CollectInterval_Add(DBConnectionManager* manager, const CollectInterval* record);
int CollectInterval_GetAll(DBConnectionManager* manager, CollectInterval* records, int max_count, int* actual_count);

#endif /* COLLECT_INTERVAL_ORM_H */