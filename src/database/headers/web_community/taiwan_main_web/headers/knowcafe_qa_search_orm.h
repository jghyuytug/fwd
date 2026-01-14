#ifndef KNOWCAFE_QA_SEARCH_ORM_H
#define KNOWCAFE_QA_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
    signed char depth;
} KnowcafeQaSearch;

/* CRUD Operations */
int KnowcafeQaSearch_Add(DBConnectionManager* manager, const KnowcafeQaSearch* record);
int KnowcafeQaSearch_GetAll(DBConnectionManager* manager, KnowcafeQaSearch* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_SEARCH_ORM_H */