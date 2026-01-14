#ifndef COMMUNITY_DISCUSSION_COMMENT_ORM_H
#define COMMUNITY_DISCUSSION_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    char m_user_id[17];
    char m_sex[2];
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[105];
    char ip[16];
    int create_day;
} CommunityDiscussionComment;

/* CRUD Operations */
int CommunityDiscussionComment_Add(DBConnectionManager* manager, const CommunityDiscussionComment* record);
int CommunityDiscussionComment_Get(DBConnectionManager* manager, int no, CommunityDiscussionComment* record);
int CommunityDiscussionComment_Update(DBConnectionManager* manager, const CommunityDiscussionComment* record);
int CommunityDiscussionComment_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionComment_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionComment_GetAll(DBConnectionManager* manager, CommunityDiscussionComment* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_COMMENT_ORM_H */