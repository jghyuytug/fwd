#ifndef KNOWCAFE_TIP_SEARCH_ORM_H
#define KNOWCAFE_TIP_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} KnowcafeTipSearch;

/* CRUD Operations */
int KnowcafeTipSearch_Add(DBConnectionManager* manager, const KnowcafeTipSearch* record);
int KnowcafeTipSearch_GetAll(DBConnectionManager* manager, KnowcafeTipSearch* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_TIP_SEARCH_ORM_H */