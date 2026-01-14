#ifndef PU_USER_LIST_ORM_H
#define PU_USER_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
} PuUserList;

/* CRUD Operations */
int PuUserList_Add(DBConnectionManager* manager, const PuUserList* user);
int PuUserList_Get(DBConnectionManager* manager, unsigned int m_id, PuUserList* user);
int PuUserList_Update(DBConnectionManager* manager, const PuUserList* user);
int PuUserList_Delete(DBConnectionManager* manager, unsigned int m_id);
int PuUserList_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int PuUserList_GetAll(DBConnectionManager* manager, PuUserList* users,
                       int max_count, int* actual_count);

/* Utility Functions */
void PuUserList_PrintInfo(const PuUserList* user);

#endif
