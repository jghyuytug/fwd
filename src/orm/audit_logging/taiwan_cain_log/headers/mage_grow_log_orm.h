#ifndef MAGE_GROW_LOG_ORM_H
#define MAGE_GROW_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int grow_type1;
    int grow_type2;
    int grow_type3;
    int grow_type4;
    int grow_type5;
} MageGrowLog;

/* CRUD Operations */
int MageGrowLog_Add(DBConnectionManager* manager, const MageGrowLog* record);
int MageGrowLog_GetAll(DBConnectionManager* manager, MageGrowLog* records, int max_count, int* actual_count);

#endif /* MAGE_GROW_LOG_ORM_H */