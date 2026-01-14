#ifndef RDAR6_SEARCH_ORM_H
#define RDAR6_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} Rdar6Search;

/* CRUD Operations */
int Rdar6Search_Add(DBConnectionManager* manager, const Rdar6Search* record);
int Rdar6Search_GetAll(DBConnectionManager* manager, Rdar6Search* records, int max_count, int* actual_count);

#endif /* RDAR6_SEARCH_ORM_H */