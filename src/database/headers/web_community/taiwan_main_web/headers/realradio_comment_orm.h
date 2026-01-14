#ifndef REALRADIO_COMMENT_ORM_H
#define REALRADIO_COMMENT_ORM_H

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
    double sequence;
} RealradioComment;

/* CRUD Operations */
int RealradioComment_Add(DBConnectionManager* manager, const RealradioComment* record);
int RealradioComment_Get(DBConnectionManager* manager, int no, RealradioComment* record);
int RealradioComment_Update(DBConnectionManager* manager, const RealradioComment* record);
int RealradioComment_Delete(DBConnectionManager* manager, int no);
int RealradioComment_Exists(DBConnectionManager* manager, int no);
int RealradioComment_GetAll(DBConnectionManager* manager, RealradioComment* records, int max_count, int* actual_count);

#endif /* REALRADIO_COMMENT_ORM_H */