#ifndef KNOWCAFE_QA_SEARCH_BAK_ORM_H
#define KNOWCAFE_QA_SEARCH_BAK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
    signed char depth;
} KnowcafeQaSearchBak;

/* CRUD Operations */
int KnowcafeQaSearchBak_Add(DBConnectionManager* manager, const KnowcafeQaSearchBak* record);
int KnowcafeQaSearchBak_GetAll(DBConnectionManager* manager, KnowcafeQaSearchBak* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_SEARCH_BAK_ORM_H */