#ifndef COMMUNITY_DISCUSSION_ASKING_ORM_H
#define COMMUNITY_DISCUSSION_ASKING_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char title[121];
    char main_title[256];
    char content[1024];
    char content_type[2];
    int admin_no;
    int start_date;
    int end_date;
    int reg_date;
    int modi_date;
    signed char open;
    signed char main_open;
    int view;
    short comment;
} CommunityDiscussionAsking;

/* CRUD Operations */
int CommunityDiscussionAsking_Add(DBConnectionManager* manager, const CommunityDiscussionAsking* record);
int CommunityDiscussionAsking_Get(DBConnectionManager* manager, int no, CommunityDiscussionAsking* record);
int CommunityDiscussionAsking_Update(DBConnectionManager* manager, const CommunityDiscussionAsking* record);
int CommunityDiscussionAsking_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionAsking_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionAsking_GetAll(DBConnectionManager* manager, CommunityDiscussionAsking* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_ASKING_ORM_H */