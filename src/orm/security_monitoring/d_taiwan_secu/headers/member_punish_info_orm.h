#ifndef MEMBER_PUNISH_INFO_ORM_H
#define MEMBER_PUNISH_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int punish_type;
    char occ_time[20];
    int punish_value;
    signed char apply_flag;
    char start_time[20];
    char end_time[20];
    char admin_id[26];
    char reason[101];
} MemberPunishInfo;

/* CRUD Operations */
int MemberPunishInfo_Add(DBConnectionManager* manager, const MemberPunishInfo* record);
int MemberPunishInfo_Get(DBConnectionManager* manager, int m_id, int punish_type, MemberPunishInfo* record);
int MemberPunishInfo_Update(DBConnectionManager* manager, const MemberPunishInfo* record);
int MemberPunishInfo_Delete(DBConnectionManager* manager, int m_id, int punish_type);
int MemberPunishInfo_Exists(DBConnectionManager* manager, int m_id, int punish_type);
int MemberPunishInfo_GetAll(DBConnectionManager* manager, MemberPunishInfo* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_INFO_ORM_H */