#include "dnf_event_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfEventInfo_Add(DBConnectionManager* manager, const DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_event_info (event_id, event_name, event_explain, apply_type, start_date, end_date) "
        "VALUES (%d, '%s', '%s', %d, '%s', '%s')",
        record->event_id, record->event_name, record->event_explain, record->apply_type, record->start_date, record->end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_Get(DBConnectionManager* manager, int event_id, DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date FROM dnf_event_info WHERE event_id = %d",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfEventInfo));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->event_name, row[1], sizeof(record->event_name) - 1);
    if (row[2]) strncpy(record->event_explain, row[2], sizeof(record->event_explain) - 1);
    record->apply_type = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->start_date, row[4], sizeof(record->start_date) - 1);
    if (row[5]) strncpy(record->end_date, row[5], sizeof(record->end_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_GetAll(DBConnectionManager* manager, DnfEventInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date FROM dnf_event_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfEventInfo));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].event_name, row[1], sizeof(records[count].event_name) - 1);
        if (row[2]) strncpy(records[count].event_explain, row[2], sizeof(records[count].event_explain) - 1);
        records[count].apply_type = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].start_date, row[4], sizeof(records[count].start_date) - 1);
        if (row[5]) strncpy(records[count].end_date, row[5], sizeof(records[count].end_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
