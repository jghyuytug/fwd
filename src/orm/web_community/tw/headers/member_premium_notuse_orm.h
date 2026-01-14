#ifndef MEMBER_PREMIUM_NOTUSE_ORM_H
#define MEMBER_PREMIUM_NOTUSE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    signed char pre_type;
    int m_id;
    char service_start[20];
    char service_end[20];
    signed char server_id;
} MemberPremiumNotuse;

/* CRUD Operations */
int MemberPremiumNotuse_Add(DBConnectionManager* manager, const MemberPremiumNotuse* record);
int MemberPremiumNotuse_Get(DBConnectionManager* manager, int event_id, signed char pre_type, signed char server_id, int m_id, const char* service_start, MemberPremiumNotuse* record);
int MemberPremiumNotuse_Update(DBConnectionManager* manager, const MemberPremiumNotuse* record);
int MemberPremiumNotuse_Delete(DBConnectionManager* manager, int event_id, signed char pre_type, signed char server_id, int m_id, const char* service_start);
int MemberPremiumNotuse_Exists(DBConnectionManager* manager, int event_id, signed char pre_type, signed char server_id, int m_id, const char* service_start);
int MemberPremiumNotuse_GetAll(DBConnectionManager* manager, MemberPremiumNotuse* records, int max_count, int* actual_count);

#endif /* MEMBER_PREMIUM_NOTUSE_ORM_H */