#ifndef MEMBER_MOUSEPASS_MODIFY_ORM_H
#define MEMBER_MOUSEPASS_MODIFY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    signed char modify_type;
} MemberMousepassModify;

/* CRUD Operations */
int MemberMousepassModify_Add(DBConnectionManager* manager, const MemberMousepassModify* record);
int MemberMousepassModify_Get(DBConnectionManager* manager, int m_id, const char* occ_time, MemberMousepassModify* record);
int MemberMousepassModify_Update(DBConnectionManager* manager, const MemberMousepassModify* record);
int MemberMousepassModify_Delete(DBConnectionManager* manager, int m_id, const char* occ_time);
int MemberMousepassModify_Exists(DBConnectionManager* manager, int m_id, const char* occ_time);
int MemberMousepassModify_GetAll(DBConnectionManager* manager, MemberMousepassModify* records, int max_count, int* actual_count);

#endif /* MEMBER_MOUSEPASS_MODIFY_ORM_H */