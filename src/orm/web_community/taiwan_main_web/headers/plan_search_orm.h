#ifndef PLAN_SEARCH_ORM_H
#define PLAN_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} PlanSearch;

/* CRUD Operations */
int PlanSearch_Add(DBConnectionManager* manager, const PlanSearch* record);
int PlanSearch_GetAll(DBConnectionManager* manager, PlanSearch* records, int max_count, int* actual_count);

#endif /* PLAN_SEARCH_ORM_H */