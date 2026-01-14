#ifndef RDAR0_COMMENT_ORM_H
#define RDAR0_COMMENT_ORM_H

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
    char comment_text[256];
    char ip[16];
    int create_day;
} Rdar0Comment;

/* CRUD Operations */
int Rdar0Comment_Add(DBConnectionManager* manager, const Rdar0Comment* record);
int Rdar0Comment_Get(DBConnectionManager* manager, int no, Rdar0Comment* record);
int Rdar0Comment_Update(DBConnectionManager* manager, const Rdar0Comment* record);
int Rdar0Comment_Delete(DBConnectionManager* manager, int no);
int Rdar0Comment_Exists(DBConnectionManager* manager, int no);
int Rdar0Comment_GetAll(DBConnectionManager* manager, Rdar0Comment* records, int max_count, int* actual_count);

#endif /* RDAR0_COMMENT_ORM_H */