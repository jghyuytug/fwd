#include "member_info_old_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new member (m_id is auto_increment, don't specify it) */
int MemberInfoOld_Add(DBConnectionManager* manager, const MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_info_old (user_id, user_name, first_ssn, second_ssn, "
        "passwd, mobile_no, reg_date, email, q_no, q_answer, state, nickname, "
        "email_yn, ssn_check, last_play_time) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %u, '%s', %u, '%s', %d, '%s', "
        "'%c', %u, '%s')",
        member->user_id, member->user_name, member->first_ssn, member->second_ssn,
        member->passwd, member->mobile_no, member->reg_date, member->email,
        member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->last_play_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by m_id */
int MemberInfoOld_Get(DBConnectionManager* manager, unsigned int m_id, MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, last_play_time "
        "FROM member_info_old WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoOld));
    member->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(member->user_id, row[1], sizeof(member->user_id) - 1);
    if (row[2]) strncpy(member->user_name, row[2], sizeof(member->user_name) - 1);
    if (row[3]) strncpy(member->first_ssn, row[3], sizeof(member->first_ssn) - 1);
    if (row[4]) strncpy(member->second_ssn, row[4], sizeof(member->second_ssn) - 1);
    if (row[5]) strncpy(member->passwd, row[5], sizeof(member->passwd) - 1);
    if (row[6]) strncpy(member->mobile_no, row[6], sizeof(member->mobile_no) - 1);
    member->reg_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(member->email, row[8], sizeof(member->email) - 1);
    member->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(member->q_answer, row[10], sizeof(member->q_answer) - 1);
    if (row[11]) strncpy(member->updt_date, row[11], sizeof(member->updt_date) - 1);
    member->state = row[12] ? (char)atoi(row[12]) : 0;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    if (row[16]) strncpy(member->last_play_time, row[16], sizeof(member->last_play_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update member */
int MemberInfoOld_Update(DBConnectionManager* manager, const MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_old SET user_id = '%s', user_name = '%s', "
        "first_ssn = '%s', second_ssn = '%s', passwd = '%s', mobile_no = '%s', "
        "reg_date = %u, email = '%s', q_no = %u, q_answer = '%s', state = %d, "
        "nickname = '%s', email_yn = '%c', ssn_check = %u, last_play_time = '%s' "
        "WHERE m_id = %u",
        member->user_id, member->user_name, member->first_ssn, member->second_ssn,
        member->passwd, member->mobile_no, member->reg_date, member->email,
        member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->last_play_time, member->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete member */
int MemberInfoOld_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_old WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if member exists */
int MemberInfoOld_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_info_old WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get member by user_id */
int MemberInfoOld_GetByUserId(DBConnectionManager* manager, const char* user_id,
                               MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !user_id || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, last_play_time "
        "FROM member_info_old WHERE user_id = '%s'", user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoOld));
    member->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(member->user_id, row[1], sizeof(member->user_id) - 1);
    if (row[2]) strncpy(member->user_name, row[2], sizeof(member->user_name) - 1);
    if (row[3]) strncpy(member->first_ssn, row[3], sizeof(member->first_ssn) - 1);
    if (row[4]) strncpy(member->second_ssn, row[4], sizeof(member->second_ssn) - 1);
    if (row[5]) strncpy(member->passwd, row[5], sizeof(member->passwd) - 1);
    if (row[6]) strncpy(member->mobile_no, row[6], sizeof(member->mobile_no) - 1);
    member->reg_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(member->email, row[8], sizeof(member->email) - 1);
    member->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(member->q_answer, row[10], sizeof(member->q_answer) - 1);
    if (row[11]) strncpy(member->updt_date, row[11], sizeof(member->updt_date) - 1);
    member->state = row[12] ? (char)atoi(row[12]) : 0;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    if (row[16]) strncpy(member->last_play_time, row[16], sizeof(member->last_play_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by nickname */
int MemberInfoOld_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                 MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !nickname || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, last_play_time "
        "FROM member_info_old WHERE nickname = '%s'", nickname);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoOld));
    member->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(member->user_id, row[1], sizeof(member->user_id) - 1);
    if (row[2]) strncpy(member->user_name, row[2], sizeof(member->user_name) - 1);
    if (row[3]) strncpy(member->first_ssn, row[3], sizeof(member->first_ssn) - 1);
    if (row[4]) strncpy(member->second_ssn, row[4], sizeof(member->second_ssn) - 1);
    if (row[5]) strncpy(member->passwd, row[5], sizeof(member->passwd) - 1);
    if (row[6]) strncpy(member->mobile_no, row[6], sizeof(member->mobile_no) - 1);
    member->reg_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(member->email, row[8], sizeof(member->email) - 1);
    member->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(member->q_answer, row[10], sizeof(member->q_answer) - 1);
    if (row[11]) strncpy(member->updt_date, row[11], sizeof(member->updt_date) - 1);
    member->state = row[12] ? (char)atoi(row[12]) : 0;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    if (row[16]) strncpy(member->last_play_time, row[16], sizeof(member->last_play_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by SSN */
int MemberInfoOld_GetBySSN(DBConnectionManager* manager, const char* first_ssn,
                            const char* second_ssn, MemberInfoOld* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !first_ssn || !second_ssn || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, last_play_time "
        "FROM member_info_old WHERE first_ssn = '%s' AND second_ssn = '%s'",
        first_ssn, second_ssn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoOld));
    member->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(member->user_id, row[1], sizeof(member->user_id) - 1);
    if (row[2]) strncpy(member->user_name, row[2], sizeof(member->user_name) - 1);
    if (row[3]) strncpy(member->first_ssn, row[3], sizeof(member->first_ssn) - 1);
    if (row[4]) strncpy(member->second_ssn, row[4], sizeof(member->second_ssn) - 1);
    if (row[5]) strncpy(member->passwd, row[5], sizeof(member->passwd) - 1);
    if (row[6]) strncpy(member->mobile_no, row[6], sizeof(member->mobile_no) - 1);
    member->reg_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(member->email, row[8], sizeof(member->email) - 1);
    member->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(member->q_answer, row[10], sizeof(member->q_answer) - 1);
    if (row[11]) strncpy(member->updt_date, row[11], sizeof(member->updt_date) - 1);
    member->state = row[12] ? (char)atoi(row[12]) : 0;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    if (row[16]) strncpy(member->last_play_time, row[16], sizeof(member->last_play_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Get members by state */
int MemberInfoOld_GetByState(DBConnectionManager* manager, char state,
                              MemberInfoOld* members, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !members || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, last_play_time "
        "FROM member_info_old WHERE state = %d ORDER BY m_id", state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(MemberInfoOld));
        members[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(members[count].user_id, row[1], sizeof(members[count].user_id) - 1);
        if (row[2]) strncpy(members[count].user_name, row[2], sizeof(members[count].user_name) - 1);
        if (row[3]) strncpy(members[count].first_ssn, row[3], sizeof(members[count].first_ssn) - 1);
        if (row[4]) strncpy(members[count].second_ssn, row[4], sizeof(members[count].second_ssn) - 1);
        if (row[5]) strncpy(members[count].passwd, row[5], sizeof(members[count].passwd) - 1);
        if (row[6]) strncpy(members[count].mobile_no, row[6], sizeof(members[count].mobile_no) - 1);
        members[count].reg_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(members[count].email, row[8], sizeof(members[count].email) - 1);
        members[count].q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(members[count].q_answer, row[10], sizeof(members[count].q_answer) - 1);
        if (row[11]) strncpy(members[count].updt_date, row[11], sizeof(members[count].updt_date) - 1);
        members[count].state = row[12] ? (char)atoi(row[12]) : 0;
        if (row[13]) strncpy(members[count].nickname, row[13], sizeof(members[count].nickname) - 1);
        members[count].email_yn = row[14] ? row[14][0] : 'y';
        members[count].ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
        if (row[16]) strncpy(members[count].last_play_time, row[16], sizeof(members[count].last_play_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Update password */
int MemberInfoOld_UpdatePassword(DBConnectionManager* manager, unsigned int m_id,
                                  const char* new_passwd) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_passwd) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_old SET passwd = '%s' WHERE m_id = %u",
        new_passwd, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update last play time */
int MemberInfoOld_UpdateLastPlayTime(DBConnectionManager* manager, unsigned int m_id,
                                      const char* play_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !play_time) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_old SET last_play_time = '%s' WHERE m_id = %u",
        play_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print member information */
void MemberInfoOld_PrintInfo(const MemberInfoOld* member) {
    if (!member) return;

    printf("Member Info (Old):\n");
    printf("  M_ID: %u\n", member->m_id);
    printf("  User ID: %s\n", member->user_id);
    printf("  User Name: %s\n", member->user_name);
    printf("  Nickname: %s\n", member->nickname);
    printf("  Email: %s (Subscription: %c)\n", member->email, member->email_yn);
    printf("  Mobile: %s\n", member->mobile_no);
    printf("  State: %d\n", member->state);
    printf("  Reg Date: %u\n", member->reg_date);
    printf("  Last Play: %s\n", member->last_play_time);
    printf("  SSN Check: %u\n", member->ssn_check);
}
