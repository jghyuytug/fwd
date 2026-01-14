#ifndef TCG_FREE_SEARCH_ORM_H
#define TCG_FREE_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} TcgFreeSearch;

/* CRUD Operations */
int TcgFreeSearch_Add(DBConnectionManager* manager, const TcgFreeSearch* record);
int TcgFreeSearch_GetAll(DBConnectionManager* manager, TcgFreeSearch* records, int max_count, int* actual_count);

#endif /* TCG_FREE_SEARCH_ORM_H */