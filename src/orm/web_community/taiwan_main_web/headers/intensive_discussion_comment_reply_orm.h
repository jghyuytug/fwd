#ifndef INTENSIVE_DISCUSSION_COMMENT_REPLY_ORM_H
#define INTENSIVE_DISCUSSION_COMMENT_REPLY_ORM_H

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
    int sequence;
    signed char del_flag;
    int content_no;
} IntensiveDiscussionCommentReply;

/* CRUD Operations */
int IntensiveDiscussionCommentReply_Add(DBConnectionManager* manager, const IntensiveDiscussionCommentReply* record);
int IntensiveDiscussionCommentReply_Get(DBConnectionManager* manager, int rno, IntensiveDiscussionCommentReply* record);
int IntensiveDiscussionCommentReply_Update(DBConnectionManager* manager, const IntensiveDiscussionCommentReply* record);
int IntensiveDiscussionCommentReply_Delete(DBConnectionManager* manager, int rno);
int IntensiveDiscussionCommentReply_Exists(DBConnectionManager* manager, int rno);
int IntensiveDiscussionCommentReply_GetAll(DBConnectionManager* manager, IntensiveDiscussionCommentReply* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_COMMENT_REPLY_ORM_H */