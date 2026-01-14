#ifndef MEMBER_PREMIUM_HISTORY_ORM_H
#define MEMBER_PREMIUM_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    signed char pre_type;
    int m_id;
    char service_start[20];
    char service_end[20];
} MemberPremiumHistory;

/* CRUD Operations */
int MemberPremiumHistory_Add(DBConnectionManager* manager, const MemberPremiumHistory* record);
int MemberPremiumHistory_Get(DBConnectionManager* manager, int event_id, signed char pre_type, int m_id, const char* service_start, MemberPremiumHistory* record);
int MemberPremiumHistory_Update(DBConnectionManager* manager, const MemberPremiumHistory* record);
int MemberPremiumHistory_Delete(DBConnectionManager* manager, int event_id, signed char pre_type, int m_id, const char* service_start);
int MemberPremiumHistory_Exists(DBConnectionManager* manager, int event_id, signed char pre_type, int m_id, const char* service_start);
int MemberPremiumHistory_GetAll(DBConnectionManager* manager, MemberPremiumHistory* records, int max_count, int* actual_count);

#endif /* MEMBER_PREMIUM_HISTORY_ORM_H */