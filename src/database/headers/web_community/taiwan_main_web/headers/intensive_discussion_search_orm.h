#ifndef INTENSIVE_DISCUSSION_SEARCH_ORM_H
#define INTENSIVE_DISCUSSION_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} IntensiveDiscussionSearch;

/* CRUD Operations */
int IntensiveDiscussionSearch_Add(DBConnectionManager* manager, const IntensiveDiscussionSearch* record);
int IntensiveDiscussionSearch_GetAll(DBConnectionManager* manager, IntensiveDiscussionSearch* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_SEARCH_ORM_H */