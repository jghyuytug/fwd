#ifndef RDAR0_ORM_H
#define RDAR0_ORM_H

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
} Rdar0;

/* CRUD Operations */
int Rdar0_Add(DBConnectionManager* manager, const Rdar0* record);
int Rdar0_Get(DBConnectionManager* manager, int no, Rdar0* record);
int Rdar0_Update(DBConnectionManager* manager, const Rdar0* record);
int Rdar0_Delete(DBConnectionManager* manager, int no);
int Rdar0_Exists(DBConnectionManager* manager, int no);
int Rdar0_GetAll(DBConnectionManager* manager, Rdar0* records, int max_count, int* actual_count);

#endif /* RDAR0_ORM_H */