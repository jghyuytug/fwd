#include "event_stamp_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventStampDaily_Add(DBConnectionManager* manager, const EventStampDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_stamp_daily (charac_no, condition1, condition2, condition3) "
        "VALUES (%d, %d, %d, %d)",
        record->charac_no, record->condition1, record->condition2, record->condition3);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampDaily_Get(DBConnectionManager* manager, int charac_no, EventStampDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, condition1, condition2, condition3 FROM event_stamp_daily WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventStampDaily));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->condition1 = row[1] ? atoi(row[1]) : 0;
    record->condition2 = row[2] ? atoi(row[2]) : 0;
    record->condition3 = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampDaily_GetAll(DBConnectionManager* manager, EventStampDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, condition1, condition2, condition3 FROM event_stamp_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventStampDaily));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].condition1 = row[1] ? atoi(row[1]) : 0;
        records[count].condition2 = row[2] ? atoi(row[2]) : 0;
        records[count].condition3 = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
