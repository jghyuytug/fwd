#ifndef CH_STATUS_ORM_H
#define CH_STATUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char gc_group;
    signed char gc_status;
} ChStatus;

/* CRUD Operations */
int ChStatus_Add(DBConnectionManager* manager, const ChStatus* record);
int ChStatus_GetAll(DBConnectionManager* manager, ChStatus* records, int max_count, int* actual_count);

#endif /* CH_STATUS_ORM_H */