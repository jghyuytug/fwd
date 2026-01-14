#ifndef MEMBER_PUNISH_INFO_ARS_MAIL_ORM_H
#define MEMBER_PUNISH_INFO_ARS_MAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int punish_type;
    signed char apply_flag;
    char occ_time[20];
    char send_time[20];
} MemberPunishInfoArsMail;

/* CRUD Operations */
int MemberPunishInfoArsMail_Add(DBConnectionManager* manager, const MemberPunishInfoArsMail* record);
int MemberPunishInfoArsMail_Get(DBConnectionManager* manager, int no, MemberPunishInfoArsMail* record);
int MemberPunishInfoArsMail_Update(DBConnectionManager* manager, const MemberPunishInfoArsMail* record);
int MemberPunishInfoArsMail_Delete(DBConnectionManager* manager, int no);
int MemberPunishInfoArsMail_Exists(DBConnectionManager* manager, int no);
int MemberPunishInfoArsMail_GetAll(DBConnectionManager* manager, MemberPunishInfoArsMail* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_INFO_ARS_MAIL_ORM_H */