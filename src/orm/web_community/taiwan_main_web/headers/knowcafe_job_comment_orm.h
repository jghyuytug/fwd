#ifndef KNOWCAFE_JOB_COMMENT_ORM_H
#define KNOWCAFE_JOB_COMMENT_ORM_H

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
} KnowcafeJobComment;

/* CRUD Operations */
int KnowcafeJobComment_Add(DBConnectionManager* manager, const KnowcafeJobComment* record);
int KnowcafeJobComment_Get(DBConnectionManager* manager, int no, KnowcafeJobComment* record);
int KnowcafeJobComment_Update(DBConnectionManager* manager, const KnowcafeJobComment* record);
int KnowcafeJobComment_Delete(DBConnectionManager* manager, int no);
int KnowcafeJobComment_Exists(DBConnectionManager* manager, int no);
int KnowcafeJobComment_GetAll(DBConnectionManager* manager, KnowcafeJobComment* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_JOB_COMMENT_ORM_H */