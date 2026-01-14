#ifndef KNOWCAFE_QA_COMMENT_ORM_H
#define KNOWCAFE_QA_COMMENT_ORM_H

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
    char comment_text[1024];
    char ip[16];
    int create_day;
} KnowcafeQaComment;

/* CRUD Operations */
int KnowcafeQaComment_Add(DBConnectionManager* manager, const KnowcafeQaComment* record);
int KnowcafeQaComment_Get(DBConnectionManager* manager, int no, KnowcafeQaComment* record);
int KnowcafeQaComment_Update(DBConnectionManager* manager, const KnowcafeQaComment* record);
int KnowcafeQaComment_Delete(DBConnectionManager* manager, int no);
int KnowcafeQaComment_Exists(DBConnectionManager* manager, int no);
int KnowcafeQaComment_GetAll(DBConnectionManager* manager, KnowcafeQaComment* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_COMMENT_ORM_H */