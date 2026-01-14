#ifndef OPENCAST_MEMBER_INFO_ORM_H
#define OPENCAST_MEMBER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char sub_type;
} OpencastMemberInfo;

/* CRUD Operations */
int OpencastMemberInfo_Add(DBConnectionManager* manager, const OpencastMemberInfo* record);
int OpencastMemberInfo_Get(DBConnectionManager* manager, int m_id, OpencastMemberInfo* record);
int OpencastMemberInfo_Update(DBConnectionManager* manager, const OpencastMemberInfo* record);
int OpencastMemberInfo_Delete(DBConnectionManager* manager, int m_id);
int OpencastMemberInfo_Exists(DBConnectionManager* manager, int m_id);
int OpencastMemberInfo_GetAll(DBConnectionManager* manager, OpencastMemberInfo* records, int max_count, int* actual_count);

#endif /* OPENCAST_MEMBER_INFO_ORM_H */