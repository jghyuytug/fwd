#ifndef MEMBER_PUNISH_INFO_HISTORY_2013_ORM_H
#define MEMBER_PUNISH_INFO_HISTORY_2013_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int punish_type;
    char occ_time[20];
    int punish_value;
    signed char apply_flag;
    char start_time[20];
    char end_time[20];
    char admin_id[26];
    char reason[101];
    signed char is_kicked;
    char first_ssn[33];
    char second_ssn[33];
} MemberPunishInfoHistory2013;

/* CRUD Operations */
int MemberPunishInfoHistory2013_Add(DBConnectionManager* manager, const MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Get(DBConnectionManager* manager, int no, MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Update(DBConnectionManager* manager, const MemberPunishInfoHistory2013* record);
int MemberPunishInfoHistory2013_Delete(DBConnectionManager* manager, int no);
int MemberPunishInfoHistory2013_Exists(DBConnectionManager* manager, int no);
int MemberPunishInfoHistory2013_GetAll(DBConnectionManager* manager, MemberPunishInfoHistory2013* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_INFO_HISTORY_2013_ORM_H */