#ifndef PSWD_QSTION_DIRECT_ORM_H
#define PSWD_QSTION_DIRECT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char q_text[21];
} PswdQstionDirect;

/* CRUD Operations */
int PswdQstionDirect_Add(DBConnectionManager* manager, const PswdQstionDirect* question);
int PswdQstionDirect_Get(DBConnectionManager* manager, int m_id, PswdQstionDirect* question);
int PswdQstionDirect_Update(DBConnectionManager* manager, const PswdQstionDirect* question);
int PswdQstionDirect_Delete(DBConnectionManager* manager, int m_id);
int PswdQstionDirect_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int PswdQstionDirect_GetByTextPattern(DBConnectionManager* manager, const char* pattern,
                                       PswdQstionDirect* questions, int max_count, int* actual_count);
int PswdQstionDirect_GetAll(DBConnectionManager* manager, PswdQstionDirect* questions,
                              int max_count, int* actual_count);
int PswdQstionDirect_Count(DBConnectionManager* manager, int* total_count);

/* Utility Functions */
void PswdQstionDirect_PrintInfo(const PswdQstionDirect* question);

#endif
