#include "spec_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SpecInfo_Add(DBConnectionManager* manager, const SpecInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO spec_info (vendor_id, device_id, vendor_name, device_name) "
        "VALUES (%d, %d, '%s', '%s')",
        record->vendor_id, record->device_id, record->vendor_name, record->device_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SpecInfo_Get(DBConnectionManager* manager, int uid, SpecInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT uid, vendor_id, device_id, vendor_name, device_name FROM spec_info WHERE uid = %d",
        uid);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SpecInfo));
    record->uid = row[0] ? atoi(row[0]) : 0;
    record->vendor_id = row[1] ? atoi(row[1]) : 0;
    record->device_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->vendor_name, row[3], sizeof(record->vendor_name) - 1);
    if (row[4]) strncpy(record->device_name, row[4], sizeof(record->device_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SpecInfo_GetAll(DBConnectionManager* manager, SpecInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT uid, vendor_id, device_id, vendor_name, device_name FROM spec_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SpecInfo));
        records[count].uid = row[0] ? atoi(row[0]) : 0;
        records[count].vendor_id = row[1] ? atoi(row[1]) : 0;
        records[count].device_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].vendor_name, row[3], sizeof(records[count].vendor_name) - 1);
        if (row[4]) strncpy(records[count].device_name, row[4], sizeof(records[count].device_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
