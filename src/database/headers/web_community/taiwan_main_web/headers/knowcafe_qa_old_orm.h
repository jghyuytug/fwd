#ifndef KNOWCAFE_QA_OLD_ORM_H
#define KNOWCAFE_QA_OLD_ORM_H

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
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    signed char adorn;
    signed char adorn_color1;
    signed char adorn_color2;
    int file;
    signed char depth;
    double sequence;
    int reply;
} KnowcafeQaOld;

/* CRUD Operations */
int KnowcafeQaOld_Add(DBConnectionManager* manager, const KnowcafeQaOld* record);
int KnowcafeQaOld_Get(DBConnectionManager* manager, int no, KnowcafeQaOld* record);
int KnowcafeQaOld_Update(DBConnectionManager* manager, const KnowcafeQaOld* record);
int KnowcafeQaOld_Delete(DBConnectionManager* manager, int no);
int KnowcafeQaOld_Exists(DBConnectionManager* manager, int no);
int KnowcafeQaOld_GetAll(DBConnectionManager* manager, KnowcafeQaOld* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_OLD_ORM_H */