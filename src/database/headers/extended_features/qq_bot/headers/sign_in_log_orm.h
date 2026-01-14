#ifndef SIGN_IN_LOG_ORM_H
#define SIGN_IN_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    long long Uin;
    int CharacId;
    int Uid;
    long long GroupCode;
    int ItemCount;
    char ItemName[256];
    char CreatedAt[20];
} SignInLog;

/* CRUD Operations */
int SignInLog_Add(DBConnectionManager* manager, const SignInLog* record);
int SignInLog_Get(DBConnectionManager* manager, int Id, SignInLog* record);
int SignInLog_Update(DBConnectionManager* manager, const SignInLog* record);
int SignInLog_Delete(DBConnectionManager* manager, int Id);
int SignInLog_Exists(DBConnectionManager* manager, int Id);
int SignInLog_GetAll(DBConnectionManager* manager, SignInLog* records, int max_count, int* actual_count);

#endif /* SIGN_IN_LOG_ORM_H */