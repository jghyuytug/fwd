#ifndef GROUP_MEMBER_BIND_ORM_H
#define GROUP_MEMBER_BIND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long Uin;
    int CharacId;
    int Uid;
    long long GroupCode;
    char CreatedAt[20];
    char UpdatedAt[20];
} GroupMemberBind;

/* CRUD Operations */
int GroupMemberBind_Add(DBConnectionManager* manager, const GroupMemberBind* record);
int GroupMemberBind_Get(DBConnectionManager* manager, long long Uin, GroupMemberBind* record);
int GroupMemberBind_Update(DBConnectionManager* manager, const GroupMemberBind* record);
int GroupMemberBind_Delete(DBConnectionManager* manager, long long Uin);
int GroupMemberBind_Exists(DBConnectionManager* manager, long long Uin);
int GroupMemberBind_GetAll(DBConnectionManager* manager, GroupMemberBind* records, int max_count, int* actual_count);

#endif /* GROUP_MEMBER_BIND_ORM_H */