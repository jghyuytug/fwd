#ifndef D_DATE_ORM_H
#define D_DATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char aa[11];
} DDate;

/* CRUD Operations */
int DDate_Add(DBConnectionManager* manager, const DDate* record);
int DDate_GetAll(DBConnectionManager* manager, DDate* records, int max_count, int* actual_count);

#endif /* D_DATE_ORM_H */