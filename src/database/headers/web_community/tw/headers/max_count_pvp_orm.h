#ifndef MAX_COUNT_PVP_ORM_H
#define MAX_COUNT_PVP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_info;
    int mc_max;
    char mc_date[20];
} MaxCountPvp;

/* CRUD Operations */
int MaxCountPvp_Add(DBConnectionManager* manager, const MaxCountPvp* record);
int MaxCountPvp_GetAll(DBConnectionManager* manager, MaxCountPvp* records, int max_count, int* actual_count);

#endif /* MAX_COUNT_PVP_ORM_H */