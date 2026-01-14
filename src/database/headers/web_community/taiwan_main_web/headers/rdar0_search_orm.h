#ifndef RDAR0_SEARCH_ORM_H
#define RDAR0_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} Rdar0Search;

/* CRUD Operations */
int Rdar0Search_Add(DBConnectionManager* manager, const Rdar0Search* record);
int Rdar0Search_GetAll(DBConnectionManager* manager, Rdar0Search* records, int max_count, int* actual_count);

#endif /* RDAR0_SEARCH_ORM_H */