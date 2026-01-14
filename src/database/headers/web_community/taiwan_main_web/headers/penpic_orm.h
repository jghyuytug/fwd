#ifndef PENPIC_ORM_H
#define PENPIC_ORM_H

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
} Penpic;

/* CRUD Operations */
int Penpic_Add(DBConnectionManager* manager, const Penpic* record);
int Penpic_Get(DBConnectionManager* manager, int no, Penpic* record);
int Penpic_Update(DBConnectionManager* manager, const Penpic* record);
int Penpic_Delete(DBConnectionManager* manager, int no);
int Penpic_Exists(DBConnectionManager* manager, int no);
int Penpic_GetAll(DBConnectionManager* manager, Penpic* records, int max_count, int* actual_count);

#endif /* PENPIC_ORM_H */