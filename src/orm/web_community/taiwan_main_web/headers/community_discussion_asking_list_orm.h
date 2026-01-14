#ifndef COMMUNITY_DISCUSSION_ASKING_LIST_ORM_H
#define COMMUNITY_DISCUSSION_ASKING_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int ask_no;
    int recom;
    int reg_date;
    char content[256];
} CommunityDiscussionAskingList;

/* CRUD Operations */
int CommunityDiscussionAskingList_Add(DBConnectionManager* manager, const CommunityDiscussionAskingList* record);
int CommunityDiscussionAskingList_Get(DBConnectionManager* manager, int no, CommunityDiscussionAskingList* record);
int CommunityDiscussionAskingList_Update(DBConnectionManager* manager, const CommunityDiscussionAskingList* record);
int CommunityDiscussionAskingList_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionAskingList_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionAskingList_GetAll(DBConnectionManager* manager, CommunityDiscussionAskingList* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_ASKING_LIST_ORM_H */