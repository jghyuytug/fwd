#ifndef TCG_PUBLIC_INFO_SEARCH_ORM_H
#define TCG_PUBLIC_INFO_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} TcgPublicInfoSearch;

/* CRUD Operations */
int TcgPublicInfoSearch_Add(DBConnectionManager* manager, const TcgPublicInfoSearch* record);
int TcgPublicInfoSearch_GetAll(DBConnectionManager* manager, TcgPublicInfoSearch* records, int max_count, int* actual_count);

#endif /* TCG_PUBLIC_INFO_SEARCH_ORM_H */