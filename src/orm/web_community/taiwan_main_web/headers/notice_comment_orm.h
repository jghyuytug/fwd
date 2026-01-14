#ifndef NOTICE_COMMENT_ORM_H
#define NOTICE_COMMENT_ORM_H

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
} NoticeComment;

/* CRUD Operations */
int NoticeComment_Add(DBConnectionManager* manager, const NoticeComment* record);
int NoticeComment_Get(DBConnectionManager* manager, int no, NoticeComment* record);
int NoticeComment_Update(DBConnectionManager* manager, const NoticeComment* record);
int NoticeComment_Delete(DBConnectionManager* manager, int no);
int NoticeComment_Exists(DBConnectionManager* manager, int no);
int NoticeComment_GetAll(DBConnectionManager* manager, NoticeComment* records, int max_count, int* actual_count);

#endif /* NOTICE_COMMENT_ORM_H */