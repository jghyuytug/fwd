#ifndef PENPIC_SEARCH_ORM_H
#define PENPIC_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char penpic_type;
    signed char category;
    char class;
    char word[33];
} PenpicSearch;

/* CRUD Operations */
int PenpicSearch_Add(DBConnectionManager* manager, const PenpicSearch* record);
int PenpicSearch_GetAll(DBConnectionManager* manager, PenpicSearch* records, int max_count, int* actual_count);

#endif /* PENPIC_SEARCH_ORM_H */