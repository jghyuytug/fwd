#ifndef INTENSIVE_DISCUSSION_COMMENT_ORM_H
#define INTENSIVE_DISCUSSION_COMMENT_ORM_H

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
    char comment_text[1024];
    char ip[16];
    int create_day;
    int recom;
    signed char opinion_mode;
    signed char del_flag;
    short comment_cnt;
} IntensiveDiscussionComment;

/* CRUD Operations */
int IntensiveDiscussionComment_Add(DBConnectionManager* manager, const IntensiveDiscussionComment* record);
int IntensiveDiscussionComment_Get(DBConnectionManager* manager, int no, IntensiveDiscussionComment* record);
int IntensiveDiscussionComment_Update(DBConnectionManager* manager, const IntensiveDiscussionComment* record);
int IntensiveDiscussionComment_Delete(DBConnectionManager* manager, int no);
int IntensiveDiscussionComment_Exists(DBConnectionManager* manager, int no);
int IntensiveDiscussionComment_GetAll(DBConnectionManager* manager, IntensiveDiscussionComment* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_COMMENT_ORM_H */