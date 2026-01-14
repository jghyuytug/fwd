#ifndef SIGN_IN_ITEM_INFO_ORM_H
#define SIGN_IN_ITEM_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int ItemCode;
    int MinNum;
    int MaxNum;
    int Probability;
    int PostalType;
} SignInItemInfo;

/* CRUD Operations */
int SignInItemInfo_Add(DBConnectionManager* manager, const SignInItemInfo* record);
int SignInItemInfo_Get(DBConnectionManager* manager, int Id, SignInItemInfo* record);
int SignInItemInfo_Update(DBConnectionManager* manager, const SignInItemInfo* record);
int SignInItemInfo_Delete(DBConnectionManager* manager, int Id);
int SignInItemInfo_Exists(DBConnectionManager* manager, int Id);
int SignInItemInfo_GetAll(DBConnectionManager* manager, SignInItemInfo* records, int max_count, int* actual_count);

#endif /* SIGN_IN_ITEM_INFO_ORM_H */