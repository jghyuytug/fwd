#ifndef KNOWCAFE_JOB_ORM_H
#define KNOWCAFE_JOB_ORM_H

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
} KnowcafeJob;

/* CRUD Operations */
int KnowcafeJob_Add(DBConnectionManager* manager, const KnowcafeJob* record);
int KnowcafeJob_Get(DBConnectionManager* manager, int no, KnowcafeJob* record);
int KnowcafeJob_Update(DBConnectionManager* manager, const KnowcafeJob* record);
int KnowcafeJob_Delete(DBConnectionManager* manager, int no);
int KnowcafeJob_Exists(DBConnectionManager* manager, int no);
int KnowcafeJob_GetAll(DBConnectionManager* manager, KnowcafeJob* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_JOB_ORM_H */