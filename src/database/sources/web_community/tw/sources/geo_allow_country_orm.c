#include "geo_allow_country_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GeoAllowCountry_Add(DBConnectionManager* manager, const GeoAllowCountry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO geo_allow_country (server_group, country_code, reg_date) "
        "VALUES (%d, '%s', '%s')",
        record->server_group, record->country_code, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllowCountry_Get(DBConnectionManager* manager, const char* country_code, signed char server_group, GeoAllowCountry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date FROM geo_allow_country WHERE country_code = '%s' AND server_group = %d",
        country_code, server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GeoAllowCountry));
    record->server_group = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->country_code, row[1], sizeof(record->country_code) - 1);
    if (row[2]) strncpy(record->reg_date, row[2], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GeoAllowCountry_GetAll(DBConnectionManager* manager, GeoAllowCountry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_group, country_code, reg_date FROM geo_allow_country");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GeoAllowCountry));
        records[count].server_group = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].country_code, row[1], sizeof(records[count].country_code) - 1);
        if (row[2]) strncpy(records[count].reg_date, row[2], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
