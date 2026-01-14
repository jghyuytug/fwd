#ifndef LOGIN_ACCOUNT_ORM_H
#define LOGIN_ACCOUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int m_channel_no;
    signed char login_status;
    char last_login_date[20];
    char login_ip[16];
} LoginAccount;

/* CRUD Operations */
int LoginAccount_Add(DBConnectionManager* manager, const LoginAccount* record);
int LoginAccount_Get(DBConnectionManager* manager, int m_id, LoginAccount* record);
int LoginAccount_Update(DBConnectionManager* manager, const LoginAccount* record);
int LoginAccount_Delete(DBConnectionManager* manager, int m_id);
int LoginAccount_Exists(DBConnectionManager* manager, int m_id);
int LoginAccount_GetAll(DBConnectionManager* manager, LoginAccount* records, int max_count, int* actual_count);

#endif /* LOGIN_ACCOUNT_ORM_H */