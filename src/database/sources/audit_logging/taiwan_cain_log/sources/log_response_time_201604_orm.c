#include "log_response_time_201604_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogResponseTime201604_Add(DBConnectionManager* manager, const LogResponseTime201604* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_response_time_201604 (occ_time, channel_no, packet_id, packet_count, total_response_time, avg_response_time) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_time, record->channel_no, record->packet_id, record->packet_count, record->total_response_time, record->avg_response_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogResponseTime201604_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int packet_id, LogResponseTime201604* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, packet_id, packet_count, total_response_time, avg_response_time FROM log_response_time_201604 WHERE occ_time = '%s' AND channel_no = %d AND packet_id = %d",
        occ_time, channel_no, packet_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogResponseTime201604));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->packet_id = row[2] ? atoi(row[2]) : 0;
    record->packet_count = row[3] ? atoi(row[3]) : 0;
    record->total_response_time = row[4] ? atoll(row[4]) : 0;
    record->avg_response_time = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogResponseTime201604_GetAll(DBConnectionManager* manager, LogResponseTime201604* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, packet_id, packet_count, total_response_time, avg_response_time FROM log_response_time_201604");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogResponseTime201604));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].packet_id = row[2] ? atoi(row[2]) : 0;
        records[count].packet_count = row[3] ? atoi(row[3]) : 0;
        records[count].total_response_time = row[4] ? atoll(row[4]) : 0;
        records[count].avg_response_time = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
