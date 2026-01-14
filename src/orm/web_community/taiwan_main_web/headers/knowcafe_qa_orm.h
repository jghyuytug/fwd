#ifndef KNOWCAFE_QA_ORM_H
#define KNOWCAFE_QA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    signed char adorn;
    signed char adorn_color1;
    signed char adorn_color2;
    int file;
    int reply;
} KnowcafeQa;

/* CRUD Operations */
int KnowcafeQa_Add(DBConnectionManager* manager, const KnowcafeQa* record);
int KnowcafeQa_Get(DBConnectionManager* manager, int no, KnowcafeQa* record);
int KnowcafeQa_Update(DBConnectionManager* manager, const KnowcafeQa* record);
int KnowcafeQa_Delete(DBConnectionManager* manager, int no);
int KnowcafeQa_Exists(DBConnectionManager* manager, int no);
int KnowcafeQa_GetAll(DBConnectionManager* manager, KnowcafeQa* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_ORM_H */