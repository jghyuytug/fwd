#ifndef NOTICE_ORM_H
#define NOTICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char bbs_name[11];
    int no;
    signed char category;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    signed char adorn;
    signed char adorn_color1;
    signed char adorn_color2;
    signed char depth;
    double sequence;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Notice;

/* CRUD Operations */
int Notice_Add(DBConnectionManager* manager, const Notice* record);
int Notice_GetAll(DBConnectionManager* manager, Notice* records, int max_count, int* actual_count);

#endif /* NOTICE_ORM_H */