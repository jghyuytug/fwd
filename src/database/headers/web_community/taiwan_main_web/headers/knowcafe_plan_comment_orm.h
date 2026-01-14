#ifndef KNOWCAFE_PLAN_COMMENT_ORM_H
#define KNOWCAFE_PLAN_COMMENT_ORM_H

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
    int create_day;
} KnowcafePlanComment;

/* CRUD Operations */
int KnowcafePlanComment_Add(DBConnectionManager* manager, const KnowcafePlanComment* record);
int KnowcafePlanComment_Get(DBConnectionManager* manager, int no, KnowcafePlanComment* record);
int KnowcafePlanComment_Update(DBConnectionManager* manager, const KnowcafePlanComment* record);
int KnowcafePlanComment_Delete(DBConnectionManager* manager, int no);
int KnowcafePlanComment_Exists(DBConnectionManager* manager, int no);
int KnowcafePlanComment_GetAll(DBConnectionManager* manager, KnowcafePlanComment* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_PLAN_COMMENT_ORM_H */