#include "event_ontime_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventOntimeInfo_Add(DBConnectionManager* manager, const EventOntimeInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_ontime_info (no, item_index, item_count, time) "
        "VALUES (%d, %d, %d, '%s')",
        record->no, record->item_index, record->item_count, record->time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventOntimeInfo_Get(DBConnectionManager* manager, int no, EventOntimeInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_index, item_count, time FROM event_ontime_info WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventOntimeInfo));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->item_index = row[1] ? atoi(row[1]) : 0;
    record->item_count = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->time, row[3], sizeof(record->time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventOntimeInfo_GetAll(DBConnectionManager* manager, EventOntimeInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, item_index, item_count, time FROM event_ontime_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventOntimeInfo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].item_index = row[1] ? atoi(row[1]) : 0;
        records[count].item_count = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].time, row[3], sizeof(records[count].time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
