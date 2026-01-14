#ifndef PENPIC3_SEARCH_ORM_H
#define PENPIC3_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char penpic_type;
    signed char category;
    char class;
    char word[33];
    signed char degree;
} Penpic3Search;

/* CRUD Operations */
int Penpic3Search_Add(DBConnectionManager* manager, const Penpic3Search* record);
int Penpic3Search_GetAll(DBConnectionManager* manager, Penpic3Search* records, int max_count, int* actual_count);

#endif /* PENPIC3_SEARCH_ORM_H */