#ifndef WIKI_MNG_ENCYCLOPEDIA_SEARCH_ORM_H
#define WIKI_MNG_ENCYCLOPEDIA_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char category;
    signed char class;
    char word[33];
} WikiMngEncyclopediaSearch;

/* CRUD Operations */
int WikiMngEncyclopediaSearch_Add(DBConnectionManager* manager, const WikiMngEncyclopediaSearch* record);
int WikiMngEncyclopediaSearch_GetAll(DBConnectionManager* manager, WikiMngEncyclopediaSearch* records, int max_count, int* actual_count);

#endif /* WIKI_MNG_ENCYCLOPEDIA_SEARCH_ORM_H */