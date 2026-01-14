#ifndef KNOWCAFE_JOB_SEARCH_ORM_H
#define KNOWCAFE_JOB_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} KnowcafeJobSearch;

/* CRUD Operations */
int KnowcafeJobSearch_Add(DBConnectionManager* manager, const KnowcafeJobSearch* record);
int KnowcafeJobSearch_GetAll(DBConnectionManager* manager, KnowcafeJobSearch* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_JOB_SEARCH_ORM_H */