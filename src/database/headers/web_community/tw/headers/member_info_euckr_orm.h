#ifndef MEMBER_INFO_EUCKR_ORM_H
#define MEMBER_INFO_EUCKR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char user_id[31];
    char user_name[11];
    char first_ssn[7];
    char second_ssn[8];
    char passwd[33];
    char mobile_no[16];
    int reg_date;
    char email[51];
    signed char q_no;
    char q_answer[31];
    char updt_date[20];
    signed char state;
    char nickname[17];
    char email_yn;
    signed char ssn_check;
    int slot;
    char last_play_time[20];
    signed char hangame_flag;
} MemberInfoEuckr;

/* CRUD Operations */
int MemberInfoEuckr_Add(DBConnectionManager* manager, const MemberInfoEuckr* record);
int MemberInfoEuckr_Get(DBConnectionManager* manager, int m_id, MemberInfoEuckr* record);
int MemberInfoEuckr_Update(DBConnectionManager* manager, const MemberInfoEuckr* record);
int MemberInfoEuckr_Delete(DBConnectionManager* manager, int m_id);
int MemberInfoEuckr_Exists(DBConnectionManager* manager, int m_id);
int MemberInfoEuckr_GetAll(DBConnectionManager* manager, MemberInfoEuckr* records, int max_count, int* actual_count);

#endif /* MEMBER_INFO_EUCKR_ORM_H */