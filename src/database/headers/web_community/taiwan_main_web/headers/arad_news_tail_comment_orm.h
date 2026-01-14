#ifndef ARAD_NEWS_TAIL_COMMENT_ORM_H
#define ARAD_NEWS_TAIL_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char lev;
    char comment[1024];
    int guild_id;
    char guild_name[41];
    int school_id;
    char school_name[41];
    char userimg_path[61];
    char blog_url[129];
} AradNewsTailComment;

/* CRUD Operations */
int AradNewsTailComment_Add(DBConnectionManager* manager, const AradNewsTailComment* record);
int AradNewsTailComment_Get(DBConnectionManager* manager, int m_id, AradNewsTailComment* record);
int AradNewsTailComment_Update(DBConnectionManager* manager, const AradNewsTailComment* record);
int AradNewsTailComment_Delete(DBConnectionManager* manager, int m_id);
int AradNewsTailComment_Exists(DBConnectionManager* manager, int m_id);
int AradNewsTailComment_GetAll(DBConnectionManager* manager, AradNewsTailComment* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_TAIL_COMMENT_ORM_H */