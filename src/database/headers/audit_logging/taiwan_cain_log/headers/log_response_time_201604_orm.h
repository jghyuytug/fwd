#ifndef LOG_RESPONSE_TIME_201604_ORM_H
#define LOG_RESPONSE_TIME_201604_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int channel_no;
    int packet_id;
    int packet_count;
    long long total_response_time;
    int avg_response_time;
} LogResponseTime201604;

/* CRUD Operations */
int LogResponseTime201604_Add(DBConnectionManager* manager, const LogResponseTime201604* record);
int LogResponseTime201604_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int packet_id, LogResponseTime201604* record);
int LogResponseTime201604_Update(DBConnectionManager* manager, const LogResponseTime201604* record);
int LogResponseTime201604_Delete(DBConnectionManager* manager, const char* occ_time, int channel_no, int packet_id);
int LogResponseTime201604_Exists(DBConnectionManager* manager, const char* occ_time, int channel_no, int packet_id);
int LogResponseTime201604_GetAll(DBConnectionManager* manager, LogResponseTime201604* records, int max_count, int* actual_count);

#endif /* LOG_RESPONSE_TIME_201604_ORM_H */