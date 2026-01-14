#include "geo_country_code_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GeoCountryCode_Add(DBConnectionManager* manager, const GeoCountryCode* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO geo_country_code (code_no, country_code_a2, country_code_a3, country) "
        "VALUES (%d, '%s', '%s', '%s')",
        record->code_no, record->country_code_a2, record->country_code_a3, record->country);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoCountryCode_Get(DBConnectionManager* manager, int code_no, GeoCountryCode* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country FROM geo_country_code WHERE code_no = %d",
        code_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GeoCountryCode));
    record->code_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->country_code_a2, row[1], sizeof(record->country_code_a2) - 1);
    if (row[2]) strncpy(record->country_code_a3, row[2], sizeof(record->country_code_a3) - 1);
    if (row[3]) strncpy(record->country, row[3], sizeof(record->country) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GeoCountryCode_GetAll(DBConnectionManager* manager, GeoCountryCode* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT code_no, country_code_a2, country_code_a3, country FROM geo_country_code");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GeoCountryCode));
        records[count].code_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].country_code_a2, row[1], sizeof(records[count].country_code_a2) - 1);
        if (row[2]) strncpy(records[count].country_code_a3, row[2], sizeof(records[count].country_code_a3) - 1);
        if (row[3]) strncpy(records[count].country, row[3], sizeof(records[count].country) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
