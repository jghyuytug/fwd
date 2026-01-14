#ifndef COMMUNITY_DISCUSSION_CONTENT_ORM_H
#define COMMUNITY_DISCUSSION_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type[2];
    char ip[16];
} CommunityDiscussionContent;

/* CRUD Operations */
int CommunityDiscussionContent_Add(DBConnectionManager* manager, const CommunityDiscussionContent* record);
int CommunityDiscussionContent_Get(DBConnectionManager* manager, int no, CommunityDiscussionContent* record);
int CommunityDiscussionContent_Update(DBConnectionManager* manager, const CommunityDiscussionContent* record);
int CommunityDiscussionContent_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionContent_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionContent_GetAll(DBConnectionManager* manager, CommunityDiscussionContent* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_CONTENT_ORM_H */