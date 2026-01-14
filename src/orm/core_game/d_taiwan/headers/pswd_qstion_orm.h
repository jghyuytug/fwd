#ifndef PSWD_QSTION_ORM_H
#define PSWD_QSTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char q_no;
    char q_text[21];
} PswdQstion;

/* CRUD Operations */
int PswdQstion_Add(DBConnectionManager* manager, const PswdQstion* question);
int PswdQstion_Get(DBConnectionManager* manager, unsigned char q_no, PswdQstion* question);
int PswdQstion_Update(DBConnectionManager* manager, const PswdQstion* question);
int PswdQstion_Delete(DBConnectionManager* manager, unsigned char q_no);
int PswdQstion_Exists(DBConnectionManager* manager, unsigned char q_no);

/* Business Operations */
int PswdQstion_GetAll(DBConnectionManager* manager, PswdQstion* questions,
                       int max_count, int* actual_count);
int PswdQstion_GetByTextPattern(DBConnectionManager* manager, const char* pattern,
                                 PswdQstion* questions, int max_count, int* actual_count);
int PswdQstion_Count(DBConnectionManager* manager, int* total_count);

/* Utility Functions */
void PswdQstion_PrintInfo(const PswdQstion* question);

#endif
