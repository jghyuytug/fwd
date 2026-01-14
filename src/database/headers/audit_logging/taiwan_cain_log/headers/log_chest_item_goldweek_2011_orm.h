#ifndef LOG_CHEST_ITEM_GOLDWEEK_2011_ORM_H
#define LOG_CHEST_ITEM_GOLDWEEK_2011_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int item_id;
    int cnt;
} LogChestItemGoldweek2011;

/* CRUD Operations */
int LogChestItemGoldweek2011_Add(DBConnectionManager* manager, const LogChestItemGoldweek2011* record);
int LogChestItemGoldweek2011_Get(DBConnectionManager* manager, int item_id, LogChestItemGoldweek2011* record);
int LogChestItemGoldweek2011_Update(DBConnectionManager* manager, const LogChestItemGoldweek2011* record);
int LogChestItemGoldweek2011_Delete(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2011_Exists(DBConnectionManager* manager, int item_id);
int LogChestItemGoldweek2011_GetAll(DBConnectionManager* manager, LogChestItemGoldweek2011* records, int max_count, int* actual_count);

#endif /* LOG_CHEST_ITEM_GOLDWEEK_2011_ORM_H */