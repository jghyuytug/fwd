#ifndef ALLOW_PROXY_USER_ORM_H
#define ALLOW_PROXY_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
} AllowProxyUser;

/* CRUD Operations */
int AllowProxyUser_Add(DBConnectionManager* manager, unsigned int m_id);
int AllowProxyUser_Delete(DBConnectionManager* manager, unsigned int m_id);
int AllowProxyUser_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int AllowProxyUser_GetAll(DBConnectionManager* manager, AllowProxyUser* users,
                          int max_count, int* actual_count);
int AllowProxyUser_Count(DBConnectionManager* manager);
int AllowProxyUser_DeleteAll(DBConnectionManager* manager);

/* Utility Functions */
void AllowProxyUser_PrintInfo(const AllowProxyUser* user);

#endif
