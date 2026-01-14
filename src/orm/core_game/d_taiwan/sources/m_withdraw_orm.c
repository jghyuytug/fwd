#include "m_withdraw_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int MWithdraw_Add(DBConnectionManager* manager, const MWithdraw* withdraw) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !withdraw) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO m_withdraw (user_id, user_name, first_ssn, second_ssn, passwd, "
        "mobile_no, reg_date, email, q_no, q_answer, state, w_type, w_cause, w_date, nickname) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', %u, '%s', %u, %d, '%s', %d, '%s')",
        withdraw->user_id, withdraw->user_name, withdraw->first_ssn, withdraw->second_ssn,
        withdraw->passwd, withdraw->mobile_no, withdraw->reg_date, withdraw->email,
        withdraw->q_no, withdraw->q_answer, withdraw->state, withdraw->w_type,
        withdraw->w_cause, withdraw->w_date, withdraw->nickname);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_Get(DBConnectionManager* manager, unsigned int m_id, MWithdraw* withdraw) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !withdraw) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, w_type, w_cause, w_date, nickname "
        "FROM m_withdraw WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(withdraw, 0, sizeof(MWithdraw));
    withdraw->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(withdraw->user_id, row[1], sizeof(withdraw->user_id) - 1);
    if (row[2]) strncpy(withdraw->user_name, row[2], sizeof(withdraw->user_name) - 1);
    if (row[3]) strncpy(withdraw->first_ssn, row[3], sizeof(withdraw->first_ssn) - 1);
    if (row[4]) strncpy(withdraw->second_ssn, row[4], sizeof(withdraw->second_ssn) - 1);
    if (row[5]) strncpy(withdraw->passwd, row[5], sizeof(withdraw->passwd) - 1);
    if (row[6]) strncpy(withdraw->mobile_no, row[6], sizeof(withdraw->mobile_no) - 1);
    withdraw->reg_date = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(withdraw->email, row[8], sizeof(withdraw->email) - 1);
    withdraw->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(withdraw->q_answer, row[10], sizeof(withdraw->q_answer) - 1);
    if (row[11]) strncpy(withdraw->updt_date, row[11], sizeof(withdraw->updt_date) - 1);
    withdraw->state = row[12] ? (unsigned char)atoi(row[12]) : 0;
    withdraw->w_type = row[13] ? (short)atoi(row[13]) : 0;
    if (row[14]) strncpy(withdraw->w_cause, row[14], sizeof(withdraw->w_cause) - 1);
    withdraw->w_date = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(withdraw->nickname, row[16], sizeof(withdraw->nickname) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_Update(DBConnectionManager* manager, const MWithdraw* withdraw) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !withdraw) return -1;

    snprintf(query, sizeof(query),
        "UPDATE m_withdraw SET user_id = '%s', user_name = '%s', first_ssn = '%s', "
        "second_ssn = '%s', passwd = '%s', mobile_no = '%s', reg_date = %d, email = '%s', "
        "q_no = %u, q_answer = '%s', state = %u, w_type = %d, w_cause = '%s', "
        "w_date = %d, nickname = '%s' WHERE m_id = %u",
        withdraw->user_id, withdraw->user_name, withdraw->first_ssn, withdraw->second_ssn,
        withdraw->passwd, withdraw->mobile_no, withdraw->reg_date, withdraw->email,
        withdraw->q_no, withdraw->q_answer, withdraw->state, withdraw->w_type,
        withdraw->w_cause, withdraw->w_date, withdraw->nickname, withdraw->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM m_withdraw WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM m_withdraw WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Specialized Operations */

int MWithdraw_UpdateState(DBConnectionManager* manager, unsigned int m_id, unsigned char new_state) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE m_withdraw SET state = %u WHERE m_id = %u",
        new_state, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_UpdateWithdrawInfo(DBConnectionManager* manager, unsigned int m_id,
                                  short w_type, const char* w_cause, int w_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !w_cause) return -1;

    snprintf(query, sizeof(query),
        "UPDATE m_withdraw SET w_type = %d, w_cause = '%s', w_date = %d WHERE m_id = %u",
        w_type, w_cause, w_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_UpdateEmail(DBConnectionManager* manager, unsigned int m_id, const char* new_email) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_email) return -1;

    snprintf(query, sizeof(query),
        "UPDATE m_withdraw SET email = '%s' WHERE m_id = %u",
        new_email, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Query Operations */

int MWithdraw_GetByUserId(DBConnectionManager* manager, const char* user_id, MWithdraw* withdraw) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !user_id || !withdraw) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, w_type, w_cause, w_date, nickname "
        "FROM m_withdraw WHERE user_id = '%s' LIMIT 1",
        user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(withdraw, 0, sizeof(MWithdraw));
    withdraw->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(withdraw->user_id, row[1], sizeof(withdraw->user_id) - 1);
    if (row[2]) strncpy(withdraw->user_name, row[2], sizeof(withdraw->user_name) - 1);
    if (row[3]) strncpy(withdraw->first_ssn, row[3], sizeof(withdraw->first_ssn) - 1);
    if (row[4]) strncpy(withdraw->second_ssn, row[4], sizeof(withdraw->second_ssn) - 1);
    if (row[5]) strncpy(withdraw->passwd, row[5], sizeof(withdraw->passwd) - 1);
    if (row[6]) strncpy(withdraw->mobile_no, row[6], sizeof(withdraw->mobile_no) - 1);
    withdraw->reg_date = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(withdraw->email, row[8], sizeof(withdraw->email) - 1);
    withdraw->q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(withdraw->q_answer, row[10], sizeof(withdraw->q_answer) - 1);
    if (row[11]) strncpy(withdraw->updt_date, row[11], sizeof(withdraw->updt_date) - 1);
    withdraw->state = row[12] ? (unsigned char)atoi(row[12]) : 0;
    withdraw->w_type = row[13] ? (short)atoi(row[13]) : 0;
    if (row[14]) strncpy(withdraw->w_cause, row[14], sizeof(withdraw->w_cause) - 1);
    withdraw->w_date = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(withdraw->nickname, row[16], sizeof(withdraw->nickname) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_GetByRegDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                 MWithdraw* withdraws, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !withdraws || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, w_type, w_cause, w_date, nickname "
        "FROM m_withdraw WHERE reg_date >= %d AND reg_date <= %d ORDER BY reg_date DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&withdraws[count], 0, sizeof(MWithdraw));
        withdraws[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(withdraws[count].user_id, row[1], sizeof(withdraws[count].user_id) - 1);
        if (row[2]) strncpy(withdraws[count].user_name, row[2], sizeof(withdraws[count].user_name) - 1);
        if (row[3]) strncpy(withdraws[count].first_ssn, row[3], sizeof(withdraws[count].first_ssn) - 1);
        if (row[4]) strncpy(withdraws[count].second_ssn, row[4], sizeof(withdraws[count].second_ssn) - 1);
        if (row[5]) strncpy(withdraws[count].passwd, row[5], sizeof(withdraws[count].passwd) - 1);
        if (row[6]) strncpy(withdraws[count].mobile_no, row[6], sizeof(withdraws[count].mobile_no) - 1);
        withdraws[count].reg_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(withdraws[count].email, row[8], sizeof(withdraws[count].email) - 1);
        withdraws[count].q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(withdraws[count].q_answer, row[10], sizeof(withdraws[count].q_answer) - 1);
        if (row[11]) strncpy(withdraws[count].updt_date, row[11], sizeof(withdraws[count].updt_date) - 1);
        withdraws[count].state = row[12] ? (unsigned char)atoi(row[12]) : 0;
        withdraws[count].w_type = row[13] ? (short)atoi(row[13]) : 0;
        if (row[14]) strncpy(withdraws[count].w_cause, row[14], sizeof(withdraws[count].w_cause) - 1);
        withdraws[count].w_date = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(withdraws[count].nickname, row[16], sizeof(withdraws[count].nickname) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_GetByState(DBConnectionManager* manager, unsigned char state,
                         MWithdraw* withdraws, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !withdraws || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, w_type, w_cause, w_date, nickname "
        "FROM m_withdraw WHERE state = %u ORDER BY m_id DESC",
        state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&withdraws[count], 0, sizeof(MWithdraw));
        withdraws[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(withdraws[count].user_id, row[1], sizeof(withdraws[count].user_id) - 1);
        if (row[2]) strncpy(withdraws[count].user_name, row[2], sizeof(withdraws[count].user_name) - 1);
        if (row[3]) strncpy(withdraws[count].first_ssn, row[3], sizeof(withdraws[count].first_ssn) - 1);
        if (row[4]) strncpy(withdraws[count].second_ssn, row[4], sizeof(withdraws[count].second_ssn) - 1);
        if (row[5]) strncpy(withdraws[count].passwd, row[5], sizeof(withdraws[count].passwd) - 1);
        if (row[6]) strncpy(withdraws[count].mobile_no, row[6], sizeof(withdraws[count].mobile_no) - 1);
        withdraws[count].reg_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(withdraws[count].email, row[8], sizeof(withdraws[count].email) - 1);
        withdraws[count].q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(withdraws[count].q_answer, row[10], sizeof(withdraws[count].q_answer) - 1);
        if (row[11]) strncpy(withdraws[count].updt_date, row[11], sizeof(withdraws[count].updt_date) - 1);
        withdraws[count].state = row[12] ? (unsigned char)atoi(row[12]) : 0;
        withdraws[count].w_type = row[13] ? (short)atoi(row[13]) : 0;
        if (row[14]) strncpy(withdraws[count].w_cause, row[14], sizeof(withdraws[count].w_cause) - 1);
        withdraws[count].w_date = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(withdraws[count].nickname, row[16], sizeof(withdraws[count].nickname) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_GetByWithdrawType(DBConnectionManager* manager, short w_type,
                                 MWithdraw* withdraws, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !withdraws || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
        "reg_date, email, q_no, q_answer, updt_date, state, w_type, w_cause, w_date, nickname "
        "FROM m_withdraw WHERE w_type = %d ORDER BY w_date DESC",
        w_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&withdraws[count], 0, sizeof(MWithdraw));
        withdraws[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(withdraws[count].user_id, row[1], sizeof(withdraws[count].user_id) - 1);
        if (row[2]) strncpy(withdraws[count].user_name, row[2], sizeof(withdraws[count].user_name) - 1);
        if (row[3]) strncpy(withdraws[count].first_ssn, row[3], sizeof(withdraws[count].first_ssn) - 1);
        if (row[4]) strncpy(withdraws[count].second_ssn, row[4], sizeof(withdraws[count].second_ssn) - 1);
        if (row[5]) strncpy(withdraws[count].passwd, row[5], sizeof(withdraws[count].passwd) - 1);
        if (row[6]) strncpy(withdraws[count].mobile_no, row[6], sizeof(withdraws[count].mobile_no) - 1);
        withdraws[count].reg_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(withdraws[count].email, row[8], sizeof(withdraws[count].email) - 1);
        withdraws[count].q_no = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(withdraws[count].q_answer, row[10], sizeof(withdraws[count].q_answer) - 1);
        if (row[11]) strncpy(withdraws[count].updt_date, row[11], sizeof(withdraws[count].updt_date) - 1);
        withdraws[count].state = row[12] ? (unsigned char)atoi(row[12]) : 0;
        withdraws[count].w_type = row[13] ? (short)atoi(row[13]) : 0;
        if (row[14]) strncpy(withdraws[count].w_cause, row[14], sizeof(withdraws[count].w_cause) - 1);
        withdraws[count].w_date = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(withdraws[count].nickname, row[16], sizeof(withdraws[count].nickname) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MWithdraw_CountByState(DBConnectionManager* manager, unsigned char state) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM m_withdraw WHERE state = %u", state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int MWithdraw_CountByWithdrawType(DBConnectionManager* manager, short w_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM m_withdraw WHERE w_type = %d", w_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int MWithdraw_CountByRegDateRange(DBConnectionManager* manager, int start_date, int end_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM m_withdraw WHERE reg_date >= %d AND reg_date <= %d",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

/* Batch Operations */

int MWithdraw_DeleteByRegDateRange(DBConnectionManager* manager, int start_date, int end_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM m_withdraw WHERE reg_date >= %d AND reg_date <= %d",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions */

void MWithdraw_PrintInfo(const MWithdraw* withdraw) {
    if (!withdraw) {
        printf("MWithdraw: NULL\n");
        return;
    }

    printf("=== Member Withdraw Info ===\n");
    printf("ID:           %u\n", withdraw->m_id);
    printf("User ID:      %s\n", withdraw->user_id);
    printf("User Name:    %s\n", withdraw->user_name);
    printf("Nickname:     %s\n", withdraw->nickname);
    printf("SSN:          %s-%s\n", withdraw->first_ssn, withdraw->second_ssn);
    printf("Email:        %s\n", withdraw->email);
    printf("Mobile:       %s\n", withdraw->mobile_no);
    printf("Reg Date:     %d\n", withdraw->reg_date);
    printf("Update Date:  %s\n", withdraw->updt_date);
    printf("State:        %u\n", withdraw->state);
    printf("W Type:       %d\n", withdraw->w_type);
    printf("W Cause:      %s\n", withdraw->w_cause);
    printf("W Date:       %d\n", withdraw->w_date);
    printf("Q No:         %u\n", withdraw->q_no);
    printf("Q Answer:     %s\n", withdraw->q_answer);
    printf("============================\n");
}
