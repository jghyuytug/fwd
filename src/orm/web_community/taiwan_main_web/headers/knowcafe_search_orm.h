#ifndef KNOWCAFE_SEARCH_ORM_H
#define KNOWCAFE_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char knowcafe_type;
    int no;
    signed char category;
    char class;
    char word[33];
} KnowcafeSearch;

/* CRUD Operations */
int KnowcafeSearch_Add(DBConnectionManager* manager, const KnowcafeSearch* record);
int KnowcafeSearch_GetAll(DBConnectionManager* manager, KnowcafeSearch* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_SEARCH_ORM_H */