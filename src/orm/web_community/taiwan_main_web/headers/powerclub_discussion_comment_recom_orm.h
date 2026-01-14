#ifndef POWERCLUB_DISCUSSION_COMMENT_RECOM_ORM_H
#define POWERCLUB_DISCUSSION_COMMENT_RECOM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int comment_no;
    int m_id;
    int content_no;
} PowerclubDiscussionCommentRecom;

/* CRUD Operations */
int PowerclubDiscussionCommentRecom_Add(DBConnectionManager* manager, const PowerclubDiscussionCommentRecom* record);
int PowerclubDiscussionCommentRecom_Get(DBConnectionManager* manager, int no, PowerclubDiscussionCommentRecom* record);
int PowerclubDiscussionCommentRecom_Update(DBConnectionManager* manager, const PowerclubDiscussionCommentRecom* record);
int PowerclubDiscussionCommentRecom_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionCommentRecom_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionCommentRecom_GetAll(DBConnectionManager* manager, PowerclubDiscussionCommentRecom* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_COMMENT_RECOM_ORM_H */