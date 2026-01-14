#include "directx_version_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DirectxVersion_Add(DBConnectionManager* manager, const DirectxVersion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO directx_version (occ_time, server_group, ver_etc, ver_8_x, ver_9_0, ver_9_0_a, ver_9_0_b, ver_9_0_c, ver_10_x, ver_11_x) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->server_group, record->ver_etc, record->ver_8_x, record->ver_9_0, record->ver_9_0_a, record->ver_9_0_b, record->ver_9_0_c, record->ver_10_x, record->ver_11_x);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DirectxVersion_Get(DBConnectionManager* manager, const char* occ_time, signed char server_group, DirectxVersion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_group, ver_etc, ver_8_x, ver_9_0, ver_9_0_a, ver_9_0_b, ver_9_0_c, ver_10_x, ver_11_x FROM directx_version WHERE occ_time = '%s' AND server_group = %d",
        occ_time, server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DirectxVersion));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_group = row[1] ? atoi(row[1]) : 0;
    record->ver_etc = row[2] ? atoi(row[2]) : 0;
    record->ver_8_x = row[3] ? atoi(row[3]) : 0;
    record->ver_9_0 = row[4] ? atoi(row[4]) : 0;
    record->ver_9_0_a = row[5] ? atoi(row[5]) : 0;
    record->ver_9_0_b = row[6] ? atoi(row[6]) : 0;
    record->ver_9_0_c = row[7] ? atoi(row[7]) : 0;
    record->ver_10_x = row[8] ? atoi(row[8]) : 0;
    record->ver_11_x = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DirectxVersion_GetAll(DBConnectionManager* manager, DirectxVersion* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_group, ver_etc, ver_8_x, ver_9_0, ver_9_0_a, ver_9_0_b, ver_9_0_c, ver_10_x, ver_11_x FROM directx_version");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DirectxVersion));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[1] ? atoi(row[1]) : 0;
        records[count].ver_etc = row[2] ? atoi(row[2]) : 0;
        records[count].ver_8_x = row[3] ? atoi(row[3]) : 0;
        records[count].ver_9_0 = row[4] ? atoi(row[4]) : 0;
        records[count].ver_9_0_a = row[5] ? atoi(row[5]) : 0;
        records[count].ver_9_0_b = row[6] ? atoi(row[6]) : 0;
        records[count].ver_9_0_c = row[7] ? atoi(row[7]) : 0;
        records[count].ver_10_x = row[8] ? atoi(row[8]) : 0;
        records[count].ver_11_x = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
