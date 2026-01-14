#ifndef ARAD_NEWS_COMMENT_ORM_H
#define ARAD_NEWS_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[256];
    int create_day;
} AradNewsComment;

/* CRUD Operations */
int AradNewsComment_Add(DBConnectionManager* manager, const AradNewsComment* record);
int AradNewsComment_Get(DBConnectionManager* manager, int no, AradNewsComment* record);
int AradNewsComment_Update(DBConnectionManager* manager, const AradNewsComment* record);
int AradNewsComment_Delete(DBConnectionManager* manager, int no);
int AradNewsComment_Exists(DBConnectionManager* manager, int no);
int AradNewsComment_GetAll(DBConnectionManager* manager, AradNewsComment* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_COMMENT_ORM_H */