#ifndef DISCUSSION_SEARCH_ETC_ORM_H
#define DISCUSSION_SEARCH_ETC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} DiscussionSearchEtc;

/* CRUD Operations */
int DiscussionSearchEtc_Add(DBConnectionManager* manager, const DiscussionSearchEtc* record);
int DiscussionSearchEtc_GetAll(DBConnectionManager* manager, DiscussionSearchEtc* records, int max_count, int* actual_count);

#endif /* DISCUSSION_SEARCH_ETC_ORM_H */