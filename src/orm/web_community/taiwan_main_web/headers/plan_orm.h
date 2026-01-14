#ifndef PLAN_ORM_H
#define PLAN_ORM_H

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
    signed char sub_job;
    signed char lev;
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    int file;
} Plan;

/* CRUD Operations */
int Plan_Add(DBConnectionManager* manager, const Plan* record);
int Plan_Get(DBConnectionManager* manager, int no, Plan* record);
int Plan_Update(DBConnectionManager* manager, const Plan* record);
int Plan_Delete(DBConnectionManager* manager, int no);
int Plan_Exists(DBConnectionManager* manager, int no);
int Plan_GetAll(DBConnectionManager* manager, Plan* records, int max_count, int* actual_count);

#endif /* PLAN_ORM_H */