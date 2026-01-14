#ifndef PROD_BUY_USER_ORM_H
#define PROD_BUY_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char user_id[26];
    char sex;
    char birthday[11];
    char first_buy_time[20];
    char last_buy_time[20];
} ProdBuyUser;

/* CRUD Operations */
int ProdBuyUser_Add(DBConnectionManager* manager, const ProdBuyUser* record);
int ProdBuyUser_Get(DBConnectionManager* manager, int m_id, ProdBuyUser* record);
int ProdBuyUser_Update(DBConnectionManager* manager, const ProdBuyUser* record);
int ProdBuyUser_Delete(DBConnectionManager* manager, int m_id);
int ProdBuyUser_Exists(DBConnectionManager* manager, int m_id);
int ProdBuyUser_GetAll(DBConnectionManager* manager, ProdBuyUser* records, int max_count, int* actual_count);

#endif /* PROD_BUY_USER_ORM_H */