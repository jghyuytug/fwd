#include "event_attendance_check_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventAttendanceCheckInfo_Add(DBConnectionManager* manager, const EventAttendanceCheckInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_attendance_check_info (m_id, base_check_date, base_check_cnt, bonus_check_date, bonus_check_cnt) "
        "VALUES (%d, '%s', %d, '%s', %d)",
        record->m_id, record->base_check_date, record->base_check_cnt, record->bonus_check_date, record->bonus_check_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventAttendanceCheckInfo_Get(DBConnectionManager* manager, int m_id, EventAttendanceCheckInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, base_check_date, base_check_cnt, bonus_check_date, bonus_check_cnt FROM event_attendance_check_info WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventAttendanceCheckInfo));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->base_check_date, row[1], sizeof(record->base_check_date) - 1);
    record->base_check_cnt = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->bonus_check_date, row[3], sizeof(record->bonus_check_date) - 1);
    record->bonus_check_cnt = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventAttendanceCheckInfo_GetAll(DBConnectionManager* manager, EventAttendanceCheckInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, base_check_date, base_check_cnt, bonus_check_date, bonus_check_cnt FROM event_attendance_check_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventAttendanceCheckInfo));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].base_check_date, row[1], sizeof(records[count].base_check_date) - 1);
        records[count].base_check_cnt = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].bonus_check_date, row[3], sizeof(records[count].bonus_check_date) - 1);
        records[count].bonus_check_cnt = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
