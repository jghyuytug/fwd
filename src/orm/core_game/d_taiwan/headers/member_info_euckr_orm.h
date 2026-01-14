#ifndef MEMBER_INFO_EUCKR_ORM_H
#define MEMBER_INFO_EUCKR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    char user_id[31];           /* VARCHAR(30) -> 31 */
    char user_name[11];         /* VARCHAR(10) -> 11 */
    char first_ssn[7];          /* VARCHAR(6) -> 7 */
    char second_ssn[8];         /* VARCHAR(7) -> 8 */
    char passwd[33];            /* VARCHAR(32) -> 33 */
    char mobile_no[16];         /* VARCHAR(15) -> 16 */
    int reg_date;
    char email[51];             /* VARCHAR(50) -> 51 */
    unsigned char q_no;         /* tinyint(4) */
    char q_answer[31];          /* VARCHAR(30) -> 31 */
    char updt_date[20];         /* TIMESTAMP */
    unsigned char state;        /* tinyint(4) */
    char nickname[17];          /* VARCHAR(16) -> 17 */
    char email_yn;              /* ENUM('y','n') */
    unsigned char ssn_check;    /* tinyint(3) unsigned */
    unsigned int slot;          /* int(10) unsigned */
    char last_play_time[20];    /* DATETIME */
    unsigned char hangame_flag; /* tinyint(4) */
} MemberInfoEuckr;

/* CRUD Operations */
int MemberInfoEuckr_Add(DBConnectionManager* manager, const MemberInfoEuckr* member);
int MemberInfoEuckr_Get(DBConnectionManager* manager, unsigned int m_id, MemberInfoEuckr* member);
int MemberInfoEuckr_Update(DBConnectionManager* manager, const MemberInfoEuckr* member);
int MemberInfoEuckr_Delete(DBConnectionManager* manager, unsigned int m_id);
int MemberInfoEuckr_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int MemberInfoEuckr_GetByUserId(DBConnectionManager* manager, const char* user_id, MemberInfoEuckr* member);
int MemberInfoEuckr_GetByNickname(DBConnectionManager* manager, const char* nickname, MemberInfoEuckr* member);
int MemberInfoEuckr_GetBySSN(DBConnectionManager* manager, const char* first_ssn, const char* second_ssn, MemberInfoEuckr* member);
int MemberInfoEuckr_UpdatePassword(DBConnectionManager* manager, unsigned int m_id, const char* new_passwd);
int MemberInfoEuckr_UpdateEmail(DBConnectionManager* manager, unsigned int m_id, const char* new_email);
int MemberInfoEuckr_UpdateState(DBConnectionManager* manager, unsigned int m_id, unsigned char state);
int MemberInfoEuckr_UpdateLastPlayTime(DBConnectionManager* manager, unsigned int m_id, const char* play_time);
int MemberInfoEuckr_GetRecentMembers(DBConnectionManager* manager, int limit,
                                      MemberInfoEuckr* members, int max_count, int* actual_count);

/* Utility Functions */
void MemberInfoEuckr_PrintInfo(const MemberInfoEuckr* member);

#endif /* MEMBER_INFO_EUCKR_ORM_H */
