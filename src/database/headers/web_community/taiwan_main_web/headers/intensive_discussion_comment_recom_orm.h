#ifndef INTENSIVE_DISCUSSION_COMMENT_RECOM_ORM_H
#define INTENSIVE_DISCUSSION_COMMENT_RECOM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int comment_no;
    int m_id;
    int content_no;
} IntensiveDiscussionCommentRecom;

/* CRUD Operations */
int IntensiveDiscussionCommentRecom_Add(DBConnectionManager* manager, const IntensiveDiscussionCommentRecom* record);
int IntensiveDiscussionCommentRecom_Get(DBConnectionManager* manager, int no, IntensiveDiscussionCommentRecom* record);
int IntensiveDiscussionCommentRecom_Update(DBConnectionManager* manager, const IntensiveDiscussionCommentRecom* record);
int IntensiveDiscussionCommentRecom_Delete(DBConnectionManager* manager, int no);
int IntensiveDiscussionCommentRecom_Exists(DBConnectionManager* manager, int no);
int IntensiveDiscussionCommentRecom_GetAll(DBConnectionManager* manager, IntensiveDiscussionCommentRecom* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_COMMENT_RECOM_ORM_H */