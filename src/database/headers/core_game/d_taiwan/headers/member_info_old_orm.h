#ifndef MEMBER_INFO_OLD_ORM_H
#define MEMBER_INFO_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;          /* Member ID (primary key, auto_increment) */
    char user_id[16];           /* User ID (VARCHAR(15) -> 16) */
    char user_name[6];          /* User name (VARCHAR(5) -> 6) */
    char first_ssn[11];         /* First SSN (VARCHAR(10) -> 11) */
    char second_ssn[11];        /* Second SSN (VARCHAR(10) -> 11) */
    char passwd[9];             /* Password (VARCHAR(8) -> 9) */
    char mobile_no[9];          /* Mobile number (VARCHAR(8) -> 9) */
    unsigned int reg_date;      /* Registration date (timestamp as int) */
    char email[26];             /* Email (VARCHAR(25) -> 26) */
    unsigned char q_no;         /* Question number */
    char q_answer[16];          /* Question answer (VARCHAR(15) -> 16) */
    char updt_date[20];         /* Update timestamp (YYYY-MM-DD HH:MM:SS) */
    char state;                 /* Account state */
    char nickname[9];           /* Nickname (VARCHAR(8) -> 9) */
    char email_yn;              /* Email subscription (ENUM: 'y' or 'n') */
    unsigned char ssn_check;    /* SSN check status */
    char last_play_time[20];    /* Last play time (DATETIME) */
} MemberInfoOld;

/* CRUD Operations */
int MemberInfoOld_Add(DBConnectionManager* manager, const MemberInfoOld* member);
int MemberInfoOld_Get(DBConnectionManager* manager, unsigned int m_id, MemberInfoOld* member);
int MemberInfoOld_Update(DBConnectionManager* manager, const MemberInfoOld* member);
int MemberInfoOld_Delete(DBConnectionManager* manager, unsigned int m_id);
int MemberInfoOld_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int MemberInfoOld_GetByUserId(DBConnectionManager* manager, const char* user_id,
                               MemberInfoOld* member);
int MemberInfoOld_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                 MemberInfoOld* member);
int MemberInfoOld_GetBySSN(DBConnectionManager* manager, const char* first_ssn,
                            const char* second_ssn, MemberInfoOld* member);
int MemberInfoOld_GetByState(DBConnectionManager* manager, char state,
                              MemberInfoOld* members, int max_count, int* actual_count);
int MemberInfoOld_UpdatePassword(DBConnectionManager* manager, unsigned int m_id,
                                  const char* new_passwd);
int MemberInfoOld_UpdateLastPlayTime(DBConnectionManager* manager, unsigned int m_id,
                                      const char* play_time);

/* Utility Functions */
void MemberInfoOld_PrintInfo(const MemberInfoOld* member);

#endif /* MEMBER_INFO_OLD_ORM_H */
