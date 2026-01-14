#ifndef LOG_ITEM_STAT_SINGLE_ORM_H
#define LOG_ITEM_STAT_SINGLE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char process_id;
    int item_id;
    long long item_cnt;
} LogItemStatSingle;

/* CRUD Operations */
int LogItemStatSingle_Add(DBConnectionManager* manager, const LogItemStatSingle* record);
int LogItemStatSingle_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemStatSingle* record);
int LogItemStatSingle_Update(DBConnectionManager* manager, const LogItemStatSingle* record);
int LogItemStatSingle_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatSingle_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStatSingle_GetAll(DBConnectionManager* manager, LogItemStatSingle* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_STAT_SINGLE_ORM_H */