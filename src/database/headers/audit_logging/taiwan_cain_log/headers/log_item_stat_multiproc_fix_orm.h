#ifndef LOG_ITEM_STAT_MULTIPROC_FIX_ORM_H
#define LOG_ITEM_STAT_MULTIPROC_FIX_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char process_id;
    int item_id;
    long long item_cnt;
} LogItemStatMultiprocFix;

/* CRUD Operations */
int LogItemStatMultiprocFix_Add(DBConnectionManager* manager, const LogItemStatMultiprocFix* record);
int LogItemStatMultiprocFix_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemStatMultiprocFix* record);
int LogItemStatMultiprocFix_Update(DBConnectionManager* manager, const LogItemStatMultiprocFix* record);
int LogItemStatMultiprocFix_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatMultiprocFix_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatMultiprocFix_GetAll(DBConnectionManager* manager, LogItemStatMultiprocFix* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_STAT_MULTIPROC_FIX_ORM_H */