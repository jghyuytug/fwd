#ifndef OPENCAST_SEARCH_ORM_H
#define OPENCAST_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_no;
    char class;
    char word[33];
} OpencastSearch;

/* CRUD Operations */
int OpencastSearch_Add(DBConnectionManager* manager, const OpencastSearch* record);
int OpencastSearch_GetAll(DBConnectionManager* manager, OpencastSearch* records, int max_count, int* actual_count);

#endif /* OPENCAST_SEARCH_ORM_H */