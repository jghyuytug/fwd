#ifndef INTENSIVE_DISCUSSION_SEARCH_NEW_ORM_H
#define INTENSIVE_DISCUSSION_SEARCH_NEW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} IntensiveDiscussionSearchNew;

/* CRUD Operations */
int IntensiveDiscussionSearchNew_Add(DBConnectionManager* manager, const IntensiveDiscussionSearchNew* record);
int IntensiveDiscussionSearchNew_GetAll(DBConnectionManager* manager, IntensiveDiscussionSearchNew* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_SEARCH_NEW_ORM_H */