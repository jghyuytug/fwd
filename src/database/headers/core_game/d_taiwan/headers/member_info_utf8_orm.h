#ifndef MEMBER_INFO_UTF8_ORM_H
#define MEMBER_INFO_UTF8_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;                    /* Member ID (primary key) */
    char user_id[31];            /* User ID (VARCHAR(30) -> 31) */
    char user_name[11];          /* User name (VARCHAR(10) -> 11) */
    char first_ssn[7];           /* First SSN (VARCHAR(6) -> 7) */
    char second_ssn[8];          /* Second SSN (VARCHAR(7) -> 8) */
    char passwd[33];             /* Password (VARCHAR(32) -> 33) */
    char mobile_no[16];          /* Mobile number (VARCHAR(15) -> 16) */
    unsigned int reg_date;       /* Registration date (timestamp as int) */
    char email[51];              /* Email (VARCHAR(50) -> 51) */
    unsigned char q_no;          /* Question number */
    char q_answer[31];           /* Question answer (VARCHAR(30) -> 31) */
    char updt_date[20];          /* Update timestamp */
    char state;                  /* Account state */
    char nickname[17];           /* Nickname (VARCHAR(16) -> 17) */
    char email_yn;               /* Email subscription (ENUM: 'y' or 'n') */
    unsigned char ssn_check;     /* SSN check status */
    unsigned int slot;           /* Character slot count */
    char last_play_time[20];     /* Last play time (DATETIME) */
    char hangame_flag;           /* Hangame flag */
    char hanmon_flag;            /* Hanmon flag */
} MemberInfoUtf8;

/* CRUD Operations */
int MemberInfoUtf8_Add(DBConnectionManager* manager, const MemberInfoUtf8* member);
int MemberInfoUtf8_Get(DBConnectionManager* manager, int m_id, MemberInfoUtf8* member);
int MemberInfoUtf8_Update(DBConnectionManager* manager, const MemberInfoUtf8* member);
int MemberInfoUtf8_Delete(DBConnectionManager* manager, int m_id);
int MemberInfoUtf8_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int MemberInfoUtf8_GetAll(DBConnectionManager* manager, MemberInfoUtf8* members,
                           int max_count, int* actual_count);
int MemberInfoUtf8_GetByUserId(DBConnectionManager* manager, const char* user_id,
                                MemberInfoUtf8* member);
int MemberInfoUtf8_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                  MemberInfoUtf8* member);
int MemberInfoUtf8_UpdatePassword(DBConnectionManager* manager, int m_id,
                                   const char* new_passwd);
int MemberInfoUtf8_UpdateSlot(DBConnectionManager* manager, int m_id,
                               unsigned int new_slot);

/* Utility Functions */
void MemberInfoUtf8_PrintInfo(const MemberInfoUtf8* member);

#endif /* MEMBER_INFO_UTF8_ORM_H */

