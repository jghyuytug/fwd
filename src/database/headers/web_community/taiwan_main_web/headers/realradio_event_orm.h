#ifndef REALRADIO_EVENT_ORM_H
#define REALRADIO_EVENT_ORM_H

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
    signed char open_flag;
} RealradioEvent;

/* CRUD Operations */
int RealradioEvent_Add(DBConnectionManager* manager, const RealradioEvent* record);
int RealradioEvent_Get(DBConnectionManager* manager, int no, RealradioEvent* record);
int RealradioEvent_Update(DBConnectionManager* manager, const RealradioEvent* record);
int RealradioEvent_Delete(DBConnectionManager* manager, int no);
int RealradioEvent_Exists(DBConnectionManager* manager, int no);
int RealradioEvent_GetAll(DBConnectionManager* manager, RealradioEvent* records, int max_count, int* actual_count);

#endif /* REALRADIO_EVENT_ORM_H */