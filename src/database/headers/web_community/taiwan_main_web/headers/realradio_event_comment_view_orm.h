#ifndef REALRADIO_EVENT_COMMENT_VIEW_ORM_H
#define REALRADIO_EVENT_COMMENT_VIEW_ORM_H

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
    char comment_text[105];
    char ip[16];
    int create_day;
} RealradioEventCommentView;

/* CRUD Operations */
int RealradioEventCommentView_Add(DBConnectionManager* manager, const RealradioEventCommentView* record);
int RealradioEventCommentView_Get(DBConnectionManager* manager, int no, RealradioEventCommentView* record);
int RealradioEventCommentView_Update(DBConnectionManager* manager, const RealradioEventCommentView* record);
int RealradioEventCommentView_Delete(DBConnectionManager* manager, int no);
int RealradioEventCommentView_Exists(DBConnectionManager* manager, int no);
int RealradioEventCommentView_GetAll(DBConnectionManager* manager, RealradioEventCommentView* records, int max_count, int* actual_count);

#endif /* REALRADIO_EVENT_COMMENT_VIEW_ORM_H */