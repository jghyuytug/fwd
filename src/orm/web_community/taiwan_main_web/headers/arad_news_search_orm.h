#ifndef ARAD_NEWS_SEARCH_ORM_H
#define ARAD_NEWS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} AradNewsSearch;

/* CRUD Operations */
int AradNewsSearch_Add(DBConnectionManager* manager, const AradNewsSearch* record);
int AradNewsSearch_GetAll(DBConnectionManager* manager, AradNewsSearch* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_SEARCH_ORM_H */