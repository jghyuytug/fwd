#include "lag_stat_module_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LagStatModule_Add(DBConnectionManager* manager, const LagStatModule* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO lag_stat_module (occ_time, server_id, module, average, deviation, count) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_time, record->server_id, record->module, record->average, record->deviation, record->count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LagStatModule_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char module, LagStatModule* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, module, average, deviation, count FROM lag_stat_module WHERE occ_time = '%s' AND server_id = %d AND module = %d",
        occ_time, server_id, module);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LagStatModule));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->module = row[2] ? atoi(row[2]) : 0;
    record->average = row[3] ? atoi(row[3]) : 0;
    record->deviation = row[4] ? atoi(row[4]) : 0;
    record->count = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LagStatModule_GetAll(DBConnectionManager* manager, LagStatModule* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, module, average, deviation, count FROM lag_stat_module");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LagStatModule));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].module = row[2] ? atoi(row[2]) : 0;
        records[count].average = row[3] ? atoi(row[3]) : 0;
        records[count].deviation = row[4] ? atoi(row[4]) : 0;
        records[count].count = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
