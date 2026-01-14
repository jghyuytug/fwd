#ifndef MEMBER_SECURITY_CARD_HISTORY_ORM_H
#define MEMBER_SECURITY_CARD_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    signed char modify_type;
} MemberSecurityCardHistory;

/* CRUD Operations */
int MemberSecurityCardHistory_Add(DBConnectionManager* manager, const MemberSecurityCardHistory* history);
int MemberSecurityCardHistory_Get(DBConnectionManager* manager, const char* occ_time, int m_id,
                                   MemberSecurityCardHistory* history);
int MemberSecurityCardHistory_Delete(DBConnectionManager* manager, const char* occ_time, int m_id);
int MemberSecurityCardHistory_Exists(DBConnectionManager* manager, const char* occ_time, int m_id);

/* Business Operations */
int MemberSecurityCardHistory_GetByMember(DBConnectionManager* manager, int m_id,
                                           MemberSecurityCardHistory* histories,
                                           int max_count, int* actual_count);
int MemberSecurityCardHistory_GetByType(DBConnectionManager* manager, signed char modify_type,
                                         MemberSecurityCardHistory* histories,
                                         int max_count, int* actual_count);
int MemberSecurityCardHistory_GetRecent(DBConnectionManager* manager, int m_id,
                                         int limit, MemberSecurityCardHistory* histories,
                                         int* actual_count);

/* Utility */
void MemberSecurityCardHistory_PrintInfo(const MemberSecurityCardHistory* history);

#endif
