#ifndef LOG_CHEST_ITEM_GOLDWEEK_2013_ORM_H
#define LOG_CHEST_ITEM_GOLDWEEK_2013_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_id;
    int cnt;
} LogChestItemGoldweek2013;

/* CRUD Operations */
int LogChestItemGoldweek2013_Add(DBConnectionManager* manager, const LogChestItemGoldweek2013* record);
int LogChestItemGoldweek2013_Get(DBConnectionManager* manager, int item_id, LogChestItemGoldweek2013* record);
int LogChestItemGoldweek2013_Update(DBConnectionManager* manager, const LogChestItemGoldweek2013* record);
int LogChestItemGoldweek2013_Delete(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2013_Exists(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2013_GetAll(DBConnectionManager* manager, LogChestItemGoldweek2013* records, int max_count, int* actual_count);

#endif /* LOG_CHEST_ITEM_GOLDWEEK_2013_ORM_H */