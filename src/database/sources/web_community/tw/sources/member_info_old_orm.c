#include "member_info_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberInfoOld_Add(DBConnectionManager* manager, const MemberInfoOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_info_old (user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, ssn_check, last_play_time) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', %d, '%s', '%s', %d, '%s', %d, %d, '%s')",
        record->user_id, record->user_name, record->first_ssn, record->second_ssn, record->passwd, record->mobile_no, record->reg_date, record->email, record->q_no, record->q_answer, record->updt_date, record->state, record->nickname, record->email_yn, record->ssn_check, record->last_play_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoOld_Get(DBConnectionManager* manager, int m_id, MemberInfoOld* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, ssn_check, last_play_time FROM member_info_old WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberInfoOld));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->user_id, row[1], sizeof(record->user_id) - 1);
    if (row[2]) strncpy(record->user_name, row[2], sizeof(record->user_name) - 1);
    if (row[3]) strncpy(record->first_ssn, row[3], sizeof(record->first_ssn) - 1);
    if (row[4]) strncpy(record->second_ssn, row[4], sizeof(record->second_ssn) - 1);
    if (row[5]) strncpy(record->passwd, row[5], sizeof(record->passwd) - 1);
    if (row[6]) strncpy(record->mobile_no, row[6], sizeof(record->mobile_no) - 1);
    record->reg_date = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->email, row[8], sizeof(record->email) - 1);
    record->q_no = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->q_answer, row[10], sizeof(record->q_answer) - 1);
    if (row[11]) strncpy(record->updt_date, row[11], sizeof(record->updt_date) - 1);
    record->state = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->nickname, row[13], sizeof(record->nickname) - 1);
    record->email_yn = row[14] ? row[14][0] : '\0';
    record->ssn_check = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->last_play_time, row[16], sizeof(record->last_play_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoOld_GetAll(DBConnectionManager* manager, MemberInfoOld* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, ssn_check, last_play_time FROM member_info_old");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberInfoOld));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].user_id, row[1], sizeof(records[count].user_id) - 1);
        if (row[2]) strncpy(records[count].user_name, row[2], sizeof(records[count].user_name) - 1);
        if (row[3]) strncpy(records[count].first_ssn, row[3], sizeof(records[count].first_ssn) - 1);
        if (row[4]) strncpy(records[count].second_ssn, row[4], sizeof(records[count].second_ssn) - 1);
        if (row[5]) strncpy(records[count].passwd, row[5], sizeof(records[count].passwd) - 1);
        if (row[6]) strncpy(records[count].mobile_no, row[6], sizeof(records[count].mobile_no) - 1);
        records[count].reg_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].email, row[8], sizeof(records[count].email) - 1);
        records[count].q_no = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].q_answer, row[10], sizeof(records[count].q_answer) - 1);
        if (row[11]) strncpy(records[count].updt_date, row[11], sizeof(records[count].updt_date) - 1);
        records[count].state = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].nickname, row[13], sizeof(records[count].nickname) - 1);
        records[count].email_yn = row[14] ? row[14][0] : '\0';
        records[count].ssn_check = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].last_play_time, row[16], sizeof(records[count].last_play_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
