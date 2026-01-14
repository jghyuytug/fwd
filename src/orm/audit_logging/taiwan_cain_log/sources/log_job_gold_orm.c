#include "log_job_gold_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogJobGold_Add(DBConnectionManager* manager, const LogJobGold* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_job_gold (cur_date, server_info, job, gold_average, gold_max, gold_sum) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->cur_date, record->server_info, record->job, record->gold_average, record->gold_max, record->gold_sum);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogJobGold_Get(DBConnectionManager* manager, const char* cur_date, signed char server_info, signed char job, LogJobGold* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, job, gold_average, gold_max, gold_sum FROM log_job_gold WHERE cur_date = '%s' AND server_info = %d AND job = %d",
        cur_date, server_info, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogJobGold));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->job = row[2] ? atoi(row[2]) : 0;
    record->gold_average = row[3] ? atoi(row[3]) : 0;
    record->gold_max = row[4] ? atoi(row[4]) : 0;
    record->gold_sum = row[5] ? atoll(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogJobGold_GetAll(DBConnectionManager* manager, LogJobGold* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, server_info, job, gold_average, gold_max, gold_sum FROM log_job_gold");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogJobGold));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].job = row[2] ? atoi(row[2]) : 0;
        records[count].gold_average = row[3] ? atoi(row[3]) : 0;
        records[count].gold_max = row[4] ? atoi(row[4]) : 0;
        records[count].gold_sum = row[5] ? atoll(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
