#ifndef LOG_ITEM_UPGRADE_MULTIPROC_ORM_H
#define LOG_ITEM_UPGRADE_MULTIPROC_ORM_H

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
} LogItemUpgradeMultiproc;

/* CRUD Operations */
int LogItemUpgradeMultiproc_Add(DBConnectionManager* manager, const LogItemUpgradeMultiproc* record);
int LogItemUpgradeMultiproc_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemUpgradeMultiproc* record);
int LogItemUpgradeMultiproc_Update(DBConnectionManager* manager, const LogItemUpgradeMultiproc* record);
int LogItemUpgradeMultiproc_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemUpgradeMultiproc_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemUpgradeMultiproc_GetAll(DBConnectionManager* manager, LogItemUpgradeMultiproc* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_UPGRADE_MULTIPROC_ORM_H */