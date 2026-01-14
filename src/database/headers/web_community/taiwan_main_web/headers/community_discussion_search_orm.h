#ifndef COMMUNITY_DISCUSSION_SEARCH_ORM_H
#define COMMUNITY_DISCUSSION_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class[2];
    char word[33];
} CommunityDiscussionSearch;

/* CRUD Operations */
int CommunityDiscussionSearch_Add(DBConnectionManager* manager, const CommunityDiscussionSearch* record);
int CommunityDiscussionSearch_Get(DBConnectionManager* manager, int no, signed char category, const char* class, const char* word, CommunityDiscussionSearch* record);
int CommunityDiscussionSearch_Update(DBConnectionManager* manager, const CommunityDiscussionSearch* record);
int CommunityDiscussionSearch_Delete(DBConnectionManager* manager, int no, signed char category, const char* class, const char* word);
int CommunityDiscussionSearch_Exists(DBConnectionManager* manager, int no, signed char category, const char* class, const char* word);
int CommunityDiscussionSearch_GetAll(DBConnectionManager* manager, CommunityDiscussionSearch* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_SEARCH_ORM_H */