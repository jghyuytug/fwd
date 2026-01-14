#ifndef TING_USER_ACCOUNT_ORM_H
#define TING_USER_ACCOUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    signed char minute;
} TingUserAccount;

/* CRUD Operations */
int TingUserAccount_Add(DBConnectionManager* manager, const TingUserAccount* record);
int TingUserAccount_GetAll(DBConnectionManager* manager, TingUserAccount* records, int max_count, int* actual_count);

#endif /* TING_USER_ACCOUNT_ORM_H */