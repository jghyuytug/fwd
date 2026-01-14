#ifndef RDAR0_SEARCH_CONTENT_ORM_H
#define RDAR0_SEARCH_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} Rdar0SearchContent;

/* CRUD Operations */
int Rdar0SearchContent_Add(DBConnectionManager* manager, const Rdar0SearchContent* record);
int Rdar0SearchContent_GetAll(DBConnectionManager* manager, Rdar0SearchContent* records, int max_count, int* actual_count);

#endif /* RDAR0_SEARCH_CONTENT_ORM_H */