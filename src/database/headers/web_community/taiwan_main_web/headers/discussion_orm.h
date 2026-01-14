#ifndef DISCUSSION_ORM_H
#define DISCUSSION_ORM_H

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
    char subject[121];
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
    short hold_day;
    int modify_time;
    int modify_cnt;
} Discussion;

/* CRUD Operations */
int Discussion_Add(DBConnectionManager* manager, const Discussion* record);
int Discussion_Get(DBConnectionManager* manager, int no, Discussion* record);
int Discussion_Update(DBConnectionManager* manager, const Discussion* record);
int Discussion_Delete(DBConnectionManager* manager, int no);
int Discussion_Exists(DBConnectionManager* manager, int no);
int Discussion_GetAll(DBConnectionManager* manager, Discussion* records, int max_count, int* actual_count);

#endif /* DISCUSSION_ORM_H */