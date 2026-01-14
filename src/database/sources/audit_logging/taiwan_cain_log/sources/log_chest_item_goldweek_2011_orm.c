#include "log_chest_item_goldweek_2011_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogChestItemGoldweek2011_Add(DBConnectionManager* manager, const LogChestItemGoldweek2011* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_chest_item_goldweek_2011 (item_id, cnt) "
        "VALUES (%d, %d)",
        record->item_id, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogChestItemGoldweek2011_Get(DBConnectionManager* manager, int item_id, LogChestItemGoldweek2011* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_id, cnt FROM log_chest_item_goldweek_2011 WHERE item_id = %d",
        item_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogChestItemGoldweek2011));
    record->item_id = row[0] ? atoi(row[0]) : 0;
    record->cnt = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogChestItemGoldweek2011_GetAll(DBConnectionManager* manager, LogChestItemGoldweek2011* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_id, cnt FROM log_chest_item_goldweek_2011");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogChestItemGoldweek2011));
        records[count].item_id = row[0] ? atoi(row[0]) : 0;
        records[count].cnt = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
