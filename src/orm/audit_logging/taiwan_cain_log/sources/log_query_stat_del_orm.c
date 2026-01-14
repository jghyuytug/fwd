#include "log_query_stat_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogQueryStatDel_Add(DBConnectionManager* manager, const LogQueryStatDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_query_stat_del (sdate, occ_time, q_id, gc_no, total, response_time) "
        "VALUES ('%s', '%s', %d, %d, %d, %d)",
        record->sdate, record->occ_time, record->q_id, record->gc_no, record->total, record->response_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStatDel_Get(DBConnectionManager* manager, const char* sdate, const char* occ_time, short q_id, LogQueryStatDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_time, q_id, gc_no, total, response_time FROM log_query_stat_del WHERE sdate = '%s' AND occ_time = '%s' AND q_id = %d",
        sdate, occ_time, q_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogQueryStatDel));
    if (row[0]) strncpy(record->sdate, row[0], sizeof(record->sdate) - 1);
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->q_id = row[2] ? atoi(row[2]) : 0;
    record->gc_no = row[3] ? atoi(row[3]) : 0;
    record->total = row[4] ? atoi(row[4]) : 0;
    record->response_time = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogQueryStatDel_GetAll(DBConnectionManager* manager, LogQueryStatDel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_time, q_id, gc_no, total, response_time FROM log_query_stat_del");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogQueryStatDel));
        if (row[0]) strncpy(records[count].sdate, row[0], sizeof(records[count].sdate) - 1);
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].q_id = row[2] ? atoi(row[2]) : 0;
        records[count].gc_no = row[3] ? atoi(row[3]) : 0;
        records[count].total = row[4] ? atoi(row[4]) : 0;
        records[count].response_time = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
