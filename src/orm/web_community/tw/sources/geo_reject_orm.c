#include "geo_reject_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GeoReject_Add(DBConnectionManager* manager, const GeoReject* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO geo_reject (rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src) "
        "VALUES ('%s', '%s', %d, '%s', '%s', %d)",
        record->rej_ip, record->rej_c_code, record->rej_ip_count, record->rej_last_date, record->rej_chk, record->rej_src);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GeoReject_Get(DBConnectionManager* manager, const char* rej_ip, GeoReject* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src FROM geo_reject WHERE rej_ip = '%s'",
        rej_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GeoReject));
    if (row[0]) strncpy(record->rej_ip, row[0], sizeof(record->rej_ip) - 1);
    if (row[1]) strncpy(record->rej_c_code, row[1], sizeof(record->rej_c_code) - 1);
    record->rej_ip_count = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->rej_last_date, row[3], sizeof(record->rej_last_date) - 1);
    if (row[4]) strncpy(record->rej_chk, row[4], sizeof(record->rej_chk) - 1);
    record->rej_src = row[5] ? row[5][0] : '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int GeoReject_GetAll(DBConnectionManager* manager, GeoReject* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rej_ip, rej_c_code, rej_ip_count, rej_last_date, rej_chk, rej_src FROM geo_reject");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GeoReject));
        if (row[0]) strncpy(records[count].rej_ip, row[0], sizeof(records[count].rej_ip) - 1);
        if (row[1]) strncpy(records[count].rej_c_code, row[1], sizeof(records[count].rej_c_code) - 1);
        records[count].rej_ip_count = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].rej_last_date, row[3], sizeof(records[count].rej_last_date) - 1);
        if (row[4]) strncpy(records[count].rej_chk, row[4], sizeof(records[count].rej_chk) - 1);
        records[count].rej_src = row[5] ? row[5][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
