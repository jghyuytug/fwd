#ifndef RDAR0_MYISAM_ORM_H
#define RDAR0_MYISAM_ORM_H

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
} Rdar0Myisam;

/* CRUD Operations */
int Rdar0Myisam_Add(DBConnectionManager* manager, const Rdar0Myisam* record);
int Rdar0Myisam_Get(DBConnectionManager* manager, int no, Rdar0Myisam* record);
int Rdar0Myisam_Update(DBConnectionManager* manager, const Rdar0Myisam* record);
int Rdar0Myisam_Delete(DBConnectionManager* manager, int no);
int Rdar0Myisam_Exists(DBConnectionManager* manager, int no);
int Rdar0Myisam_GetAll(DBConnectionManager* manager, Rdar0Myisam* records, int max_count, int* actual_count);

#endif /* RDAR0_MYISAM_ORM_H */