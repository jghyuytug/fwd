#ifndef MEMBER_PUNISH_INFO_HISTORY_2013_ORM_H
#define MEMBER_PUNISH_INFO_HISTORY_2013_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;                  /* Auto-increment primary key */
    int m_id;                /* Member ID */
    int punish_type;         /* Punishment type */
    char occ_time[20];       /* Occurrence time (DATETIME) */
    int punish_value;        /* Punishment value */
    signed char apply_flag;  /* Apply flag */
    char start_time[20];     /* Start time (DATETIME) */
    char end_time[20];       /* End time (DATETIME) */
    char admin_id[26];       /* Admin ID (VARCHAR(25) -> 26) */
    char reason[101];        /* Reason (VARCHAR(100) -> 101) */
    signed char is_kicked;   /* Is kicked flag */
    char first_ssn[33];      /* First SSN (VARCHAR(32) -> 33) */
    char second_ssn[33];     /* Second SSN (VARCHAR(32) -> 33) */
} MemberPunishInfoHistory2013;

/* CRUD Operations */
int MemberPunishInfoHistory2013_Add(DBConnectionManager* manager,
                                     const MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Get(DBConnectionManager* manager,
                                     int no,
                                     MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Update(DBConnectionManager* manager,
                                        const MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Delete(DBConnectionManager* manager, int no);

/* Query Operations */
int MemberPunishInfoHistory2013_GetByMId(DBConnectionManager* manager,
                                          int m_id,
                                          MemberPunishInfoHistory2013* records,
                                          int max_count,
                                          int* actual_count);

int MemberPunishInfoHistory2013_GetByTimeRange(DBConnectionManager* manager,
                                                 const char* start_time,
                                                 const char* end_time,
                                                 MemberPunishInfoHistory2013* records,
                                                 int max_count,
                                                 int* actual_count);

int MemberPunishInfoHistory2013_GetByPunishType(DBConnectionManager* manager,
                                                  int punish_type,
                                                  MemberPunishInfoHistory2013* records,
                                                  int max_count,
                                                  int* actual_count);

int MemberPunishInfoHistory2013_CountByMId(DBConnectionManager* manager, int m_id);

/* Update Operations */
int MemberPunishInfoHistory2013_UpdateApplyFlag(DBConnectionManager* manager,
                                                  int no,
                                                  signed char apply_flag);

/* Utility */
void MemberPunishInfoHistory2013_PrintInfo(const MemberPunishInfoHistory2013* record);

#endif
