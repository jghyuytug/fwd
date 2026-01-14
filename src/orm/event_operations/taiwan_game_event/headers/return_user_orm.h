#ifndef RETURN_USER_ORM_H
#define RETURN_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char expire_time[20];
    signed char first_login;
} ReturnUser;

/* CRUD Operations */
int ReturnUser_Add(DBConnectionManager* manager, const ReturnUser* record);
int ReturnUser_Get(DBConnectionManager* manager, int m_id, ReturnUser* record);
int ReturnUser_Update(DBConnectionManager* manager, const ReturnUser* record);
int ReturnUser_Delete(DBConnectionManager* manager, int m_id);
int ReturnUser_Exists(DBConnectionManager* manager, int m_id);
int ReturnUser_GetAll(DBConnectionManager* manager, ReturnUser* records, int max_count, int* actual_count);

#endif /* RETURN_USER_ORM_H */