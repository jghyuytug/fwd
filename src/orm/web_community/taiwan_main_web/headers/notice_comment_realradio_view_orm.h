#ifndef NOTICE_COMMENT_REALRADIO_VIEW_ORM_H
#define NOTICE_COMMENT_REALRADIO_VIEW_ORM_H

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
} NoticeCommentRealradioView;

/* CRUD Operations */
int NoticeCommentRealradioView_Add(DBConnectionManager* manager, const NoticeCommentRealradioView* record);
int NoticeCommentRealradioView_Get(DBConnectionManager* manager, int no, NoticeCommentRealradioView* record);
int NoticeCommentRealradioView_Update(DBConnectionManager* manager, const NoticeCommentRealradioView* record);
int NoticeCommentRealradioView_Delete(DBConnectionManager* manager, int no);
int NoticeCommentRealradioView_Exists(DBConnectionManager* manager, int no);
int NoticeCommentRealradioView_GetAll(DBConnectionManager* manager, NoticeCommentRealradioView* records, int max_count, int* actual_count);

#endif /* NOTICE_COMMENT_REALRADIO_VIEW_ORM_H */