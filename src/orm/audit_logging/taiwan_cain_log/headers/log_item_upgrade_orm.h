#ifndef LOG_ITEM_UPGRADE_ORM_H
#define LOG_ITEM_UPGRADE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char process_id;
    int item_id;
    long long item_cnt;
    int upgrade;
    long long amplify_item_cnt;
    int rarity;
    int level;
} LogItemUpgrade;

/* CRUD Operations */
int LogItemUpgrade_Add(DBConnectionManager* manager, const LogItemUpgrade* record);
int LogItemUpgrade_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemUpgrade* record);
int LogItemUpgrade_Update(DBConnectionManager* manager, const LogItemUpgrade* record);
int LogItemUpgrade_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemUpgrade_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemUpgrade_GetAll(DBConnectionManager* manager, LogItemUpgrade* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_UPGRADE_ORM_H */