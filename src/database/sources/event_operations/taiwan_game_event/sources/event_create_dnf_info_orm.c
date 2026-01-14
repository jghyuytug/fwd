#include "event_create_dnf_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventCreateDnfInfo_Add(DBConnectionManager* manager, const EventCreateDnfInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_create_dnf_info (event_day, occ_time, rate) "
        "VALUES (%d, '%s', %d)",
        record->event_day, record->occ_time, record->rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventCreateDnfInfo_Get(DBConnectionManager* manager, const char* occ_time, EventCreateDnfInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_day, occ_time, rate FROM event_create_dnf_info WHERE occ_time = '%s'",
        occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventCreateDnfInfo));
    record->event_day = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->rate = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCreateDnfInfo_GetAll(DBConnectionManager* manager, EventCreateDnfInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_day, occ_time, rate FROM event_create_dnf_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventCreateDnfInfo));
        records[count].event_day = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].rate = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
