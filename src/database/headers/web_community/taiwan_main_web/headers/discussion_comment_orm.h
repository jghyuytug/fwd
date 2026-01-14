#ifndef DISCUSSION_COMMENT_ORM_H
#define DISCUSSION_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    char comment_text[1024];
    char ip[16];
    int create_day;
    int modify_time;
    int modify_cnt;
} DiscussionComment;

/* CRUD Operations */
int DiscussionComment_Add(DBConnectionManager* manager, const DiscussionComment* record);
int DiscussionComment_Get(DBConnectionManager* manager, int no, DiscussionComment* record);
int DiscussionComment_Update(DBConnectionManager* manager, const DiscussionComment* record);
int DiscussionComment_Delete(DBConnectionManager* manager, int no);
int DiscussionComment_Exists(DBConnectionManager* manager, int no);
int DiscussionComment_GetAll(DBConnectionManager* manager, DiscussionComment* records, int max_count, int* actual_count);

#endif /* DISCUSSION_COMMENT_ORM_H */