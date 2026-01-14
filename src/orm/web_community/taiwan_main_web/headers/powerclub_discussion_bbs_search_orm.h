#ifndef POWERCLUB_DISCUSSION_BBS_SEARCH_ORM_H
#define POWERCLUB_DISCUSSION_BBS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} PowerclubDiscussionBbsSearch;

/* CRUD Operations */
int PowerclubDiscussionBbsSearch_Add(DBConnectionManager* manager, const PowerclubDiscussionBbsSearch* record);
int PowerclubDiscussionBbsSearch_GetAll(DBConnectionManager* manager, PowerclubDiscussionBbsSearch* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_BBS_SEARCH_ORM_H */