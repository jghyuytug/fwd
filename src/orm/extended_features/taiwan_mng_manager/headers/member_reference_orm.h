#ifndef MEMBER_REFERENCE_ORM_H
#define MEMBER_REFERENCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char reference[251];
} MemberReference;

/* CRUD Operations */
int MemberReference_Add(DBConnectionManager* manager, const MemberReference* record);
int MemberReference_Get(DBConnectionManager* manager, int m_id, MemberReference* record);
int MemberReference_Update(DBConnectionManager* manager, const MemberReference* record);
int MemberReference_Delete(DBConnectionManager* manager, int m_id);
int MemberReference_Exists(DBConnectionManager* manager, int m_id);
int MemberReference_GetAll(DBConnectionManager* manager, MemberReference* records, int max_count, int* actual_count);

#endif /* MEMBER_REFERENCE_ORM_H */