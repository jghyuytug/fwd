#ifndef FOCUS_GAME_SEARCH_ORM_H
#define FOCUS_GAME_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} FocusGameSearch;

/* CRUD Operations */
int FocusGameSearch_Add(DBConnectionManager* manager, const FocusGameSearch* record);
int FocusGameSearch_GetAll(DBConnectionManager* manager, FocusGameSearch* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_SEARCH_ORM_H */