#ifndef REALRADIO_EVENT_COMMENT_ORM_H
#define REALRADIO_EVENT_COMMENT_ORM_H

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
} RealradioEventComment;

/* CRUD Operations */
int RealradioEventComment_Add(DBConnectionManager* manager, const RealradioEventComment* record);
int RealradioEventComment_Get(DBConnectionManager* manager, int no, RealradioEventComment* record);
int RealradioEventComment_Update(DBConnectionManager* manager, const RealradioEventComment* record);
int RealradioEventComment_Delete(DBConnectionManager* manager, int no);
int RealradioEventComment_Exists(DBConnectionManager* manager, int no);
int RealradioEventComment_GetAll(DBConnectionManager* manager, RealradioEventComment* records, int max_count, int* actual_count);

#endif /* REALRADIO_EVENT_COMMENT_ORM_H */