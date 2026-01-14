#ifndef KNOWSTANDBY_SEARCH_ORM_H
#define KNOWSTANDBY_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    long long kn_id;
    char word[33];
} KnowstandbySearch;

/* CRUD Operations */
int KnowstandbySearch_Add(DBConnectionManager* manager, const KnowstandbySearch* record);
int KnowstandbySearch_GetAll(DBConnectionManager* manager, KnowstandbySearch* records, int max_count, int* actual_count);

#endif /* KNOWSTANDBY_SEARCH_ORM_H */