#ifndef POWERCLUB_DISCUSSION_COMMENT_REPLY_ORM_H
#define POWERCLUB_DISCUSSION_COMMENT_REPLY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    int no;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[256];
    int create_day;
    signed char depth;
    double sequence;
    signed char del_flag;
    int content_no;
} PowerclubDiscussionCommentReply;

/* CRUD Operations */
int PowerclubDiscussionCommentReply_Add(DBConnectionManager* manager, const PowerclubDiscussionCommentReply* record);
int PowerclubDiscussionCommentReply_Get(DBConnectionManager* manager, int rno, PowerclubDiscussionCommentReply* record);
int PowerclubDiscussionCommentReply_Update(DBConnectionManager* manager, const PowerclubDiscussionCommentReply* record);
int PowerclubDiscussionCommentReply_Delete(DBConnectionManager* manager, int rno);
int PowerclubDiscussionCommentReply_Exists(DBConnectionManager* manager, int rno);
int PowerclubDiscussionCommentReply_GetAll(DBConnectionManager* manager, PowerclubDiscussionCommentReply* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_COMMENT_REPLY_ORM_H */