#include "log_query_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogQueryStat_Add(DBConnectionManager* manager, const LogQueryStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_query_stat (occ_time, q_id, gc_no, total, response_time) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_time, record->q_id, record->gc_no, record->total, record->response_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_Get(DBConnectionManager* manager, const char* occ_time, short q_id, int gc_no, LogQueryStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time FROM log_query_stat WHERE occ_time = '%s' AND q_id = %d AND gc_no = %d",
        occ_time, q_id, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogQueryStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->q_id = row[1] ? atoi(row[1]) : 0;
    record->gc_no = row[2] ? atoi(row[2]) : 0;
    record->total = row[3] ? atoi(row[3]) : 0;
    record->response_time = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStat_GetAll(DBConnectionManager* manager, LogQueryStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, q_id, gc_no, total, response_time FROM log_query_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogQueryStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].q_id = row[1] ? atoi(row[1]) : 0;
        records[count].gc_no = row[2] ? atoi(row[2]) : 0;
        records[count].total = row[3] ? atoi(row[3]) : 0;
        records[count].response_time = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
