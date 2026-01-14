#ifndef KNOWCAFE_PLAN_SEARCH_ORM_H
#define KNOWCAFE_PLAN_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} KnowcafePlanSearch;

/* CRUD Operations */
int KnowcafePlanSearch_Add(DBConnectionManager* manager, const KnowcafePlanSearch* record);
int KnowcafePlanSearch_GetAll(DBConnectionManager* manager, KnowcafePlanSearch* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_PLAN_SEARCH_ORM_H */