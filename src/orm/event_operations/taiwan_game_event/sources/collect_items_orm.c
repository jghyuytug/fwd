#include "collect_items_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CollectItems_Add(DBConnectionManager* manager, const CollectItems* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO collect_items (server_info, total_count, cur_count, change_flag, full_time) "
        "VALUES (%d, %d, %d, %d, '%s')",
        record->server_info, record->total_count, record->cur_count, record->change_flag, record->full_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CollectItems_Get(DBConnectionManager* manager, signed char server_info, CollectItems* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, total_count, cur_count, change_flag, full_time FROM collect_items WHERE server_info = %d",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CollectItems));
    record->server_info = row[0] ? atoi(row[0]) : 0;
    record->total_count = row[1] ? atoi(row[1]) : 0;
    record->cur_count = row[2] ? atoi(row[2]) : 0;
    record->change_flag = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->full_time, row[4], sizeof(record->full_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CollectItems_GetAll(DBConnectionManager* manager, CollectItems* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, total_count, cur_count, change_flag, full_time FROM collect_items");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CollectItems));
        records[count].server_info = row[0] ? atoi(row[0]) : 0;
        records[count].total_count = row[1] ? atoi(row[1]) : 0;
        records[count].cur_count = row[2] ? atoi(row[2]) : 0;
        records[count].change_flag = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].full_time, row[4], sizeof(records[count].full_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
