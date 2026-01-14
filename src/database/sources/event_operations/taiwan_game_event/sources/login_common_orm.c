#include "login_common_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LoginCommon_Add(DBConnectionManager* manager, const LoginCommon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO login_common (m_id, update_time, member_bonus_fatigue, radio_flag, daily_point, acc_point, random_option_guide, event_charac_cnt, login_ip, inform_notice) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s')",
        record->m_id, record->update_time, record->member_bonus_fatigue, record->radio_flag, record->daily_point, record->acc_point, record->random_option_guide, record->event_charac_cnt, record->login_ip, record->inform_notice);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LoginCommon_Get(DBConnectionManager* manager, int m_id, LoginCommon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, update_time, member_bonus_fatigue, radio_flag, daily_point, acc_point, random_option_guide, event_charac_cnt, login_ip, inform_notice FROM login_common WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LoginCommon));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->update_time = row[1] ? atoi(row[1]) : 0;
    record->member_bonus_fatigue = row[2] ? atoi(row[2]) : 0;
    record->radio_flag = row[3] ? atoi(row[3]) : 0;
    record->daily_point = row[4] ? atoi(row[4]) : 0;
    record->acc_point = row[5] ? atoi(row[5]) : 0;
    record->random_option_guide = row[6] ? atoi(row[6]) : 0;
    record->event_charac_cnt = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->login_ip, row[8], sizeof(record->login_ip) - 1);
    if (row[9]) strncpy(record->inform_notice, row[9], sizeof(record->inform_notice) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LoginCommon_GetAll(DBConnectionManager* manager, LoginCommon* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, update_time, member_bonus_fatigue, radio_flag, daily_point, acc_point, random_option_guide, event_charac_cnt, login_ip, inform_notice FROM login_common");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LoginCommon));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].update_time = row[1] ? atoi(row[1]) : 0;
        records[count].member_bonus_fatigue = row[2] ? atoi(row[2]) : 0;
        records[count].radio_flag = row[3] ? atoi(row[3]) : 0;
        records[count].daily_point = row[4] ? atoi(row[4]) : 0;
        records[count].acc_point = row[5] ? atoi(row[5]) : 0;
        records[count].random_option_guide = row[6] ? atoi(row[6]) : 0;
        records[count].event_charac_cnt = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].login_ip, row[8], sizeof(records[count].login_ip) - 1);
        if (row[9]) strncpy(records[count].inform_notice, row[9], sizeof(records[count].inform_notice) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
