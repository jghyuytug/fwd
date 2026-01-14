#ifndef MEMBER_SECURITY_GRADE_ORM_H
#define MEMBER_SECURITY_GRADE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char last_visit_time[20];
    int pass_fail_cnt;
    char last_vaccine_date[20];
    char last_window_date[20];
    char goblin_pass_mod[20];
    int goblin_fail_cnt;
    char security_card_reg[20];
    int security_card_fail_cnt;
    char m_opt_reg[20];
    char pc_opt_reg[20];
    char black_ip_try_time[20];
    int linear_pass_fail_cnt;
    int last_pass_fail_time;
    char last_check_time[20];
    char pass_modify_check[20];
    char member_pc_reg[20];
    char gatekeeper_otp_reg[20];
    int goblin_validity_time;
    int security_card_validity_time;
    char validity_ip[16];
    signed char cargopad_status;
    char cargopad_mod[20];
    int cargopad_validity_time;
} MemberSecurityGrade;

/* CRUD Operations */
int MemberSecurityGrade_Add(DBConnectionManager* manager, const MemberSecurityGrade* record);
int MemberSecurityGrade_Get(DBConnectionManager* manager, int m_id, MemberSecurityGrade* record);
int MemberSecurityGrade_Update(DBConnectionManager* manager, const MemberSecurityGrade* record);
int MemberSecurityGrade_Delete(DBConnectionManager* manager, int m_id);
int MemberSecurityGrade_Exists(DBConnectionManager* manager, int m_id);
int MemberSecurityGrade_GetAll(DBConnectionManager* manager, MemberSecurityGrade* records, int max_count, int* actual_count);

#endif /* MEMBER_SECURITY_GRADE_ORM_H */