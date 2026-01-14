#include "member_join_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberJoinInfo_Add(DBConnectionManager* manager, const MemberJoinInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_join_info (m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history) "
        "VALUES (%d, %d, '%s', %d, %d, %d, '%s', %d)",
        record->m_id, record->reg_date, record->ip, record->contry_code, record->login_time, record->error_type, record->login_ip, record->game_use_history);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberJoinInfo_Get(DBConnectionManager* manager, int m_id, MemberJoinInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history FROM member_join_info WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberJoinInfo));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->reg_date = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ip, row[2], sizeof(record->ip) - 1);
    record->contry_code = row[3] ? atoi(row[3]) : 0;
    record->login_time = row[4] ? atoi(row[4]) : 0;
    record->error_type = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->login_ip, row[6], sizeof(record->login_ip) - 1);
    record->game_use_history = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberJoinInfo_GetAll(DBConnectionManager* manager, MemberJoinInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history FROM member_join_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberJoinInfo));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].reg_date = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ip, row[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = row[3] ? atoi(row[3]) : 0;
        records[count].login_time = row[4] ? atoi(row[4]) : 0;
        records[count].error_type = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].login_ip, row[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
