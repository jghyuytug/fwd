#ifndef COMMUNITY_DISCUSSION_RECOM_ORM_H
#define COMMUNITY_DISCUSSION_RECOM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int content_no;
    int remote_addr;
} CommunityDiscussionRecom;

/* CRUD Operations */
int CommunityDiscussionRecom_Add(DBConnectionManager* manager, const CommunityDiscussionRecom* record);
int CommunityDiscussionRecom_Get(DBConnectionManager* manager, int no, CommunityDiscussionRecom* record);
int CommunityDiscussionRecom_Update(DBConnectionManager* manager, const CommunityDiscussionRecom* record);
int CommunityDiscussionRecom_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionRecom_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionRecom_GetAll(DBConnectionManager* manager, CommunityDiscussionRecom* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_RECOM_ORM_H */