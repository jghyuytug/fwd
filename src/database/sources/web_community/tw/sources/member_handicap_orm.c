#include "member_handicap_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberHandicap_Add(DBConnectionManager* manager, const MemberHandicap* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_handicap (event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d)",
        record->event_id, record->cap_type, record->server_id, record->m_id, record->start_time, record->end_time, record->handicap_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberHandicap_Get(DBConnectionManager* manager, int event_id, signed char cap_type, signed char server_id, int m_id, const char* start_time, MemberHandicap* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value FROM member_handicap WHERE event_id = %d AND cap_type = %d AND server_id = %d AND m_id = %d AND start_time = '%s'",
        event_id, cap_type, server_id, m_id, start_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberHandicap));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->cap_type = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->start_time, row[4], sizeof(record->start_time) - 1);
    if (row[5]) strncpy(record->end_time, row[5], sizeof(record->end_time) - 1);
    record->handicap_value = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberHandicap_GetAll(DBConnectionManager* manager, MemberHandicap* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, cap_type, server_id, m_id, start_time, end_time, handicap_value FROM member_handicap");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberHandicap));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].cap_type = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].start_time, row[4], sizeof(records[count].start_time) - 1);
        if (row[5]) strncpy(records[count].end_time, row[5], sizeof(records[count].end_time) - 1);
        records[count].handicap_value = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
