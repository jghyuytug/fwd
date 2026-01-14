#ifndef MEMBER_WHITE_ACCOUNT_ORM_H
#define MEMBER_WHITE_ACCOUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char reg_date[20];
} MemberWhiteAccount;

/* CRUD Operations */
int MemberWhiteAccount_Add(DBConnectionManager* manager, const MemberWhiteAccount* record);
int MemberWhiteAccount_Get(DBConnectionManager* manager, int m_id, MemberWhiteAccount* record);
int MemberWhiteAccount_Update(DBConnectionManager* manager, const MemberWhiteAccount* record);
int MemberWhiteAccount_Delete(DBConnectionManager* manager, int m_id);
int MemberWhiteAccount_Exists(DBConnectionManager* manager, int m_id);
int MemberWhiteAccount_GetAll(DBConnectionManager* manager, MemberWhiteAccount* records, int max_count, int* actual_count);

#endif /* MEMBER_WHITE_ACCOUNT_ORM_H */