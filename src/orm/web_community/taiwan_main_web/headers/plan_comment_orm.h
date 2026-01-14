#ifndef PLAN_COMMENT_ORM_H
#define PLAN_COMMENT_ORM_H

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
} PlanComment;

/* CRUD Operations */
int PlanComment_Add(DBConnectionManager* manager, const PlanComment* record);
int PlanComment_Get(DBConnectionManager* manager, int no, PlanComment* record);
int PlanComment_Update(DBConnectionManager* manager, const PlanComment* record);
int PlanComment_Delete(DBConnectionManager* manager, int no);
int PlanComment_Exists(DBConnectionManager* manager, int no);
int PlanComment_GetAll(DBConnectionManager* manager, PlanComment* records, int max_count, int* actual_count);

#endif /* PLAN_COMMENT_ORM_H */