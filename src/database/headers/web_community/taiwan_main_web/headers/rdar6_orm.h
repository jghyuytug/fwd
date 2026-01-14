#ifndef RDAR6_ORM_H
#define RDAR6_ORM_H

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
} Rdar6;

/* CRUD Operations */
int Rdar6_Add(DBConnectionManager* manager, const Rdar6* record);
int Rdar6_Get(DBConnectionManager* manager, int no, Rdar6* record);
int Rdar6_Update(DBConnectionManager* manager, const Rdar6* record);
int Rdar6_Delete(DBConnectionManager* manager, int no);
int Rdar6_Exists(DBConnectionManager* manager, int no);
int Rdar6_GetAll(DBConnectionManager* manager, Rdar6* records, int max_count, int* actual_count);

#endif /* RDAR6_ORM_H */