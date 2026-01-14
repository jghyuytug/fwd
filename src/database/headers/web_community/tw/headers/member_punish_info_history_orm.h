#ifndef MEMBER_PUNISH_INFO_HISTORY_ORM_H
#define MEMBER_PUNISH_INFO_HISTORY_ORM_H

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
} MemberPunishInfoHistory;

/* CRUD Operations */
int MemberPunishInfoHistory_Add(DBConnectionManager* manager, const MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Get(DBConnectionManager* manager, int no, MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Update(DBConnectionManager* manager, const MemberPunishInfoHistory* record);
int MemberPunishInfoHistory_Delete(DBConnectionManager* manager, int no);
int MemberPunishInfoHistory_Exists(DBConnectionManager* manager, int no);
int MemberPunishInfoHistory_GetAll(DBConnectionManager* manager, MemberPunishInfoHistory* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_INFO_HISTORY_ORM_H */