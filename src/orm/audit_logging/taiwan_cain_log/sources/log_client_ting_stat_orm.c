#include "log_client_ting_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogClientTingStat_Add(DBConnectionManager* manager, const LogClientTingStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_client_ting_stat (occ_time, channel_no, reason, cnt) "
        "VALUES ('%s', %d, %d, %d)",
        record->occ_time, record->channel_no, record->reason, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogClientTingStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int reason, LogClientTingStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, reason, cnt FROM log_client_ting_stat WHERE occ_time = '%s' AND channel_no = %d AND reason = %d",
        occ_time, channel_no, reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogClientTingStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->reason = row[2] ? atoi(row[2]) : 0;
    record->cnt = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogClientTingStat_GetAll(DBConnectionManager* manager, LogClientTingStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, reason, cnt FROM log_client_ting_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogClientTingStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].reason = row[2] ? atoi(row[2]) : 0;
        records[count].cnt = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
