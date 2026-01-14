#ifndef PU_USER_LIST_ORM_H
#define PU_USER_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
} PuUserList;

/* CRUD Operations */
int PuUserList_Add(DBConnectionManager* manager, const PuUserList* record);
int PuUserList_Get(DBConnectionManager* manager, int m_id, PuUserList* record);
int PuUserList_Update(DBConnectionManager* manager, const PuUserList* record);
int PuUserList_Delete(DBConnectionManager* manager, int m_id);
int PuUserList_Exists(DBConnectionManager* manager, int m_id);
int PuUserList_GetAll(DBConnectionManager* manager, PuUserList* records, int max_count, int* actual_count);

#endif /* PU_USER_LIST_ORM_H */