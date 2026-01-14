#ifndef LOG_QUERY_STAT_ORM_H
#define LOG_QUERY_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];           /* YYYY-MM-DD HH:MM:SS */
    unsigned short q_id;
    unsigned int gc_no;
    unsigned int total;
    unsigned int response_time;  /* milliseconds */
} LogQueryStat;

/* CRUD Operations */
int LogQueryStat_Add(DBConnectionManager* manager, const LogQueryStat* stat);
int LogQueryStat_Get(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                      unsigned int gc_no, LogQueryStat* stat);
int LogQueryStat_Update(DBConnectionManager* manager, const LogQueryStat* stat);
int LogQueryStat_Delete(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                         unsigned int gc_no);
int LogQueryStat_Exists(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                         unsigned int gc_no);

/* Specialized Operations */
int LogQueryStat_UpdateTotal(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                              unsigned int gc_no, unsigned int new_total);
int LogQueryStat_UpdateResponseTime(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                                     unsigned int gc_no, unsigned int new_response_time);
int LogQueryStat_IncrementTotal(DBConnectionManager* manager, const char* occ_time, unsigned short q_id,
                                 unsigned int gc_no, unsigned int increment);

/* Query Operations */
int LogQueryStat_GetByQueryId(DBConnectionManager* manager, unsigned short q_id,
                               LogQueryStat* stats, int max_count, int* actual_count);
int LogQueryStat_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                 const char* end_time, LogQueryStat* stats, int max_count,
                                 int* actual_count);
int LogQueryStat_GetByChannel(DBConnectionManager* manager, unsigned int gc_no,
                               LogQueryStat* stats, int max_count, int* actual_count);
int LogQueryStat_CountByQueryId(DBConnectionManager* manager, unsigned short q_id);
int LogQueryStat_CountByTimeRange(DBConnectionManager* manager, const char* start_time,
                                   const char* end_time);
int LogQueryStat_GetAverageResponseTime(DBConnectionManager* manager, unsigned short q_id);
int LogQueryStat_GetMaxResponseTime(DBConnectionManager* manager, unsigned short q_id);

/* Batch Operations */
int LogQueryStat_DeleteByTimeRange(DBConnectionManager* manager, const char* start_time,
                                    const char* end_time);
int LogQueryStat_DeleteOlderThan(DBConnectionManager* manager, int days);

/* Utility Functions */
void LogQueryStat_PrintInfo(const LogQueryStat* stat);

#endif /* LOG_QUERY_STAT_ORM_H */
