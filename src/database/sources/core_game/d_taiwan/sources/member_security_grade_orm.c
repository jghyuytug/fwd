#include "member_security_grade_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 25

/* ======================================== CRUD Operations ======================================== */

int MemberSecurityGrade_Add(DBConnectionManager* manager, const MemberSecurityGrade* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_security_grade (m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, "
        "last_window_date, goblin_pass_mod, goblin_fail_cnt, security_card_reg, security_card_fail_cnt, "
        "m_opt_reg, pc_opt_reg, black_ip_try_time, linear_pass_fail_cnt, last_pass_fail_time, "
        "last_check_time, pass_modify_check, member_pc_reg, gatekeeper_otp_reg, goblin_validity_time, "
        "security_card_validity_time, validity_ip, cargopad_status, cargopad_mod, cargopad_validity_time) "
        "VALUES (%d, '%s', %d, '%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', %d, %u, '%s', '%s', "
        "'%s', '%s', %d, %d, '%s', %d, '%s', %d)",
        record->m_id, record->last_visit_time, record->pass_fail_cnt, record->last_vaccine_date,
        record->last_window_date, record->goblin_pass_mod, record->goblin_fail_cnt, record->security_card_reg,
        record->security_card_fail_cnt, record->m_opt_reg, record->pc_opt_reg, record->black_ip_try_time,
        record->linear_pass_fail_cnt, record->last_pass_fail_time, record->last_check_time,
        record->pass_modify_check, record->member_pc_reg, record->gatekeeper_otp_reg,
        record->goblin_validity_time, record->security_card_validity_time, record->validity_ip,
        record->cargopad_status, record->cargopad_mod, record->cargopad_validity_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_Get(DBConnectionManager* manager, int m_id, MemberSecurityGrade* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = atoi(values[0]);
        strncpy(record->last_visit_time, values[1], sizeof(record->last_visit_time) - 1);
        record->last_visit_time[sizeof(record->last_visit_time) - 1] = '\0';
        record->pass_fail_cnt = atoi(values[2]);
        strncpy(record->last_vaccine_date, values[3], sizeof(record->last_vaccine_date) - 1);
        record->last_vaccine_date[sizeof(record->last_vaccine_date) - 1] = '\0';
        strncpy(record->last_window_date, values[4], sizeof(record->last_window_date) - 1);
        record->last_window_date[sizeof(record->last_window_date) - 1] = '\0';
        strncpy(record->goblin_pass_mod, values[5], sizeof(record->goblin_pass_mod) - 1);
        record->goblin_pass_mod[sizeof(record->goblin_pass_mod) - 1] = '\0';
        record->goblin_fail_cnt = atoi(values[6]);
        strncpy(record->security_card_reg, values[7], sizeof(record->security_card_reg) - 1);
        record->security_card_reg[sizeof(record->security_card_reg) - 1] = '\0';
        record->security_card_fail_cnt = atoi(values[8]);
        strncpy(record->m_opt_reg, values[9], sizeof(record->m_opt_reg) - 1);
        record->m_opt_reg[sizeof(record->m_opt_reg) - 1] = '\0';
        strncpy(record->pc_opt_reg, values[10], sizeof(record->pc_opt_reg) - 1);
        record->pc_opt_reg[sizeof(record->pc_opt_reg) - 1] = '\0';
        strncpy(record->black_ip_try_time, values[11], sizeof(record->black_ip_try_time) - 1);
        record->black_ip_try_time[sizeof(record->black_ip_try_time) - 1] = '\0';
        record->linear_pass_fail_cnt = atoi(values[12]);
        record->last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(record->last_check_time, values[14], sizeof(record->last_check_time) - 1);
        record->last_check_time[sizeof(record->last_check_time) - 1] = '\0';
        strncpy(record->pass_modify_check, values[15], sizeof(record->pass_modify_check) - 1);
        record->pass_modify_check[sizeof(record->pass_modify_check) - 1] = '\0';
        strncpy(record->member_pc_reg, values[16], sizeof(record->member_pc_reg) - 1);
        record->member_pc_reg[sizeof(record->member_pc_reg) - 1] = '\0';
        strncpy(record->gatekeeper_otp_reg, values[17], sizeof(record->gatekeeper_otp_reg) - 1);
        record->gatekeeper_otp_reg[sizeof(record->gatekeeper_otp_reg) - 1] = '\0';
        record->goblin_validity_time = atoi(values[18]);
        record->security_card_validity_time = atoi(values[19]);
        strncpy(record->validity_ip, values[20], sizeof(record->validity_ip) - 1);
        record->validity_ip[sizeof(record->validity_ip) - 1] = '\0';
        record->cargopad_status = (char)atoi(values[21]);
        strncpy(record->cargopad_mod, values[22], sizeof(record->cargopad_mod) - 1);
        record->cargopad_mod[sizeof(record->cargopad_mod) - 1] = '\0';
        record->cargopad_validity_time = atoi(values[23]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberSecurityGrade_Update(DBConnectionManager* manager, const MemberSecurityGrade* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_security_grade SET last_visit_time = '%s', pass_fail_cnt = %d, "
        "last_vaccine_date = '%s', last_window_date = '%s', goblin_pass_mod = '%s', goblin_fail_cnt = %d, "
        "security_card_reg = '%s', security_card_fail_cnt = %d, m_opt_reg = '%s', pc_opt_reg = '%s', "
        "black_ip_try_time = '%s', linear_pass_fail_cnt = %d, last_pass_fail_time = %u, "
        "last_check_time = '%s', pass_modify_check = '%s', member_pc_reg = '%s', gatekeeper_otp_reg = '%s', "
        "goblin_validity_time = %d, security_card_validity_time = %d, validity_ip = '%s', "
        "cargopad_status = %d, cargopad_mod = '%s', cargopad_validity_time = %d WHERE m_id = %d",
        record->last_visit_time, record->pass_fail_cnt, record->last_vaccine_date, record->last_window_date,
        record->goblin_pass_mod, record->goblin_fail_cnt, record->security_card_reg, record->security_card_fail_cnt,
        record->m_opt_reg, record->pc_opt_reg, record->black_ip_try_time, record->linear_pass_fail_cnt,
        record->last_pass_fail_time, record->last_check_time, record->pass_modify_check, record->member_pc_reg,
        record->gatekeeper_otp_reg, record->goblin_validity_time, record->security_card_validity_time,
        record->validity_ip, record->cargopad_status, record->cargopad_mod, record->cargopad_validity_time,
        record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_Delete(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM member_security_grade WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_security_grade WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberSecurityGrade_IncrementPassFailCnt(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_security_grade SET pass_fail_cnt = pass_fail_cnt + 1, last_pass_fail_time = UNIX_TIMESTAMP() "
        "WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_ResetPassFailCnt(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_security_grade SET pass_fail_cnt = 0, linear_pass_fail_cnt = 0 WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_IncrementLinearPassFailCnt(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_security_grade SET linear_pass_fail_cnt = linear_pass_fail_cnt + 1, "
        "last_pass_fail_time = UNIX_TIMESTAMP() WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_GetByPassFailCntRange(DBConnectionManager* manager, int min_count, int max_count,
                                                 MemberSecurityGrade* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade "
        "WHERE pass_fail_cnt BETWEEN %d AND %d ORDER BY pass_fail_cnt DESC LIMIT %d",
        min_count, max_count, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].last_visit_time, values[1], sizeof(records[count].last_visit_time) - 1);
        records[count].last_visit_time[sizeof(records[count].last_visit_time) - 1] = '\0';
        records[count].pass_fail_cnt = atoi(values[2]);
        strncpy(records[count].last_vaccine_date, values[3], sizeof(records[count].last_vaccine_date) - 1);
        records[count].last_vaccine_date[sizeof(records[count].last_vaccine_date) - 1] = '\0';
        strncpy(records[count].last_window_date, values[4], sizeof(records[count].last_window_date) - 1);
        records[count].last_window_date[sizeof(records[count].last_window_date) - 1] = '\0';
        strncpy(records[count].goblin_pass_mod, values[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_pass_mod[sizeof(records[count].goblin_pass_mod) - 1] = '\0';
        records[count].goblin_fail_cnt = atoi(values[6]);
        strncpy(records[count].security_card_reg, values[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_reg[sizeof(records[count].security_card_reg) - 1] = '\0';
        records[count].security_card_fail_cnt = atoi(values[8]);
        strncpy(records[count].m_opt_reg, values[9], sizeof(records[count].m_opt_reg) - 1);
        records[count].m_opt_reg[sizeof(records[count].m_opt_reg) - 1] = '\0';
        strncpy(records[count].pc_opt_reg, values[10], sizeof(records[count].pc_opt_reg) - 1);
        records[count].pc_opt_reg[sizeof(records[count].pc_opt_reg) - 1] = '\0';
        strncpy(records[count].black_ip_try_time, values[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].black_ip_try_time[sizeof(records[count].black_ip_try_time) - 1] = '\0';
        records[count].linear_pass_fail_cnt = atoi(values[12]);
        records[count].last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(records[count].last_check_time, values[14], sizeof(records[count].last_check_time) - 1);
        records[count].last_check_time[sizeof(records[count].last_check_time) - 1] = '\0';
        strncpy(records[count].pass_modify_check, values[15], sizeof(records[count].pass_modify_check) - 1);
        records[count].pass_modify_check[sizeof(records[count].pass_modify_check) - 1] = '\0';
        strncpy(records[count].member_pc_reg, values[16], sizeof(records[count].member_pc_reg) - 1);
        records[count].member_pc_reg[sizeof(records[count].member_pc_reg) - 1] = '\0';
        strncpy(records[count].gatekeeper_otp_reg, values[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].gatekeeper_otp_reg[sizeof(records[count].gatekeeper_otp_reg) - 1] = '\0';
        records[count].goblin_validity_time = atoi(values[18]);
        records[count].security_card_validity_time = atoi(values[19]);
        strncpy(records[count].validity_ip, values[20], sizeof(records[count].validity_ip) - 1);
        records[count].validity_ip[sizeof(records[count].validity_ip) - 1] = '\0';
        records[count].cargopad_status = (char)atoi(values[21]);
        strncpy(records[count].cargopad_mod, values[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_mod[sizeof(records[count].cargopad_mod) - 1] = '\0';
        records[count].cargopad_validity_time = atoi(values[23]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSecurityGrade_GetByLinearPassFailCnt(DBConnectionManager* manager, int min_linear_cnt,
                                                  MemberSecurityGrade* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade "
        "WHERE linear_pass_fail_cnt >= %d ORDER BY linear_pass_fail_cnt DESC LIMIT %d",
        min_linear_cnt, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].last_visit_time, values[1], sizeof(records[count].last_visit_time) - 1);
        records[count].last_visit_time[sizeof(records[count].last_visit_time) - 1] = '\0';
        records[count].pass_fail_cnt = atoi(values[2]);
        strncpy(records[count].last_vaccine_date, values[3], sizeof(records[count].last_vaccine_date) - 1);
        records[count].last_vaccine_date[sizeof(records[count].last_vaccine_date) - 1] = '\0';
        strncpy(records[count].last_window_date, values[4], sizeof(records[count].last_window_date) - 1);
        records[count].last_window_date[sizeof(records[count].last_window_date) - 1] = '\0';
        strncpy(records[count].goblin_pass_mod, values[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_pass_mod[sizeof(records[count].goblin_pass_mod) - 1] = '\0';
        records[count].goblin_fail_cnt = atoi(values[6]);
        strncpy(records[count].security_card_reg, values[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_reg[sizeof(records[count].security_card_reg) - 1] = '\0';
        records[count].security_card_fail_cnt = atoi(values[8]);
        strncpy(records[count].m_opt_reg, values[9], sizeof(records[count].m_opt_reg) - 1);
        records[count].m_opt_reg[sizeof(records[count].m_opt_reg) - 1] = '\0';
        strncpy(records[count].pc_opt_reg, values[10], sizeof(records[count].pc_opt_reg) - 1);
        records[count].pc_opt_reg[sizeof(records[count].pc_opt_reg) - 1] = '\0';
        strncpy(records[count].black_ip_try_time, values[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].black_ip_try_time[sizeof(records[count].black_ip_try_time) - 1] = '\0';
        records[count].linear_pass_fail_cnt = atoi(values[12]);
        records[count].last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(records[count].last_check_time, values[14], sizeof(records[count].last_check_time) - 1);
        records[count].last_check_time[sizeof(records[count].last_check_time) - 1] = '\0';
        strncpy(records[count].pass_modify_check, values[15], sizeof(records[count].pass_modify_check) - 1);
        records[count].pass_modify_check[sizeof(records[count].pass_modify_check) - 1] = '\0';
        strncpy(records[count].member_pc_reg, values[16], sizeof(records[count].member_pc_reg) - 1);
        records[count].member_pc_reg[sizeof(records[count].member_pc_reg) - 1] = '\0';
        strncpy(records[count].gatekeeper_otp_reg, values[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].gatekeeper_otp_reg[sizeof(records[count].gatekeeper_otp_reg) - 1] = '\0';
        records[count].goblin_validity_time = atoi(values[18]);
        records[count].security_card_validity_time = atoi(values[19]);
        strncpy(records[count].validity_ip, values[20], sizeof(records[count].validity_ip) - 1);
        records[count].validity_ip[sizeof(records[count].validity_ip) - 1] = '\0';
        records[count].cargopad_status = (char)atoi(values[21]);
        strncpy(records[count].cargopad_mod, values[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_mod[sizeof(records[count].cargopad_mod) - 1] = '\0';
        records[count].cargopad_validity_time = atoi(values[23]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSecurityGrade_GetByValidityIP(DBConnectionManager* manager, const char* validity_ip,
                                          MemberSecurityGrade* records, int max_records) {
    if (!manager || !validity_ip || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade "
        "WHERE validity_ip LIKE '%s' LIMIT %d", validity_ip, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].last_visit_time, values[1], sizeof(records[count].last_visit_time) - 1);
        records[count].last_visit_time[sizeof(records[count].last_visit_time) - 1] = '\0';
        records[count].pass_fail_cnt = atoi(values[2]);
        strncpy(records[count].last_vaccine_date, values[3], sizeof(records[count].last_vaccine_date) - 1);
        records[count].last_vaccine_date[sizeof(records[count].last_vaccine_date) - 1] = '\0';
        strncpy(records[count].last_window_date, values[4], sizeof(records[count].last_window_date) - 1);
        records[count].last_window_date[sizeof(records[count].last_window_date) - 1] = '\0';
        strncpy(records[count].goblin_pass_mod, values[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_pass_mod[sizeof(records[count].goblin_pass_mod) - 1] = '\0';
        records[count].goblin_fail_cnt = atoi(values[6]);
        strncpy(records[count].security_card_reg, values[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_reg[sizeof(records[count].security_card_reg) - 1] = '\0';
        records[count].security_card_fail_cnt = atoi(values[8]);
        strncpy(records[count].m_opt_reg, values[9], sizeof(records[count].m_opt_reg) - 1);
        records[count].m_opt_reg[sizeof(records[count].m_opt_reg) - 1] = '\0';
        strncpy(records[count].pc_opt_reg, values[10], sizeof(records[count].pc_opt_reg) - 1);
        records[count].pc_opt_reg[sizeof(records[count].pc_opt_reg) - 1] = '\0';
        strncpy(records[count].black_ip_try_time, values[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].black_ip_try_time[sizeof(records[count].black_ip_try_time) - 1] = '\0';
        records[count].linear_pass_fail_cnt = atoi(values[12]);
        records[count].last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(records[count].last_check_time, values[14], sizeof(records[count].last_check_time) - 1);
        records[count].last_check_time[sizeof(records[count].last_check_time) - 1] = '\0';
        strncpy(records[count].pass_modify_check, values[15], sizeof(records[count].pass_modify_check) - 1);
        records[count].pass_modify_check[sizeof(records[count].pass_modify_check) - 1] = '\0';
        strncpy(records[count].member_pc_reg, values[16], sizeof(records[count].member_pc_reg) - 1);
        records[count].member_pc_reg[sizeof(records[count].member_pc_reg) - 1] = '\0';
        strncpy(records[count].gatekeeper_otp_reg, values[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].gatekeeper_otp_reg[sizeof(records[count].gatekeeper_otp_reg) - 1] = '\0';
        records[count].goblin_validity_time = atoi(values[18]);
        records[count].security_card_validity_time = atoi(values[19]);
        strncpy(records[count].validity_ip, values[20], sizeof(records[count].validity_ip) - 1);
        records[count].validity_ip[sizeof(records[count].validity_ip) - 1] = '\0';
        records[count].cargopad_status = (char)atoi(values[21]);
        strncpy(records[count].cargopad_mod, values[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_mod[sizeof(records[count].cargopad_mod) - 1] = '\0';
        records[count].cargopad_validity_time = atoi(values[23]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSecurityGrade_GetByLastVisitTimeRange(DBConnectionManager* manager, const char* start_time,
                                                   const char* end_time, MemberSecurityGrade* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade "
        "WHERE last_visit_time BETWEEN '%s' AND '%s' ORDER BY last_visit_time DESC LIMIT %d",
        start_time, end_time, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].last_visit_time, values[1], sizeof(records[count].last_visit_time) - 1);
        records[count].last_visit_time[sizeof(records[count].last_visit_time) - 1] = '\0';
        records[count].pass_fail_cnt = atoi(values[2]);
        strncpy(records[count].last_vaccine_date, values[3], sizeof(records[count].last_vaccine_date) - 1);
        records[count].last_vaccine_date[sizeof(records[count].last_vaccine_date) - 1] = '\0';
        strncpy(records[count].last_window_date, values[4], sizeof(records[count].last_window_date) - 1);
        records[count].last_window_date[sizeof(records[count].last_window_date) - 1] = '\0';
        strncpy(records[count].goblin_pass_mod, values[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_pass_mod[sizeof(records[count].goblin_pass_mod) - 1] = '\0';
        records[count].goblin_fail_cnt = atoi(values[6]);
        strncpy(records[count].security_card_reg, values[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_reg[sizeof(records[count].security_card_reg) - 1] = '\0';
        records[count].security_card_fail_cnt = atoi(values[8]);
        strncpy(records[count].m_opt_reg, values[9], sizeof(records[count].m_opt_reg) - 1);
        records[count].m_opt_reg[sizeof(records[count].m_opt_reg) - 1] = '\0';
        strncpy(records[count].pc_opt_reg, values[10], sizeof(records[count].pc_opt_reg) - 1);
        records[count].pc_opt_reg[sizeof(records[count].pc_opt_reg) - 1] = '\0';
        strncpy(records[count].black_ip_try_time, values[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].black_ip_try_time[sizeof(records[count].black_ip_try_time) - 1] = '\0';
        records[count].linear_pass_fail_cnt = atoi(values[12]);
        records[count].last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(records[count].last_check_time, values[14], sizeof(records[count].last_check_time) - 1);
        records[count].last_check_time[sizeof(records[count].last_check_time) - 1] = '\0';
        strncpy(records[count].pass_modify_check, values[15], sizeof(records[count].pass_modify_check) - 1);
        records[count].pass_modify_check[sizeof(records[count].pass_modify_check) - 1] = '\0';
        strncpy(records[count].member_pc_reg, values[16], sizeof(records[count].member_pc_reg) - 1);
        records[count].member_pc_reg[sizeof(records[count].member_pc_reg) - 1] = '\0';
        strncpy(records[count].gatekeeper_otp_reg, values[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].gatekeeper_otp_reg[sizeof(records[count].gatekeeper_otp_reg) - 1] = '\0';
        records[count].goblin_validity_time = atoi(values[18]);
        records[count].security_card_validity_time = atoi(values[19]);
        strncpy(records[count].validity_ip, values[20], sizeof(records[count].validity_ip) - 1);
        records[count].validity_ip[sizeof(records[count].validity_ip) - 1] = '\0';
        records[count].cargopad_status = (char)atoi(values[21]);
        strncpy(records[count].cargopad_mod, values[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_mod[sizeof(records[count].cargopad_mod) - 1] = '\0';
        records[count].cargopad_validity_time = atoi(values[23]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSecurityGrade_GetHighRiskUsers(DBConnectionManager* manager, int fail_threshold, int linear_threshold,
                                           MemberSecurityGrade* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, "
        "goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, "
        "linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, "
        "gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, "
        "cargopad_mod, cargopad_validity_time FROM member_security_grade "
        "WHERE pass_fail_cnt >= %d OR linear_pass_fail_cnt >= %d "
        "ORDER BY pass_fail_cnt DESC, linear_pass_fail_cnt DESC LIMIT %d",
        fail_threshold, linear_threshold, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = atoi(values[0]);
        strncpy(records[count].last_visit_time, values[1], sizeof(records[count].last_visit_time) - 1);
        records[count].last_visit_time[sizeof(records[count].last_visit_time) - 1] = '\0';
        records[count].pass_fail_cnt = atoi(values[2]);
        strncpy(records[count].last_vaccine_date, values[3], sizeof(records[count].last_vaccine_date) - 1);
        records[count].last_vaccine_date[sizeof(records[count].last_vaccine_date) - 1] = '\0';
        strncpy(records[count].last_window_date, values[4], sizeof(records[count].last_window_date) - 1);
        records[count].last_window_date[sizeof(records[count].last_window_date) - 1] = '\0';
        strncpy(records[count].goblin_pass_mod, values[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_pass_mod[sizeof(records[count].goblin_pass_mod) - 1] = '\0';
        records[count].goblin_fail_cnt = atoi(values[6]);
        strncpy(records[count].security_card_reg, values[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_reg[sizeof(records[count].security_card_reg) - 1] = '\0';
        records[count].security_card_fail_cnt = atoi(values[8]);
        strncpy(records[count].m_opt_reg, values[9], sizeof(records[count].m_opt_reg) - 1);
        records[count].m_opt_reg[sizeof(records[count].m_opt_reg) - 1] = '\0';
        strncpy(records[count].pc_opt_reg, values[10], sizeof(records[count].pc_opt_reg) - 1);
        records[count].pc_opt_reg[sizeof(records[count].pc_opt_reg) - 1] = '\0';
        strncpy(records[count].black_ip_try_time, values[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].black_ip_try_time[sizeof(records[count].black_ip_try_time) - 1] = '\0';
        records[count].linear_pass_fail_cnt = atoi(values[12]);
        records[count].last_pass_fail_time = (unsigned int)strtoul(values[13], NULL, 10);
        strncpy(records[count].last_check_time, values[14], sizeof(records[count].last_check_time) - 1);
        records[count].last_check_time[sizeof(records[count].last_check_time) - 1] = '\0';
        strncpy(records[count].pass_modify_check, values[15], sizeof(records[count].pass_modify_check) - 1);
        records[count].pass_modify_check[sizeof(records[count].pass_modify_check) - 1] = '\0';
        strncpy(records[count].member_pc_reg, values[16], sizeof(records[count].member_pc_reg) - 1);
        records[count].member_pc_reg[sizeof(records[count].member_pc_reg) - 1] = '\0';
        strncpy(records[count].gatekeeper_otp_reg, values[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].gatekeeper_otp_reg[sizeof(records[count].gatekeeper_otp_reg) - 1] = '\0';
        records[count].goblin_validity_time = atoi(values[18]);
        records[count].security_card_validity_time = atoi(values[19]);
        strncpy(records[count].validity_ip, values[20], sizeof(records[count].validity_ip) - 1);
        records[count].validity_ip[sizeof(records[count].validity_ip) - 1] = '\0';
        records[count].cargopad_status = (char)atoi(values[21]);
        strncpy(records[count].cargopad_mod, values[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_mod[sizeof(records[count].cargopad_mod) - 1] = '\0';
        records[count].cargopad_validity_time = atoi(values[23]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberSecurityGrade_UpdateLastVisitTime(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_security_grade SET last_visit_time = NOW() WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ======================================== Bulk Operations ======================================== */

int MemberSecurityGrade_BulkAdd(DBConnectionManager* manager, const MemberSecurityGrade* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberSecurityGrade_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberSecurityGrade_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query), "DELETE FROM member_security_grade WHERE m_id = %d", m_ids[i]);

        DBQueryResult result;
        memset(&result, 0, sizeof(DBQueryResult));

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberSecurityGrade_PrintInfo(const MemberSecurityGrade* record) {
    if (!record) {
        printf("MemberSecurityGrade is NULL\n");
        return;
    }

    printf("=== Member Security Grade Info ===\n");
    printf("  m_id:                       %d\n", record->m_id);
    printf("  last_visit_time:            %s\n", record->last_visit_time);
    printf("  pass_fail_cnt:              %d\n", record->pass_fail_cnt);
    printf("  last_vaccine_date:          %s\n", record->last_vaccine_date);
    printf("  last_window_date:           %s\n", record->last_window_date);
    printf("  goblin_pass_mod:            %s\n", record->goblin_pass_mod);
    printf("  goblin_fail_cnt:            %d\n", record->goblin_fail_cnt);
    printf("  security_card_reg:          %s\n", record->security_card_reg);
    printf("  security_card_fail_cnt:     %d\n", record->security_card_fail_cnt);
    printf("  m_opt_reg:                  %s\n", record->m_opt_reg);
    printf("  pc_opt_reg:                 %s\n", record->pc_opt_reg);
    printf("  black_ip_try_time:          %s\n", record->black_ip_try_time);
    printf("  linear_pass_fail_cnt:       %d\n", record->linear_pass_fail_cnt);
    printf("  last_pass_fail_time:        %u\n", record->last_pass_fail_time);
    printf("  last_check_time:            %s\n", record->last_check_time);
    printf("  pass_modify_check:          %s\n", record->pass_modify_check);
    printf("  member_pc_reg:              %s\n", record->member_pc_reg);
    printf("  gatekeeper_otp_reg:         %s\n", record->gatekeeper_otp_reg);
    printf("  goblin_validity_time:       %d\n", record->goblin_validity_time);
    printf("  security_card_validity_time:%d\n", record->security_card_validity_time);
    printf("  validity_ip:                %s\n", record->validity_ip);
    printf("  cargopad_status:            %d\n", record->cargopad_status);
    printf("  cargopad_mod:               %s\n", record->cargopad_mod);
    printf("  cargopad_validity_time:     %d\n", record->cargopad_validity_time);
    printf("===================================\n");
}
