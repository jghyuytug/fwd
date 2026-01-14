#ifndef PENPIC_COMMENT_ORM_H
#define PENPIC_COMMENT_ORM_H

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
    signed char point;
    char ip[16];
    int create_day;
} PenpicComment;

/* CRUD Operations */
int PenpicComment_Add(DBConnectionManager* manager, const PenpicComment* record);
int PenpicComment_Get(DBConnectionManager* manager, int no, PenpicComment* record);
int PenpicComment_Update(DBConnectionManager* manager, const PenpicComment* record);
int PenpicComment_Delete(DBConnectionManager* manager, int no);
int PenpicComment_Exists(DBConnectionManager* manager, int no);
int PenpicComment_GetAll(DBConnectionManager* manager, PenpicComment* records, int max_count, int* actual_count);

#endif /* PENPIC_COMMENT_ORM_H */