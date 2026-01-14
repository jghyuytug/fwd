#ifndef DISCUSSION_SEARCH_ORM_H
#define DISCUSSION_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} DiscussionSearch;

/* CRUD Operations */
int DiscussionSearch_Add(DBConnectionManager* manager, const DiscussionSearch* record);
int DiscussionSearch_GetAll(DBConnectionManager* manager, DiscussionSearch* records, int max_count, int* actual_count);

#endif /* DISCUSSION_SEARCH_ORM_H */