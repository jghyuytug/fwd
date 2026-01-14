#ifndef REALRADIO_ORM_H
#define REALRADIO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_type;
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
} Realradio;

/* CRUD Operations */
int Realradio_Add(DBConnectionManager* manager, const Realradio* record);
int Realradio_Get(DBConnectionManager* manager, int no, Realradio* record);
int Realradio_Update(DBConnectionManager* manager, const Realradio* record);
int Realradio_Delete(DBConnectionManager* manager, int no);
int Realradio_Exists(DBConnectionManager* manager, int no);
int Realradio_GetAll(DBConnectionManager* manager, Realradio* records, int max_count, int* actual_count);

#endif /* REALRADIO_ORM_H */