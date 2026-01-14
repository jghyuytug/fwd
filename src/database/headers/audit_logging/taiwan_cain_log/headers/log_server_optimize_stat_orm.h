#ifndef LOG_SERVER_OPTIMIZE_STAT_ORM_H
#define LOG_SERVER_OPTIMIZE_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short channel_no;
    char occ_time[20];
    int total_count;
    int sp_less_5;
    int sp_less_10;
    int sp_less_30;
    int sp_less_50;
    int sp_less_100;
    int sp_more_100;
    float result;
} LogServerOptimizeStat;

/* CRUD Operations */
int LogServerOptimizeStat_Add(DBConnectionManager* manager, const LogServerOptimizeStat* record);
int LogServerOptimizeStat_Get(DBConnectionManager* manager, short channel_no, const char* occ_time, LogServerOptimizeStat* record);
int LogServerOptimizeStat_Update(DBConnectionManager* manager, const LogServerOptimizeStat* record);
int LogServerOptimizeStat_Delete(DBConnectionManager* manager, short channel_no, const char* occ_time);
int LogServerOptimizeStat_Exists(DBConnectionManager* manager, short channel_no, const char* occ_time);
int LogServerOptimizeStat_GetAll(DBConnectionManager* manager, LogServerOptimizeStat* records, int max_count, int* actual_count);

#endif /* LOG_SERVER_OPTIMIZE_STAT_ORM_H */