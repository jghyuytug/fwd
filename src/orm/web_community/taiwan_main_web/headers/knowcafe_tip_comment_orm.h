#ifndef KNOWCAFE_TIP_COMMENT_ORM_H
#define KNOWCAFE_TIP_COMMENT_ORM_H

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
} KnowcafeTipComment;

/* CRUD Operations */
int KnowcafeTipComment_Add(DBConnectionManager* manager, const KnowcafeTipComment* record);
int KnowcafeTipComment_Get(DBConnectionManager* manager, int no, KnowcafeTipComment* record);
int KnowcafeTipComment_Update(DBConnectionManager* manager, const KnowcafeTipComment* record);
int KnowcafeTipComment_Delete(DBConnectionManager* manager, int no);
int KnowcafeTipComment_Exists(DBConnectionManager* manager, int no);
int KnowcafeTipComment_GetAll(DBConnectionManager* manager, KnowcafeTipComment* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_TIP_COMMENT_ORM_H */