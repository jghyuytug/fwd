#ifndef COMMUNITY_DISCUSSION_ASKING_LOG_ORM_H
#define COMMUNITY_DISCUSSION_ASKING_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int ask_no;
    int ask_list_no;
    int m_id;
    int reg_date;
} CommunityDiscussionAskingLog;

/* CRUD Operations */
int CommunityDiscussionAskingLog_Add(DBConnectionManager* manager, const CommunityDiscussionAskingLog* record);
int CommunityDiscussionAskingLog_Get(DBConnectionManager* manager, int no, CommunityDiscussionAskingLog* record);
int CommunityDiscussionAskingLog_Update(DBConnectionManager* manager, const CommunityDiscussionAskingLog* record);
int CommunityDiscussionAskingLog_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionAskingLog_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionAskingLog_GetAll(DBConnectionManager* manager, CommunityDiscussionAskingLog* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_ASKING_LOG_ORM_H */