#ifndef FOCUS_SEARCH_ORM_H
#define FOCUS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} FocusSearch;

/* CRUD Operations */
int FocusSearch_Add(DBConnectionManager* manager, const FocusSearch* record);
int FocusSearch_GetAll(DBConnectionManager* manager, FocusSearch* records, int max_count, int* actual_count);

#endif /* FOCUS_SEARCH_ORM_H */