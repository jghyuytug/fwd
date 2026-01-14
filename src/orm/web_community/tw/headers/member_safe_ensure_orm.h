#ifndef MEMBER_SAFE_ENSURE_ORM_H
#define MEMBER_SAFE_ENSURE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    char mobile_no[16];
    signed char service_flag;
    signed char type1_flag;
    signed char type2_flag;
    char expire_time[20];
    char settle_id[19];
} MemberSafeEnsure;

/* CRUD Operations */
int MemberSafeEnsure_Add(DBConnectionManager* manager, const MemberSafeEnsure* record);
int MemberSafeEnsure_GetAll(DBConnectionManager* manager, MemberSafeEnsure* records, int max_count, int* actual_count);

#endif /* MEMBER_SAFE_ENSURE_ORM_H */