#ifndef MEMBER_INFO_MILEAGE_ORM_H
#define MEMBER_INFO_MILEAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;          /* Member ID (auto_increment primary key) */
    char user_id[31];           /* User ID (varchar 30 + 1) */
    char user_name[11];         /* User name (varchar 10 + 1) */
    char first_ssn[7];          /* First part of SSN (varchar 6 + 1) */
    char second_ssn[8];         /* Second part of SSN (varchar 7 + 1) */
    char passwd[33];            /* Password hash (varchar 32 + 1) */
    char mobile_no[16];         /* Mobile number (varchar 15 + 1) */
    unsigned int reg_date;      /* Registration date (Unix timestamp) */
    char email[51];             /* Email address (varchar 50 + 1) */
    unsigned char q_no;         /* Security question number */
    char q_answer[31];          /* Security question answer (varchar 30 + 1) */
    char updt_date[20];         /* Last update timestamp (datetime string) */
    char state;                 /* Account state (1=active, etc.) */
    char nickname[17];          /* Nickname (varchar 16 + 1) */
    char email_yn;              /* Email notification (y/n) */
    unsigned char ssn_check;    /* SSN verification status */
    unsigned int slot;          /* Character slots */
    char last_play_time[20];    /* Last play time (datetime string) */
    char hangame_flag;          /* Hangame integration flag */
    char hanmon_flag;           /* Hanmon integration flag */
    int mileage;                /* Mileage points */
} MemberInfoMileage;

/* CRUD Operations */
int MemberInfoMileage_Add(DBConnectionManager* manager, const MemberInfoMileage* member);
int MemberInfoMileage_Get(DBConnectionManager* manager, unsigned int m_id, MemberInfoMileage* member);
int MemberInfoMileage_Update(DBConnectionManager* manager, const MemberInfoMileage* member);
int MemberInfoMileage_Delete(DBConnectionManager* manager, unsigned int m_id);
int MemberInfoMileage_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int MemberInfoMileage_GetByUserId(DBConnectionManager* manager, const char* user_id,
                                   MemberInfoMileage* member);
int MemberInfoMileage_GetByNickname(DBConnectionManager* manager, const char* nickname,
                                     MemberInfoMileage* member);
int MemberInfoMileage_UpdateMileage(DBConnectionManager* manager, unsigned int m_id,
                                     int mileage_delta);
int MemberInfoMileage_UpdatePassword(DBConnectionManager* manager, unsigned int m_id,
                                      const char* new_passwd);
int MemberInfoMileage_UpdateEmail(DBConnectionManager* manager, unsigned int m_id,
                                   const char* new_email);
int MemberInfoMileage_UpdateLastPlayTime(DBConnectionManager* manager, unsigned int m_id);
int MemberInfoMileage_CountByState(DBConnectionManager* manager, char state, int* count);

/* Utility Functions */
void MemberInfoMileage_PrintInfo(const MemberInfoMileage* member);

#endif
