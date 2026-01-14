#ifndef PSWD_QSTION_DIRECT_ORM_H
#define PSWD_QSTION_DIRECT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char q_text[21];
} PswdQstionDirect;

/* CRUD Operations */
int PswdQstionDirect_Add(DBConnectionManager* manager, const PswdQstionDirect* record);
int PswdQstionDirect_Get(DBConnectionManager* manager, int m_id, PswdQstionDirect* record);
int PswdQstionDirect_Update(DBConnectionManager* manager, const PswdQstionDirect* record);
int PswdQstionDirect_Delete(DBConnectionManager* manager, int m_id);
int PswdQstionDirect_Exists(DBConnectionManager* manager, int m_id);
int PswdQstionDirect_GetAll(DBConnectionManager* manager, PswdQstionDirect* records, int max_count, int* actual_count);

#endif /* PSWD_QSTION_DIRECT_ORM_H */