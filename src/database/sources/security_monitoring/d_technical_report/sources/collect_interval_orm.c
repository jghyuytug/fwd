#include "collect_interval_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CollectInterval_Add(DBConnectionManager* manager, const CollectInterval* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO collect_interval (start_time, end_time, value) "
        "VALUES ('%s', '%s', %d)",
        record->start_time, record->end_time, record->value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CollectInterval_GetAll(DBConnectionManager* manager, CollectInterval* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT start_time, end_time, value FROM collect_interval");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CollectInterval));
        if (row[0]) strncpy(records[count].start_time, row[0], sizeof(records[count].start_time) - 1);
        if (row[1]) strncpy(records[count].end_time, row[1], sizeof(records[count].end_time) - 1);
        records[count].value = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
