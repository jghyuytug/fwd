#ifndef MAX_COUNT_ORM_H
#define MAX_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_info;
    int mc_max;
    char mc_date[20];
} MaxCount;

/* CRUD Operations */
int MaxCount_Add(DBConnectionManager* manager, const MaxCount* record);
int MaxCount_GetAll(DBConnectionManager* manager, MaxCount* records, int max_count, int* actual_count);

#endif /* MAX_COUNT_ORM_H */