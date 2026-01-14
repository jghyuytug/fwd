#ifndef ACCOUNT_CERASHOP_RESTRICT_ORM_H
#define ACCOUNT_CERASHOP_RESTRICT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int ipg_no;
    int count;
    int next_date;
    int end_date;
    int last_access_date;
} AccountCerashopRestrict;

/* CRUD Operations */
int AccountCerashopRestrict_Add(DBConnectionManager* manager, const AccountCerashopRestrict* record);
int AccountCerashopRestrict_Get(DBConnectionManager* manager, int m_id, int ipg_no, AccountCerashopRestrict* record);
int AccountCerashopRestrict_Update(DBConnectionManager* manager, const AccountCerashopRestrict* record);
int AccountCerashopRestrict_Delete(DBConnectionManager* manager, int m_id, int ipg_no);
int AccountCerashopRestrict_Exists(DBConnectionManager* manager, int m_id, int ipg_no);
int AccountCerashopRestrict_GetAll(DBConnectionManager* manager, AccountCerashopRestrict* records, int max_count, int* actual_count);

#endif /* ACCOUNT_CERASHOP_RESTRICT_ORM_H */