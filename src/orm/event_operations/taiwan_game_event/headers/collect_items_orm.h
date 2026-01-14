#ifndef COLLECT_ITEMS_ORM_H
#define COLLECT_ITEMS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_info;
    int total_count;
    int cur_count;
    signed char change_flag;
    char full_time[20];
} CollectItems;

/* CRUD Operations */
int CollectItems_Add(DBConnectionManager* manager, const CollectItems* record);
int CollectItems_Get(DBConnectionManager* manager, signed char server_info, CollectItems* record);
int CollectItems_Update(DBConnectionManager* manager, const CollectItems* record);
int CollectItems_Delete(DBConnectionManager* manager, signed char server_info);
int CollectItems_Exists(DBConnectionManager* manager, signed char server_info);
int CollectItems_GetAll(DBConnectionManager* manager, CollectItems* records, int max_count, int* actual_count);

#endif /* COLLECT_ITEMS_ORM_H */