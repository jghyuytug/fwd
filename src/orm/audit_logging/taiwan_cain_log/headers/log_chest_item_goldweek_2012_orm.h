#ifndef LOG_CHEST_ITEM_GOLDWEEK_2012_ORM_H
#define LOG_CHEST_ITEM_GOLDWEEK_2012_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_id;
    int cnt;
} LogChestItemGoldweek2012;

/* CRUD Operations */
int LogChestItemGoldweek2012_Add(DBConnectionManager* manager, const LogChestItemGoldweek2012* record);
int LogChestItemGoldweek2012_Get(DBConnectionManager* manager, int item_id, LogChestItemGoldweek2012* record);
int LogChestItemGoldweek2012_Update(DBConnectionManager* manager, const LogChestItemGoldweek2012* record);
int LogChestItemGoldweek2012_Delete(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2012_Exists(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2012_GetAll(DBConnectionManager* manager, LogChestItemGoldweek2012* records, int max_count, int* actual_count);

#endif /* LOG_CHEST_ITEM_GOLDWEEK_2012_ORM_H */