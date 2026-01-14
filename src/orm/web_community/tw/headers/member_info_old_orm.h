#ifndef MEMBER_INFO_OLD_ORM_H
#define MEMBER_INFO_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char user_id[16];
    char user_name[6];
    char first_ssn[11];
    char second_ssn[11];
    char passwd[9];
    char mobile_no[9];
    int reg_date;
    char email[26];
    signed char q_no;
    char q_answer[16];
    char updt_date[20];
    signed char state;
    char nickname[9];
    char email_yn;
    signed char ssn_check;
    char last_play_time[20];
} MemberInfoOld;

/* CRUD Operations */
int MemberInfoOld_Add(DBConnectionManager* manager, const MemberInfoOld* record);
int MemberInfoOld_Get(DBConnectionManager* manager, int m_id, MemberInfoOld* record);
int MemberInfoOld_Update(DBConnectionManager* manager, const MemberInfoOld* record);
int MemberInfoOld_Delete(DBConnectionManager* manager, int m_id);
int MemberInfoOld_Exists(DBConnectionManager* manager, int m_id);
int MemberInfoOld_GetAll(DBConnectionManager* manager, MemberInfoOld* records, int max_count, int* actual_count);

#endif /* MEMBER_INFO_OLD_ORM_H */