#ifndef DICTIONARY_ITEM_SEARCH_ORM_H
#define DICTIONARY_ITEM_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    char search[101];
} DictionaryItemSearch;

/* CRUD Operations */
int DictionaryItemSearch_Add(DBConnectionManager* manager, const DictionaryItemSearch* record);
int DictionaryItemSearch_GetAll(DBConnectionManager* manager, DictionaryItemSearch* records, int max_count, int* actual_count);

#endif /* DICTIONARY_ITEM_SEARCH_ORM_H */