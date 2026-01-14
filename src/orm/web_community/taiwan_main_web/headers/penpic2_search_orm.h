#ifndef PENPIC2_SEARCH_ORM_H
#define PENPIC2_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char penpic_type;
    signed char category;
    char class;
    char word[33];
    signed char degree;
} Penpic2Search;

/* CRUD Operations */
int Penpic2Search_Add(DBConnectionManager* manager, const Penpic2Search* record);
int Penpic2Search_GetAll(DBConnectionManager* manager, Penpic2Search* records, int max_count, int* actual_count);

#endif /* PENPIC2_SEARCH_ORM_H */