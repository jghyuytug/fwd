#include "geo_allow_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GeoAllow_Add(DBConnectionManager* manager, const GeoAllow* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO geo_allow (allow_ip, allow_c_code, allow_date) "
        "VALUES ('%s', '%s', '%s')",
        record->allow_ip, record->allow_c_code, record->allow_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllow_Get(DBConnectionManager* manager, const char* allow_ip, GeoAllow* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date FROM geo_allow WHERE allow_ip = '%s'",
        allow_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GeoAllow));
    if (row[0]) strncpy(record->allow_ip, row[0], sizeof(record->allow_ip) - 1);
    if (row[1]) strncpy(record->allow_c_code, row[1], sizeof(record->allow_c_code) - 1);
    if (row[2]) strncpy(record->allow_date, row[2], sizeof(record->allow_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllow_GetAll(DBConnectionManager* manager, GeoAllow* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT allow_ip, allow_c_code, allow_date FROM geo_allow");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GeoAllow));
        if (row[0]) strncpy(records[count].allow_ip, row[0], sizeof(records[count].allow_ip) - 1);
        if (row[1]) strncpy(records[count].allow_c_code, row[1], sizeof(records[count].allow_c_code) - 1);
        if (row[2]) strncpy(records[count].allow_date, row[2], sizeof(records[count].allow_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
