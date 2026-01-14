#ifndef REALDRAMA_COMMENT_ORM_H
#define REALDRAMA_COMMENT_ORM_H

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
} RealdramaComment;

/* CRUD Operations */
int RealdramaComment_Add(DBConnectionManager* manager, const RealdramaComment* record);
int RealdramaComment_Get(DBConnectionManager* manager, int no, RealdramaComment* record);
int RealdramaComment_Update(DBConnectionManager* manager, const RealdramaComment* record);
int RealdramaComment_Delete(DBConnectionManager* manager, int no);
int RealdramaComment_Exists(DBConnectionManager* manager, int no);
int RealdramaComment_GetAll(DBConnectionManager* manager, RealdramaComment* records, int max_count, int* actual_count);

#endif /* REALDRAMA_COMMENT_ORM_H */