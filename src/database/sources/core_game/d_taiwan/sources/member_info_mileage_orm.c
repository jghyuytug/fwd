#include "member_info_mileage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 8192

/* Add a new member (m_id is auto-generated) */
int MemberInfoMileage_Add(DBConnectionManager* manager, const MemberInfoMileage* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_info_mileage (user_id, user_name, first_ssn, second_ssn, "
        "passwd, mobile_no, reg_date, email, q_no, q_answer, state, nickname, "
        "email_yn, ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, mileage) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %u, '%s', %u, '%s', %d, '%s', "
        "'%c', %u, %u, '%s', %d, %d, %d)",
        member->user_id, member->user_name, member->first_ssn, member->second_ssn,
        member->passwd, member->mobile_no, member->reg_date, member->email,
        member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->slot, member->last_play_time,
        member->hangame_flag, member->hanmon_flag, member->mileage);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by m_id */
int MemberInfoMileage_Get(DBConnectionManager* manager, unsigned int m_id, MemberInfoMileage* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, mileage "
        "FROM member_info_mileage WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoMileage));
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
    member->state = row[12] ? (char)atoi(row[12]) : 1;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;
    member->mileage = row[20] ? atoi(row[20]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing member */
int MemberInfoMileage_Update(DBConnectionManager* manager, const MemberInfoMileage* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_mileage SET user_id = '%s', user_name = '%s', "
        "first_ssn = '%s', second_ssn = '%s', passwd = '%s', mobile_no = '%s', "
        "reg_date = %u, email = '%s', q_no = %u, q_answer = '%s', state = %d, "
        "nickname = '%s', email_yn = '%c', ssn_check = %u, slot = %u, "
        "last_play_time = '%s', hangame_flag = %d, hanmon_flag = %d, mileage = %d "
        "WHERE m_id = %u",
        member->user_id, member->user_name, member->first_ssn, member->second_ssn,
        member->passwd, member->mobile_no, member->reg_date, member->email,
        member->q_no, member->q_answer, member->state, member->nickname,
        member->email_yn, member->ssn_check, member->slot, member->last_play_time,
        member->hangame_flag, member->hanmon_flag, member->mileage, member->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete member by m_id */
int MemberInfoMileage_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_info_mileage WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if member exists */
int MemberInfoMileage_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_info_mileage WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get member by user_id */
int MemberInfoMileage_GetByUserId(DBConnectionManager* manager, const char* user_id,
                                   MemberInfoMileage* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];

    if (!manager || !user_id || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, mileage "
        "FROM member_info_mileage WHERE user_id = '%s'", user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoMileage));
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
    member->state = row[12] ? (char)atoi(row[12]) : 1;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;
    member->mileage = row[20] ? atoi(row[20]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member by nickname */
int MemberInfoMileage_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                     MemberInfoMileage* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];

    if (!manager || !nickname || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
        "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, mileage "
        "FROM member_info_mileage WHERE nickname = '%s'", nickname);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfoMileage));
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
    member->state = row[12] ? (char)atoi(row[12]) : 1;
    if (row[13]) strncpy(member->nickname, row[13], sizeof(member->nickname) - 1);
    member->email_yn = row[14] ? row[14][0] : 'y';
    member->ssn_check = row[15] ? (unsigned char)atoi(row[15]) : 0;
    member->slot = row[16] ? (unsigned int)atoi(row[16]) : 8;
    if (row[17]) strncpy(member->last_play_time, row[17], sizeof(member->last_play_time) - 1);
    member->hangame_flag = row[18] ? (char)atoi(row[18]) : 0;
    member->hanmon_flag = row[19] ? (char)atoi(row[19]) : 0;
    member->mileage = row[20] ? atoi(row[20]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update mileage by delta (add or subtract) */
int MemberInfoMileage_UpdateMileage(DBConnectionManager* manager, unsigned int m_id,
                                     int mileage_delta) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_mileage SET mileage = mileage + %d WHERE m_id = %u",
        mileage_delta, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update password */
int MemberInfoMileage_UpdatePassword(DBConnectionManager* manager, unsigned int m_id,
                                      const char* new_passwd) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_passwd) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_mileage SET passwd = '%s' WHERE m_id = %u",
        new_passwd, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update email */
int MemberInfoMileage_UpdateEmail(DBConnectionManager* manager, unsigned int m_id,
                                   const char* new_email) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_email) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_mileage SET email = '%s' WHERE m_id = %u",
        new_email, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update last play time to current time */
int MemberInfoMileage_UpdateLastPlayTime(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_info_mileage SET last_play_time = NOW() WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Count members by state */
int MemberInfoMileage_CountByState(DBConnectionManager* manager, char state, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_info_mileage WHERE state = %d", state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    *count = 0;
    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Print member information */
void MemberInfoMileage_PrintInfo(const MemberInfoMileage* member) {
    if (!member) return;

    printf("Member Info:\n");
    printf("  m_id: %u\n", member->m_id);
    printf("  user_id: %s\n", member->user_id);
    printf("  user_name: %s\n", member->user_name);
    printf("  nickname: %s\n", member->nickname);
    printf("  email: %s\n", member->email);
    printf("  mobile_no: %s\n", member->mobile_no);
    printf("  reg_date: %u\n", member->reg_date);
    printf("  state: %d\n", member->state);
    printf("  slot: %u\n", member->slot);
    printf("  mileage: %d\n", member->mileage);
    printf("  email_yn: %c\n", member->email_yn);
    printf("  last_play_time: %s\n", member->last_play_time);
    printf("  updt_date: %s\n", member->updt_date);
}
