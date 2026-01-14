#ifndef LOG_ITEM_STAT_ORM_H
#define LOG_ITEM_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char process_id;
    int item_id;
    long long item_cnt;
} LogItemStat;

/* CRUD Operations */
int LogItemStat_Add(DBConnectionManager* manager, const LogItemStat* record);
int LogItemStat_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id, LogItemStat* record);
int LogItemStat_Update(DBConnectionManager* manager, const LogItemStat* record);
int LogItemStat_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStat_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char process_id, int item_id);
int LogItemStat_GetAll(DBConnectionManager* manager, LogItemStat* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_STAT_ORM_H */