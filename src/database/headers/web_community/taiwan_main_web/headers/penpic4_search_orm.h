#ifndef PENPIC4_SEARCH_ORM_H
#define PENPIC4_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char penpic_type;
    signed char category;
    char class;
    char word[33];
    signed char degree;
} Penpic4Search;

/* CRUD Operations */
int Penpic4Search_Add(DBConnectionManager* manager, const Penpic4Search* record);
int Penpic4Search_GetAll(DBConnectionManager* manager, Penpic4Search* records, int max_count, int* actual_count);

#endif /* PENPIC4_SEARCH_ORM_H */