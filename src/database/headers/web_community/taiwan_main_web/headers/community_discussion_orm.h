#ifndef COMMUNITY_DISCUSSION_ORM_H
#define COMMUNITY_DISCUSSION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int ask_no;
    char notice[2];
    int m_id;
    char m_user_id[17];
    char m_sex[2];
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    int modi_day;
    short comment;
    int view;
    int recom;
    int no_recom;
    double sequence;
} CommunityDiscussion;

/* CRUD Operations */
int CommunityDiscussion_Add(DBConnectionManager* manager, const CommunityDiscussion* record);
int CommunityDiscussion_Get(DBConnectionManager* manager, int no, CommunityDiscussion* record);
int CommunityDiscussion_Update(DBConnectionManager* manager, const CommunityDiscussion* record);
int CommunityDiscussion_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussion_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussion_GetAll(DBConnectionManager* manager, CommunityDiscussion* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_ORM_H */