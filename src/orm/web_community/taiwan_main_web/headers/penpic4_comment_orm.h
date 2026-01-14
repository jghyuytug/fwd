#ifndef PENPIC4_COMMENT_ORM_H
#define PENPIC4_COMMENT_ORM_H

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
} Penpic4Comment;

/* CRUD Operations */
int Penpic4Comment_Add(DBConnectionManager* manager, const Penpic4Comment* record);
int Penpic4Comment_Get(DBConnectionManager* manager, int no, Penpic4Comment* record);
int Penpic4Comment_Update(DBConnectionManager* manager, const Penpic4Comment* record);
int Penpic4Comment_Delete(DBConnectionManager* manager, int no);
int Penpic4Comment_Exists(DBConnectionManager* manager, int no);
int Penpic4Comment_GetAll(DBConnectionManager* manager, Penpic4Comment* records, int max_count, int* actual_count);

#endif /* PENPIC4_COMMENT_ORM_H */