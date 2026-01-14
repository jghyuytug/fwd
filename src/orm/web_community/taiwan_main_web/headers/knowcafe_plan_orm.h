#ifndef KNOWCAFE_PLAN_ORM_H
#define KNOWCAFE_PLAN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char sub_job;
    signed char lev;
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    int file;
} KnowcafePlan;

/* CRUD Operations */
int KnowcafePlan_Add(DBConnectionManager* manager, const KnowcafePlan* record);
int KnowcafePlan_Get(DBConnectionManager* manager, int no, KnowcafePlan* record);
int KnowcafePlan_Update(DBConnectionManager* manager, const KnowcafePlan* record);
int KnowcafePlan_Delete(DBConnectionManager* manager, int no);
int KnowcafePlan_Exists(DBConnectionManager* manager, int no);
int KnowcafePlan_GetAll(DBConnectionManager* manager, KnowcafePlan* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_PLAN_ORM_H */