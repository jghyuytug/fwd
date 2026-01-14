#include "event_webmoneystamp_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventWebmoneystampEntry_Add(DBConnectionManager* manager, const EventWebmoneystampEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_webmoneystamp_entry (m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item) "
        "VALUES (%d, '%s', %d, '%s', %d, %d)",
        record->m_id, record->occ_time, record->attend_point, record->last_attend_time, record->return_flag, record->entry_item);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_Get(DBConnectionManager* manager, int m_id, EventWebmoneystampEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item FROM event_webmoneystamp_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventWebmoneystampEntry));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->attend_point = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->last_attend_time, row[3], sizeof(record->last_attend_time) - 1);
    record->return_flag = row[4] ? atoi(row[4]) : 0;
    record->entry_item = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_GetAll(DBConnectionManager* manager, EventWebmoneystampEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item FROM event_webmoneystamp_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventWebmoneystampEntry));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].attend_point = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].last_attend_time, row[3], sizeof(records[count].last_attend_time) - 1);
        records[count].return_flag = row[4] ? atoi(row[4]) : 0;
        records[count].entry_item = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
