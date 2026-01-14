#include "loading_time_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LoadingTime_Add(DBConnectionManager* manager, const LoadingTime* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO loading_time (occ_time, server_id, type, load_sec) "
        "VALUES ('%s', %d, %d, %d)",
        record->occ_time, record->server_id, record->type, record->load_sec);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LoadingTime_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char type, LoadingTime* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, type, load_sec FROM loading_time WHERE occ_time = '%s' AND server_id = %d AND type = %d",
        occ_time, server_id, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LoadingTime));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->type = row[2] ? atoi(row[2]) : 0;
    record->load_sec = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LoadingTime_GetAll(DBConnectionManager* manager, LoadingTime* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, type, load_sec FROM loading_time");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LoadingTime));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].type = row[2] ? atoi(row[2]) : 0;
        records[count].load_sec = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
