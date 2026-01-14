#ifndef MEMBER_SAFE_ENSURE_HISTORY_ORM_H
#define MEMBER_SAFE_ENSURE_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char mod_flag;
    char mod_time[20];
    int m_id;
    char occ_time[20];
    char mobile_no[16];
    signed char service_flag;
    signed char type1_flag;
    signed char type2_flag;
    char expire_time[20];
    char settle_id[19];
} MemberSafeEnsureHistory;

/* CRUD Operations */
int MemberSafeEnsureHistory_Add(DBConnectionManager* manager, const MemberSafeEnsureHistory* record);
int MemberSafeEnsureHistory_GetAll(DBConnectionManager* manager, MemberSafeEnsureHistory* records, int max_count, int* actual_count);

#endif /* MEMBER_SAFE_ENSURE_HISTORY_ORM_H */