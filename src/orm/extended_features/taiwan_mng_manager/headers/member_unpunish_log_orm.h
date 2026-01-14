#ifndef MEMBER_UNPUNISH_LOG_ORM_H
#define MEMBER_UNPUNISH_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    int admin_no;
    char admin_id[21];
    char total_m_id[1024];
    char success_m_id[1024];
    char fail_m_id[1024];
    char reg_date[20];
    char reason[1024];
} MemberUnpunishLog;

/* CRUD Operations */
int MemberUnpunishLog_Add(DBConnectionManager* manager, const MemberUnpunishLog* record);
int MemberUnpunishLog_Get(DBConnectionManager* manager, int idx, MemberUnpunishLog* record);
int MemberUnpunishLog_Update(DBConnectionManager* manager, const MemberUnpunishLog* record);
int MemberUnpunishLog_Delete(DBConnectionManager* manager, int idx);
int MemberUnpunishLog_Exists(DBConnectionManager* manager, int idx);
int MemberUnpunishLog_GetAll(DBConnectionManager* manager, MemberUnpunishLog* records, int max_count, int* actual_count);

#endif /* MEMBER_UNPUNISH_LOG_ORM_H */