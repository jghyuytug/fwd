#ifndef POWERCLUB_DISCUSSION_SEARCH_ORM_H
#define POWERCLUB_DISCUSSION_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} PowerclubDiscussionSearch;

/* CRUD Operations */
int PowerclubDiscussionSearch_Add(DBConnectionManager* manager, const PowerclubDiscussionSearch* record);
int PowerclubDiscussionSearch_GetAll(DBConnectionManager* manager, PowerclubDiscussionSearch* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_SEARCH_ORM_H */