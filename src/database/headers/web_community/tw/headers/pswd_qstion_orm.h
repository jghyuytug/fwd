#ifndef PSWD_QSTION_ORM_H
#define PSWD_QSTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char q_no;
    char q_text[21];
} PswdQstion;

/* CRUD Operations */
int PswdQstion_Add(DBConnectionManager* manager, const PswdQstion* record);
int PswdQstion_Get(DBConnectionManager* manager, signed char q_no, PswdQstion* record);
int PswdQstion_Update(DBConnectionManager* manager, const PswdQstion* record);
int PswdQstion_Delete(DBConnectionManager* manager, signed char q_no);
int PswdQstion_Exists(DBConnectionManager* manager, signed char q_no);
int PswdQstion_GetAll(DBConnectionManager* manager, PswdQstion* records, int max_count, int* actual_count);

#endif /* PSWD_QSTION_ORM_H */