#include "log_otherserver_load_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogOtherserverLoadStat_Add(DBConnectionManager* manager, const LogOtherserverLoadStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_otherserver_load_stat (occ_time, server_type, kind, q_cnt) "
        "VALUES ('%s', %d, %d, %d)",
        record->occ_time, record->server_type, record->kind, record->q_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogOtherserverLoadStat_Get(DBConnectionManager* manager, const char* occ_time, signed char server_type, signed char kind, LogOtherserverLoadStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_type, kind, q_cnt FROM log_otherserver_load_stat WHERE occ_time = '%s' AND server_type = %d AND kind = %d",
        occ_time, server_type, kind);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogOtherserverLoadStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_type = row[1] ? atoi(row[1]) : 0;
    record->kind = row[2] ? atoi(row[2]) : 0;
    record->q_cnt = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogOtherserverLoadStat_GetAll(DBConnectionManager* manager, LogOtherserverLoadStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_type, kind, q_cnt FROM log_otherserver_load_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogOtherserverLoadStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_type = row[1] ? atoi(row[1]) : 0;
        records[count].kind = row[2] ? atoi(row[2]) : 0;
        records[count].q_cnt = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
