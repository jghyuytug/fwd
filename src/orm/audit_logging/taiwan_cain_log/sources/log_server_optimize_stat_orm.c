#include "log_server_optimize_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogServerOptimizeStat_Add(DBConnectionManager* manager, const LogServerOptimizeStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_server_optimize_stat (channel_no, occ_time, total_count, sp_less_5, sp_less_10, sp_less_30, sp_less_50, sp_less_100, sp_more_100, result) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %f)",
        record->channel_no, record->occ_time, record->total_count, record->sp_less_5, record->sp_less_10, record->sp_less_30, record->sp_less_50, record->sp_less_100, record->sp_more_100, record->result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogServerOptimizeStat_Get(DBConnectionManager* manager, short channel_no, const char* occ_time, LogServerOptimizeStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_time, total_count, sp_less_5, sp_less_10, sp_less_30, sp_less_50, sp_less_100, sp_more_100, result FROM log_server_optimize_stat WHERE channel_no = %d AND occ_time = '%s'",
        channel_no, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogServerOptimizeStat));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->total_count = row[2] ? atoi(row[2]) : 0;
    record->sp_less_5 = row[3] ? atoi(row[3]) : 0;
    record->sp_less_10 = row[4] ? atoi(row[4]) : 0;
    record->sp_less_30 = row[5] ? atoi(row[5]) : 0;
    record->sp_less_50 = row[6] ? atoi(row[6]) : 0;
    record->sp_less_100 = row[7] ? atoi(row[7]) : 0;
    record->sp_more_100 = row[8] ? atoi(row[8]) : 0;
    record->result = row[9] ? atof(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogServerOptimizeStat_GetAll(DBConnectionManager* manager, LogServerOptimizeStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_time, total_count, sp_less_5, sp_less_10, sp_less_30, sp_less_50, sp_less_100, sp_more_100, result FROM log_server_optimize_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogServerOptimizeStat));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].total_count = row[2] ? atoi(row[2]) : 0;
        records[count].sp_less_5 = row[3] ? atoi(row[3]) : 0;
        records[count].sp_less_10 = row[4] ? atoi(row[4]) : 0;
        records[count].sp_less_30 = row[5] ? atoi(row[5]) : 0;
        records[count].sp_less_50 = row[6] ? atoi(row[6]) : 0;
        records[count].sp_less_100 = row[7] ? atoi(row[7]) : 0;
        records[count].sp_more_100 = row[8] ? atoi(row[8]) : 0;
        records[count].result = row[9] ? atof(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
