#include "member_info_utf8_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new member */
int MemberInfoUtf8_Add(DBConnectionManager* manager, const MemberInfoUtf8* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_info_utf8 (m_id, user_id, user_name, first_ssn, second_ssn, "
        "passwd, mobile_no, reg_date, email, q_no, q_answer, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', %u, '%s', %u, '%s', %d, '%s', "
        "'%c', %u, %u, '%s', %d, %d)",
        member->m_id, member->user_id, member->user_name, member->first_ssn,
        member->second_ssn, member->passwd, member->mobile_no, member->reg_date,
        member->email, member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->slot, member->last_play_time,
        member->hangame_flag, member->hanmon_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by m_id */
int MemberInfoUtf8_Get(DBConnectionManager* manager, int m_id, MemberInfoUtf8* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[20];

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag "
        "FROM member_info_utf8 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoUtf8));
    member->m_id = row[0] ? atoi(row[0]) : 0;
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
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update member */
int MemberInfoUtf8_Update(DBConnectionManager* manager, const MemberInfoUtf8* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_utf8 SET user_id = '%s', user_name = '%s', "
        "first_ssn = '%s', second_ssn = '%s', passwd = '%s', mobile_no = '%s', "
        "reg_date = %u, email = '%s', q_no = %u, q_answer = '%s', state = %d, "
        "nickname = '%s', email_yn = '%c', ssn_check = %u, slot = %u, "
        "last_play_time = '%s', hangame_flag = %d, hanmon_flag = %d WHERE m_id = %d",
        member->user_id, member->user_name, member->first_ssn, member->second_ssn,
        member->passwd, member->mobile_no, member->reg_date, member->email,
        member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->slot, member->last_play_time,
        member->hangame_flag, member->hanmon_flag, member->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete member */
int MemberInfoUtf8_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_utf8 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if member exists */
int MemberInfoUtf8_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_info_utf8 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all members */
int MemberInfoUtf8_GetAll(DBConnectionManager* manager, MemberInfoUtf8* members,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[20];
    int count = 0;

    if (!manager || !members || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag "
        "FROM member_info_utf8 ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(MemberInfoUtf8));
        members[count].m_id = row[0] ? atoi(row[0]) : 0;
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
        members[count].state = row[12] ? row[12][0] : '\0';
        if (row[13]) strncpy(members[count].nickname, row[13], sizeof(members[count].nickname) - 1);
        members[count].email_yn = row[14] ? row[14][0] : 'n';
        members[count].ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
        members[count].slot = row[16] ? (unsigned int)atoi(row[16]) : 0;
        if (row[17]) strncpy(members[count].last_play_time, row[17], sizeof(members[count].last_play_time) - 1);
        members[count].hangame_flag = row[18] ? row[18][0] : '\0';
        members[count].hanmon_flag = row[19] ? row[19][0] : '\0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by user_id */
int MemberInfoUtf8_GetByUserId(DBConnectionManager* manager, const char* user_id,
                                MemberInfoUtf8* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[20];

    if (!manager || !user_id || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag "
        "FROM member_info_utf8 WHERE user_id = '%s'", user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoUtf8));
    member->m_id = row[0] ? atoi(row[0]) : 0;
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
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by nickname */
int MemberInfoUtf8_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                  MemberInfoUtf8* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[20];

    if (!manager || !nickname || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag "
        "FROM member_info_utf8 WHERE nickname = '%s'", nickname);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoUtf8));
    member->m_id = row[0] ? atoi(row[0]) : 0;
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
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update password */
int MemberInfoUtf8_UpdatePassword(DBConnectionManager* manager, int m_id,
                                   const char* new_passwd) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_passwd) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_utf8 SET passwd = '%s' WHERE m_id = %d",
        new_passwd, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update slot */
int MemberInfoUtf8_UpdateSlot(DBConnectionManager* manager, int m_id,
                               unsigned int new_slot) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_utf8 SET slot = %u WHERE m_id = %d",
        new_slot, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print member information */
void MemberInfoUtf8_PrintInfo(const MemberInfoUtf8* member) {
    if (!member) return;

    printf("Member Info (UTF8):\n");
    printf("  M_ID: %d\n", member->m_id);
    printf("  User ID: %s\n", member->user_id);
    printf("  User Name: %s\n", member->user_name);
    printf("  Nickname: %s\n", member->nickname);
    printf("  Email: %s (Sub: %c)\n", member->email, member->email_yn);
    printf("  Mobile: %s\n", member->mobile_no);
    printf("  State: %d\n", member->state);
    printf("  Slot: %u\n", member->slot);
    printf("  Reg Date: %u\n", member->reg_date);
    printf("  Last Play: %s\n", member->last_play_time);
}
