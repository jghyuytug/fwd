#include "member_broadcast_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberBroadcast_Add(DBConnectionManager* manager, const MemberBroadcast* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_broadcast (event_id, m_id, server_id, charac_no, charac_name, start_time, end_time) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', '%s')",
        record->event_id, record->m_id, record->server_id, record->charac_no, record->charac_name, record->start_time, record->end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBroadcast_Get(DBConnectionManager* manager, int event_id, int m_id, signed char server_id, int charac_no, const char* start_time, MemberBroadcast* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time FROM member_broadcast WHERE event_id = %d AND m_id = %d AND server_id = %d AND charac_no = %d AND start_time = '%s'",
        event_id, m_id, server_id, charac_no, start_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberBroadcast));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->start_time, row[5], sizeof(record->start_time) - 1);
    if (row[6]) strncpy(record->end_time, row[6], sizeof(record->end_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBroadcast_GetAll(DBConnectionManager* manager, MemberBroadcast* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, server_id, charac_no, charac_name, start_time, end_time FROM member_broadcast");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberBroadcast));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].start_time, row[5], sizeof(records[count].start_time) - 1);
        if (row[6]) strncpy(records[count].end_time, row[6], sizeof(records[count].end_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
