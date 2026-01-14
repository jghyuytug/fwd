#ifndef MEMBER_PUNISH_INFO_HISTORY_2012_ORM_H
#define MEMBER_PUNISH_INFO_HISTORY_2012_ORM_H

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
} MemberPunishInfoHistory2012;

/* CRUD Operations */
int MemberPunishInfoHistory2012_Add(DBConnectionManager* manager,
                                     const MemberPunishInfoHistory2012* record);
int MemberPunishInfoHistory2012_Get(DBConnectionManager* manager,
                                     int no,
                                     MemberPunishInfoHistory2012* record);
int MemberPunishInfoHistory2012_Update(DBConnectionManager* manager,
                                        const MemberPunishInfoHistory2012* record);
int MemberPunishInfoHistory2012_Delete(DBConnectionManager* manager, int no);

/* Query Operations */
int MemberPunishInfoHistory2012_GetByMId(DBConnectionManager* manager,
                                          int m_id,
                                          MemberPunishInfoHistory2012* records,
                                          int max_count,
                                          int* actual_count);

int MemberPunishInfoHistory2012_GetByTimeRange(DBConnectionManager* manager,
                                                 const char* start_time,
                                                 const char* end_time,
                                                 MemberPunishInfoHistory2012* records,
                                                 int max_count,
                                                 int* actual_count);

int MemberPunishInfoHistory2012_GetByPunishType(DBConnectionManager* manager,
                                                  int punish_type,
                                                  MemberPunishInfoHistory2012* records,
                                                  int max_count,
                                                  int* actual_count);

int MemberPunishInfoHistory2012_CountByMId(DBConnectionManager* manager, int m_id);

/* Update Operations */
int MemberPunishInfoHistory2012_UpdateApplyFlag(DBConnectionManager* manager,
                                                  int no,
                                                  signed char apply_flag);

/* Utility */
void MemberPunishInfoHistory2012_PrintInfo(const MemberPunishInfoHistory2012* record);

#endif
