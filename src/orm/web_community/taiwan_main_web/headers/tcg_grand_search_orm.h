#ifndef TCG_GRAND_SEARCH_ORM_H
#define TCG_GRAND_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} TcgGrandSearch;

/* CRUD Operations */
int TcgGrandSearch_Add(DBConnectionManager* manager, const TcgGrandSearch* record);
int TcgGrandSearch_GetAll(DBConnectionManager* manager, TcgGrandSearch* records, int max_count, int* actual_count);

#endif /* TCG_GRAND_SEARCH_ORM_H */