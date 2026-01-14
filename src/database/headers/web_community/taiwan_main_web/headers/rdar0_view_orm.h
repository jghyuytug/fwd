#ifndef RDAR0_VIEW_ORM_H
#define RDAR0_VIEW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int total_count;
} Rdar0View;

/* CRUD Operations */
int Rdar0View_Add(DBConnectionManager* manager, const Rdar0View* record);
int Rdar0View_GetAll(DBConnectionManager* manager, Rdar0View* records, int max_count, int* actual_count);

#endif /* RDAR0_VIEW_ORM_H */