#ifndef LOG_ITEM_STAT_MULTIPROC_ORM_H
#define LOG_ITEM_STAT_MULTIPROC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char process_id;
    int item_id;
    long long item_cnt;
} LogItemStatMultiproc;

/* CRUD Operations */
int LogItemStatMultiproc_Add(DBConnectionManager* manager, const LogItemStatMultiproc* record);
int LogItemStatMultiproc_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemStatMultiproc* record);
int LogItemStatMultiproc_Update(DBConnectionManager* manager, const LogItemStatMultiproc* record);
int LogItemStatMultiproc_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatMultiproc_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatMultiproc_GetAll(DBConnectionManager* manager, LogItemStatMultiproc* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_STAT_MULTIPROC_ORM_H */