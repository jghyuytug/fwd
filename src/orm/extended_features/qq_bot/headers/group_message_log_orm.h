#ifndef GROUP_MESSAGE_LOG_ORM_H
#define GROUP_MESSAGE_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int QQGroupNumber;
    char QQGroupName[256];
    int SendQQNumber;
    char SendNickName[256];
    char QQMessage[1024];
    char CreatedAt[20];
} GroupMessageLog;

/* CRUD Operations */
int GroupMessageLog_Add(DBConnectionManager* manager, const GroupMessageLog* record);
int GroupMessageLog_Get(DBConnectionManager* manager, int Id, GroupMessageLog* record);
int GroupMessageLog_Update(DBConnectionManager* manager, const GroupMessageLog* record);
int GroupMessageLog_Delete(DBConnectionManager* manager, int Id);
int GroupMessageLog_Exists(DBConnectionManager* manager, int Id);
int GroupMessageLog_GetAll(DBConnectionManager* manager, GroupMessageLog* records, int max_count, int* actual_count);

#endif /* GROUP_MESSAGE_LOG_ORM_H */