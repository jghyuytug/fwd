#ifndef LOG_CHEST_ITEM_GOLDWEEK_2016_ORM_H
#define LOG_CHEST_ITEM_GOLDWEEK_2016_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_id;
    int cnt;
} LogChestItemGoldweek2016;

/* CRUD Operations */
int LogChestItemGoldweek2016_Add(DBConnectionManager* manager, const LogChestItemGoldweek2016* record);
int LogChestItemGoldweek2016_Get(DBConnectionManager* manager, int item_id, LogChestItemGoldweek2016* record);
int LogChestItemGoldweek2016_Update(DBConnectionManager* manager, const LogChestItemGoldweek2016* record);
int LogChestItemGoldweek2016_Delete(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2016_Exists(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2016_GetAll(DBConnectionManager* manager, LogChestItemGoldweek2016* records, int max_count, int* actual_count);

#endif /* LOG_CHEST_ITEM_GOLDWEEK_2016_ORM_H */