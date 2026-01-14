#ifndef POWERCLUB_DISCUSSION_COMMENT_ORM_H
#define POWERCLUB_DISCUSSION_COMMENT_ORM_H

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
} PowerclubDiscussionComment;

/* CRUD Operations */
int PowerclubDiscussionComment_Add(DBConnectionManager* manager, const PowerclubDiscussionComment* record);
int PowerclubDiscussionComment_Get(DBConnectionManager* manager, int no, PowerclubDiscussionComment* record);
int PowerclubDiscussionComment_Update(DBConnectionManager* manager, const PowerclubDiscussionComment* record);
int PowerclubDiscussionComment_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionComment_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionComment_GetAll(DBConnectionManager* manager, PowerclubDiscussionComment* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_COMMENT_ORM_H */