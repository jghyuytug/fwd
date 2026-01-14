#ifndef GROUP_MEMBER_JOIN_LOG_ORM_H
#define GROUP_MEMBER_JOIN_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    long long Uin;
    long long GroupCode;
    char NickName[256];
    long long JoinTime;
    short Level;
    char CreatedAt[20];
    long long JoinNumber;
    char UpdatedAt[20];
} GroupMemberJoinLog;

/* CRUD Operations */
int GroupMemberJoinLog_Add(DBConnectionManager* manager, const GroupMemberJoinLog* record);
int GroupMemberJoinLog_Get(DBConnectionManager* manager, int Id, GroupMemberJoinLog* record);
int GroupMemberJoinLog_Update(DBConnectionManager* manager, const GroupMemberJoinLog* record);
int GroupMemberJoinLog_Delete(DBConnectionManager* manager, int Id);
int GroupMemberJoinLog_Exists(DBConnectionManager* manager, int Id);
int GroupMemberJoinLog_GetAll(DBConnectionManager* manager, GroupMemberJoinLog* records, int max_count, int* actual_count);

#endif /* GROUP_MEMBER_JOIN_LOG_ORM_H */