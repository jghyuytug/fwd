#ifndef PENPIC2_ORM_H
#define PENPIC2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char penpic_type;
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
    int point;
    signed char adorn;
    signed char adorn_color1;
    signed char adorn_color2;
    signed char degree;
} Penpic2;

/* CRUD Operations */
int Penpic2_Add(DBConnectionManager* manager, const Penpic2* record);
int Penpic2_Get(DBConnectionManager* manager, int no, Penpic2* record);
int Penpic2_Update(DBConnectionManager* manager, const Penpic2* record);
int Penpic2_Delete(DBConnectionManager* manager, int no);
int Penpic2_Exists(DBConnectionManager* manager, int no);
int Penpic2_GetAll(DBConnectionManager* manager, Penpic2* records, int max_count, int* actual_count);

#endif /* PENPIC2_ORM_H */