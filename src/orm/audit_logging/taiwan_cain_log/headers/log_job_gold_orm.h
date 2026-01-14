#ifndef LOG_JOB_GOLD_ORM_H
#define LOG_JOB_GOLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    signed char server_info;
    signed char job;
    int gold_average;
    int gold_max;
    long long gold_sum;
} LogJobGold;

/* CRUD Operations */
int LogJobGold_Add(DBConnectionManager* manager, const LogJobGold* record);
int LogJobGold_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char job, LogJobGold* record);
int LogJobGold_Update(DBConnectionManager* manager, const LogJobGold* record);
int LogJobGold_Delete(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char job);
int LogJobGold_Exists(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char job);
int LogJobGold_GetAll(DBConnectionManager* manager, LogJobGold* records, int max_count, int* actual_count);

#endif /* LOG_JOB_GOLD_ORM_H */