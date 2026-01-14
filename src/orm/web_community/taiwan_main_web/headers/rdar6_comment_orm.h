#ifndef RDAR6_COMMENT_ORM_H
#define RDAR6_COMMENT_ORM_H

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
} Rdar6Comment;

/* CRUD Operations */
int Rdar6Comment_Add(DBConnectionManager* manager, const Rdar6Comment* record);
int Rdar6Comment_Get(DBConnectionManager* manager, int no, Rdar6Comment* record);
int Rdar6Comment_Update(DBConnectionManager* manager, const Rdar6Comment* record);
int Rdar6Comment_Delete(DBConnectionManager* manager, int no);
int Rdar6Comment_Exists(DBConnectionManager* manager, int no);
int Rdar6Comment_GetAll(DBConnectionManager* manager, Rdar6Comment* records, int max_count, int* actual_count);

#endif /* RDAR6_COMMENT_ORM_H */