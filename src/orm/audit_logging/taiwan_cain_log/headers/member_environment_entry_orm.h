#ifndef MEMBER_ENVIRONMENT_ENTRY_ORM_H
#define MEMBER_ENVIRONMENT_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
} MemberEnvironmentEntry;

/* CRUD Operations */
int MemberEnvironmentEntry_Add(DBConnectionManager* manager, const MemberEnvironmentEntry* record);
int MemberEnvironmentEntry_Get(DBConnectionManager* manager, int m_id, MemberEnvironmentEntry* record);
int MemberEnvironmentEntry_Update(DBConnectionManager* manager, const MemberEnvironmentEntry* record);
int MemberEnvironmentEntry_Delete(DBConnectionManager* manager, int m_id);
int MemberEnvironmentEntry_Exists(DBConnectionManager* manager, int m_id);
int MemberEnvironmentEntry_GetAll(DBConnectionManager* manager, MemberEnvironmentEntry* records, int max_count, int* actual_count);

#endif /* MEMBER_ENVIRONMENT_ENTRY_ORM_H */