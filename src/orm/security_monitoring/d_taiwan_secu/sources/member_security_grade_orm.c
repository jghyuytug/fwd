#include "member_security_grade_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberSecurityGrade_Add(DBConnectionManager* manager, const MemberSecurityGrade* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_security_grade (m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, cargopad_mod, cargopad_validity_time) "
        "VALUES (%d, '%s', %d, '%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d)",
        record->m_id, record->last_visit_time, record->pass_fail_cnt, record->last_vaccine_date, record->last_window_date, record->goblin_pass_mod, record->goblin_fail_cnt, record->security_card_reg, record->security_card_fail_cnt, record->m_opt_reg, record->pc_opt_reg, record->black_ip_try_time, record->linear_pass_fail_cnt, record->last_pass_fail_time, record->last_check_time, record->pass_modify_check, record->member_pc_reg, record->gatekeeper_otp_reg, record->goblin_validity_time, record->security_card_validity_time, record->validity_ip, record->cargopad_status, record->cargopad_mod, record->cargopad_validity_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_Get(DBConnectionManager* manager, int m_id, MemberSecurityGrade* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[24];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, cargopad_mod, cargopad_validity_time FROM member_security_grade WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberSecurityGrade));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->last_visit_time, row[1], sizeof(record->last_visit_time) - 1);
    record->pass_fail_cnt = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->last_vaccine_date, row[3], sizeof(record->last_vaccine_date) - 1);
    if (row[4]) strncpy(record->last_window_date, row[4], sizeof(record->last_window_date) - 1);
    if (row[5]) strncpy(record->goblin_pass_mod, row[5], sizeof(record->goblin_pass_mod) - 1);
    record->goblin_fail_cnt = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->security_card_reg, row[7], sizeof(record->security_card_reg) - 1);
    record->security_card_fail_cnt = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->m_opt_reg, row[9], sizeof(record->m_opt_reg) - 1);
    if (row[10]) strncpy(record->pc_opt_reg, row[10], sizeof(record->pc_opt_reg) - 1);
    if (row[11]) strncpy(record->black_ip_try_time, row[11], sizeof(record->black_ip_try_time) - 1);
    record->linear_pass_fail_cnt = row[12] ? atoi(row[12]) : 0;
    record->last_pass_fail_time = row[13] ? atoi(row[13]) : 0;
    if (row[14]) strncpy(record->last_check_time, row[14], sizeof(record->last_check_time) - 1);
    if (row[15]) strncpy(record->pass_modify_check, row[15], sizeof(record->pass_modify_check) - 1);
    if (row[16]) strncpy(record->member_pc_reg, row[16], sizeof(record->member_pc_reg) - 1);
    if (row[17]) strncpy(record->gatekeeper_otp_reg, row[17], sizeof(record->gatekeeper_otp_reg) - 1);
    record->goblin_validity_time = row[18] ? atoi(row[18]) : 0;
    record->security_card_validity_time = row[19] ? atoi(row[19]) : 0;
    if (row[20]) strncpy(record->validity_ip, row[20], sizeof(record->validity_ip) - 1);
    record->cargopad_status = row[21] ? atoi(row[21]) : 0;
    if (row[22]) strncpy(record->cargopad_mod, row[22], sizeof(record->cargopad_mod) - 1);
    record->cargopad_validity_time = row[23] ? atoi(row[23]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityGrade_GetAll(DBConnectionManager* manager, MemberSecurityGrade* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[24];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, last_visit_time, pass_fail_cnt, last_vaccine_date, last_window_date, goblin_pass_mod, goblin_fail_cnt, security_card_reg, security_card_fail_cnt, m_opt_reg, pc_opt_reg, black_ip_try_time, linear_pass_fail_cnt, last_pass_fail_time, last_check_time, pass_modify_check, member_pc_reg, gatekeeper_otp_reg, goblin_validity_time, security_card_validity_time, validity_ip, cargopad_status, cargopad_mod, cargopad_validity_time FROM member_security_grade");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberSecurityGrade));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].last_visit_time, row[1], sizeof(records[count].last_visit_time) - 1);
        records[count].pass_fail_cnt = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].last_vaccine_date, row[3], sizeof(records[count].last_vaccine_date) - 1);
        if (row[4]) strncpy(records[count].last_window_date, row[4], sizeof(records[count].last_window_date) - 1);
        if (row[5]) strncpy(records[count].goblin_pass_mod, row[5], sizeof(records[count].goblin_pass_mod) - 1);
        records[count].goblin_fail_cnt = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].security_card_reg, row[7], sizeof(records[count].security_card_reg) - 1);
        records[count].security_card_fail_cnt = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].m_opt_reg, row[9], sizeof(records[count].m_opt_reg) - 1);
        if (row[10]) strncpy(records[count].pc_opt_reg, row[10], sizeof(records[count].pc_opt_reg) - 1);
        if (row[11]) strncpy(records[count].black_ip_try_time, row[11], sizeof(records[count].black_ip_try_time) - 1);
        records[count].linear_pass_fail_cnt = row[12] ? atoi(row[12]) : 0;
        records[count].last_pass_fail_time = row[13] ? atoi(row[13]) : 0;
        if (row[14]) strncpy(records[count].last_check_time, row[14], sizeof(records[count].last_check_time) - 1);
        if (row[15]) strncpy(records[count].pass_modify_check, row[15], sizeof(records[count].pass_modify_check) - 1);
        if (row[16]) strncpy(records[count].member_pc_reg, row[16], sizeof(records[count].member_pc_reg) - 1);
        if (row[17]) strncpy(records[count].gatekeeper_otp_reg, row[17], sizeof(records[count].gatekeeper_otp_reg) - 1);
        records[count].goblin_validity_time = row[18] ? atoi(row[18]) : 0;
        records[count].security_card_validity_time = row[19] ? atoi(row[19]) : 0;
        if (row[20]) strncpy(records[count].validity_ip, row[20], sizeof(records[count].validity_ip) - 1);
        records[count].cargopad_status = row[21] ? atoi(row[21]) : 0;
        if (row[22]) strncpy(records[count].cargopad_mod, row[22], sizeof(records[count].cargopad_mod) - 1);
        records[count].cargopad_validity_time = row[23] ? atoi(row[23]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
