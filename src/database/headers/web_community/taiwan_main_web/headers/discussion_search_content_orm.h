#ifndef DISCUSSION_SEARCH_CONTENT_ORM_H
#define DISCUSSION_SEARCH_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} DiscussionSearchContent;

/* CRUD Operations */
int DiscussionSearchContent_Add(DBConnectionManager* manager, const DiscussionSearchContent* record);
int DiscussionSearchContent_GetAll(DBConnectionManager* manager, DiscussionSearchContent* records, int max_count, int* actual_count);

#endif /* DISCUSSION_SEARCH_CONTENT_ORM_H */