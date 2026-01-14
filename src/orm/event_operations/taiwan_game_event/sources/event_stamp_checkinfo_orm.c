#include "event_stamp_checkinfo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventStampCheckinfo_Add(DBConnectionManager* manager, const EventStampCheckinfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_stamp_checkinfo (charac_no, current, stamp_checkinfo) "
        "VALUES (%d, %d, '%s')",
        record->charac_no, record->current, record->stamp_checkinfo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampCheckinfo_Get(DBConnectionManager* manager, int charac_no, EventStampCheckinfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, current, stamp_checkinfo FROM event_stamp_checkinfo WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventStampCheckinfo));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->current = row[1] ? atoi(row[1]) : 0;
    if (row[2]) memcpy(record->stamp_checkinfo, row[2], sizeof(record->stamp_checkinfo));

    DBQueryResult_Free(&result);
    return 0;
}

int EventStampCheckinfo_GetAll(DBConnectionManager* manager, EventStampCheckinfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, current, stamp_checkinfo FROM event_stamp_checkinfo");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventStampCheckinfo));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].current = row[1] ? atoi(row[1]) : 0;
        if (row[2]) memcpy(records[count].stamp_checkinfo, row[2], sizeof(records[count].stamp_checkinfo));
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

